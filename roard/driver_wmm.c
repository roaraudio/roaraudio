//driver_wmm.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_HAVE_LIBWINMM

static int _alloc_wave_headers(struct driver_wmm * self) {
  int bytesPerBlock = self->wavefmt.nBlockAlign * self->splPerBlock;
  /*   int bytes = internal->blocks * (sizeof(WAVEHDR) + bytesPerBlock); */
  int bytes = self->blocks * (sizeof(*self->wh) + bytesPerBlock);
  int res;
  MMRESULT mmres;

  self->bigbuffer = malloc(bytes);
  if (self->bigbuffer != NULL) {
    int i;
    BYTE * b;

    memset(self->bigbuffer,0,bytes);
    self->wh = self->bigbuffer;
    self->spl = (LPBYTE) (self->wh+self->blocks);
    for (i=0, b=self->spl; i<self->blocks; ++i, b+=bytesPerBlock) {
      self->wh[i].data = b;
      self->wh[i].wh.lpData = self->wh[i].data;
      self->wh[i].length = bytesPerBlock;
      self->wh[i].wh.dwBufferLength = self->wh[i].length;
      self->wh[i].wh.dwUser = (DWORD_PTR)self;
      mmres = waveOutPrepareHeader(self->hwo,
                                   &self->wh[i].wh,sizeof(WAVEHDR));
      if (MMSYSERR_NOERROR != mmres) {
        break;
      }
    }
    if (i<self->blocks) {
      while (--i >= 0) {
        waveOutUnprepareHeader(self->hwo,
                               &self->wh[i].wh,sizeof(WAVEHDR));
      }
      free(self->bigbuffer);
      self->wh        = 0;
      self->spl       = 0;
      self->bigbuffer = 0;
    } else {
      /* all ok ! */
    }
  }

  res = (self->bigbuffer != NULL);
  return res;
}

static int _get_free_block(struct driver_wmm * self);
static int _wait_wave_headers(struct driver_wmm * self, int wait_all) {
  int res = 1;

  while (self->sent_blocks > 0) {
    int n;
    _get_free_block(self);
    n = self->sent_blocks;
    if (n > 0) {
      unsigned int ms = (self->msPerBlock>>1)+1;
      if (wait_all) ms *= n;
      Sleep(ms);
    }
  }

  res &= !self->sent_blocks;
  return res;
}

static int _get_free_block(struct driver_wmm * self) {
  const int idx = self->widx;
  int ridx = self->ridx;

  while (self->wh[ridx].sent && !!(self->wh[ridx].wh.dwFlags & WHDR_DONE)) {
    /* block successfully sent to hardware, release it */
    /*debug("_ao_get_free_block: release block %d\n",ridx);*/
    self->wh[ridx].sent = 0;
    self->wh[ridx].wh.dwFlags &= ~WHDR_DONE;

    --self->full_blocks;
    if (self->full_blocks<0) {
      self->full_blocks = 0;
    }

    --self->sent_blocks;
    if (self->sent_blocks<0) {
      self->sent_blocks = 0;
    }
    if (++ridx >= self->blocks) ridx = 0;
  }
  self->ridx = ridx;

  return self->wh[idx].sent
    ? -1
    : idx;
}

static int _free_wave_headers(struct driver_wmm * self) {
  MMRESULT mmres;
  int res = 1;

  if (self->wh) {
    int i;

    /* Reset so we dont need to wait ... Just a satefy net
     * since _ao_wait_wave_headers() has been called once before.
     */
    mmres = waveOutReset(self->hwo);
    /* Wait again to be sure reseted waveheaders has been released. */
    _wait_wave_headers(self, 0);

    for (i=self->blocks; --i>=0; ) {
      mmres = waveOutUnprepareHeader(self->hwo,
                                     &self->wh[i].wh,sizeof(WAVEHDR));

      res &= mmres == MMSYSERR_NOERROR;
    }
    self->wh  = 0;
    self->spl = 0;
  }

  return res;
}

/* Send a block to audio hardware */
static int _send_block(struct driver_wmm * self, const int idx) {
  MMRESULT mmres;

  /* Satanity checks */
  if (self->wh[idx].sent) {
    return 0;
  }
  if (!!(self->wh[idx].wh.dwFlags & WHDR_DONE)) {
    return 0;
  }

  /* count <= 0, just pretend it's been sent */
  if (self->wh[idx].count <= 0) {
    self->wh[idx].sent = 2; /* set with 2 so we can track these special cases */
    self->wh[idx].wh.dwFlags |= WHDR_DONE;
    ++self->sent_blocks;
    return 1;
  }

  self->wh[idx].wh.dwBufferLength = self->wh[idx].count;
  self->wh[idx].count = 0;
  mmres = waveOutWrite(self->hwo,
                       &self->wh[idx].wh, sizeof(WAVEHDR));
  self->wh[idx].sent = (mmres == MMSYSERR_NOERROR);
  /*&& !(internal->wh[idx].wh.dwFlags & WHDR_DONE);*/
  self->sent_blocks += self->wh[idx].sent;
  return mmres == MMSYSERR_NOERROR;
}

int     driver_wmm_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 struct driver_wmm * self;
 WAVEFORMATEX wavefmt;
 MMRESULT mmres;

 if ( (self = malloc(sizeof(struct driver_wmm))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct driver_wmm));

 // VIO Setup:
 memset(inst, 0, sizeof(struct roar_vio_calls));
 inst->inst  = self;
 inst->close = driver_wmm_close_vio;
 inst->write = driver_wmm_write;

 // WMM Setup:
 memset(&wavefmt, 0, sizeof(wavefmt));

 info->codec = ROAR_CODEC_PCM_S_LE;

 self->wavefmt.wFormatTag      = WAVE_FORMAT_PCM;
 self->wavefmt.nChannels       = info->channels;
 self->wavefmt.wBitsPerSample  = info->bits;
 self->wavefmt.nSamplesPerSec  = info->rate;
 self->wavefmt.nBlockAlign     = (self->wavefmt.wBitsPerSample>>3)*self->wavefmt.nChannels;
 self->wavefmt.nAvgBytesPerSec = self->wavefmt.nSamplesPerSec*self->wavefmt.nBlockAlign;
 self->wavefmt.cbSize          = 0;

 /* $$$ later this should be optionnal parms */
  self->id          = WAVE_MAPPER;
  self->blocks      = 64;
  self->splPerBlock = 512;
  self->msPerBlock  =
    (self->splPerBlock * 1000 + info->rate - 1) / info->rate;

  mmres =
    waveOutOpen(&self->hwo,
                self->id,
                &self->wavefmt,
                (DWORD_PTR)0/* waveOutProc */,
                (DWORD_PTR)self,
                CALLBACK_NULL/* |WAVE_FORMAT_DIRECT */|WAVE_ALLOWSYNC);
 if ( mmres != MMSYSERR_NOERROR ) {
  driver_wmm_close_vio(inst);
  return -1;
 }

 // FIXME: do error checking
 waveOutGetID(self->hwo, &(self->id));

 _alloc_wave_headers(self);

 self->opened = 1;

 ROAR_DBG("driver_wmm_open_vio(*) = 0");

 return 0;
}

int     driver_wmm_close_vio(struct roar_vio_calls * vio) {
 struct driver_wmm * self;

 if ( vio == NULL )
  return -1;

 if ( (self = vio->inst) == NULL )
  return -1;

 if ( self->opened ) {
  _wait_wave_headers(self, 1);
  _free_wave_headers(self);
  waveOutClose(self->hwo);
 }

 free(self);

 ROAR_DBG("driver_wmm_close_vio(*) = 0");

 return 0;
}

ssize_t driver_wmm_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_wmm * self;
 ssize_t ret_ok = count;
 int ret = 1;

 ROAR_DBG("driver_wmm_write(vio=%p, buf=%p, count=%lu) = ?", vio, buf, (unsigned long)count);

 if ( vio == NULL )
  return -1;

 if ( (self = vio->inst) == NULL )
  return -1;

 if ( ! self->opened )
  return -1;

  while(ret && count > 0) {
    int n;
    const int idx = _get_free_block(self);

    if (idx == -1) {
      /* debug("sleep %dms, rem %d bytes\n",internal->msPerBlock,num_bytes); */
      Sleep(self->msPerBlock);
      continue;
    }

    /* Get free bytes in the block */
    n = self->wh[idx].wh.dwBufferLength
      - self->wh[idx].count;

    /*     debug("free in block %d : %d/%d\n", */
    /*    idx,n,internal->wh[idx].dwBufferLength); */

    /* Get amount to copy */
    if (n > (int)count) {
      n = count;
    }
    /*     debug("copy = %d\n",n); */



    /* Do copy */
    CopyMemory((char*)self->wh[idx].wh.lpData
               + self->wh[idx].count,
               buf, n);

    /* Updates pointers and counters */
    buf += n;
    count -= n;
    /*     debug("rem = %d\n",num_bytes); */
    self->wh[idx].count += n;

    /* Is this block full ? */
    if (self->wh[idx].count
        == self->wh[idx].wh.dwBufferLength) {
      ++self->full_blocks;
      /*       debug("blocks %d full, total:%d\n",internal->widx,internal->full_blocks); */
      if (++self->widx == self->blocks) {
        self->widx = 0;
      }
      ret = _send_block(self, idx);
    }
  }

 ROAR_DBG("driver_wmm_write(vio=%p, buf=%p, count=%lu): ret=%i", vio, buf, (unsigned long)count, ret);

  /*   debug("ao_wmm_play => %d rem => [%s]\n",num_bytes,ret?"success":"error"); */
  return ret > -1 ? ret_ok : -1;
}

#endif

//ll
