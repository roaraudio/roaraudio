//vio_rtp.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

static const struct {
 int pt;
 struct roar_audio_info info;
} _g_rtp_pt[] = {
 {ROAR_RTP_PT_A_PCMU,      {.codec = ROAR_CODEC_MULAW,    .bits =  8, .rate =  8000, .channels = 1}},
 {ROAR_RTP_PT_A_PCMA,      {.codec = ROAR_CODEC_ALAW,     .bits =  8, .rate =  8000, .channels = 1}},
 {ROAR_RTP_PT_A_L16_441_2, {.codec = ROAR_CODEC_PCM_S_BE, .bits = 16, .rate = 44100, .channels = 2}},
 {ROAR_RTP_PT_A_L16_441_1, {.codec = ROAR_CODEC_PCM_S_BE, .bits = 16, .rate = 44100, .channels = 1}},
 {-1, {-1, -1, -1}}
};

static int _info2pt (struct roar_audio_info * info) {
 int i;

 ROAR_DBG("_info2pt(info=%p{.codec=%s(%i), .bits=%i, .rate=%i, .channels=%i}) = ?", info, roar_codec2str(info->codec), info->codec, info->bits, info->rate, info->channels);

 for (i = 0; _g_rtp_pt[i].pt != -1; i++) {
  if ( info->codec    == _g_rtp_pt[i].info.codec    &&
       info->bits     == _g_rtp_pt[i].info.bits     &&
       info->rate     == _g_rtp_pt[i].info.rate     &&
       info->channels == _g_rtp_pt[i].info.channels ) {
   return _g_rtp_pt[i].pt;
  }
 }

 ROAR_DBG("_info2pt(info=%p{.codec=%s(%i), .bits=%i, .rate=%i, .channels=%i}) = -1", info, roar_codec2str(info->codec), info->codec, info->bits, info->rate, info->channels);
 return -1;
}

static const struct roar_audio_info * _pt2info (int pt) {
 int i;

 for (i = 0; _g_rtp_pt[i].pt != -1; i++) {
  if ( _g_rtp_pt[i].pt == pt ) {
   return &(_g_rtp_pt[i].info);
  }
 }

 return NULL;
}

int roar_vio_open_rtp        (struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                              char * dstr, struct roar_vio_defaults * odef) {
 struct roar_rtp_inst * self = NULL;

 ROAR_DBG("roar_vio_open_rtp(calls=%p, dst=%p, dstr='%s', odef=%p) = ?", calls, dst, dstr, odef);

 if ( calls == NULL || dst == NULL )
  return -1;

 if ( (self = roar_mm_malloc(sizeof(struct roar_rtp_inst))) == NULL )
  return -1;

 ROAR_DBG("roar_vio_open_rtp(calls=%p, dst=%p, dstr='%s', odef=%p) = ?", calls, dst, dstr, odef);

 memset(self, 0, sizeof(struct roar_rtp_inst));

 self->vio                 = dst;
 self->bpf                 = 0;
 self->mtu                 = 768;

 memset(&(self->info), 0, sizeof(struct roar_audio_info));

 self->header.version      = 2;
 self->header.payload_type = ROAR_RTP_PT_UNKNOWN;

 // TODO: init with random values:
 //       Sequence Number 
 //       ts
 //       SSRC

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst               = self;
// calls->read               = roar_vio_rtp_read;
 calls->write              = roar_vio_rtp_write;
// calls->lseek              = roar_vio_rtp_lseek;
 calls->nonblock           = roar_vio_rtp_nonblock;
 calls->sync               = roar_vio_rtp_sync;
 calls->ctl                = roar_vio_rtp_ctl;
 calls->close              = roar_vio_rtp_close;

 ROAR_DBG("roar_vio_open_rtp(calls=%p, dst=%p, dstr='%s', odef=%p) = 0", calls, dst, dstr, odef);

 return 0;
}

ssize_t roar_vio_rtp_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_rtp_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_rtp_inst * self = vio->inst;
 size_t len_need = count + sizeof(struct roar_rtp_header); // this is a bit more than we need
                                                           // we ignore this at the moment
 size_t len_have;
 union {
  void     * vp;
  char     * cp;
  uint16_t * u16;
  uint32_t * u32;
 } data;
 size_t  dataoffset;
 ssize_t ret;
 int     i;

 ROAR_DBG("roar_vio_rtp_write(vio=%p, buf=%p, count=%llu) = ?", vio, buf, (long long unsigned)count);

 if ( self->tx == NULL ) {
  if ( roar_buffer_new(&(self->tx), len_need) == -1 )
   return -1;

  len_have = len_need;
 } else {
  if ( roar_buffer_get_len(self->tx, &len_have) == -1 )
   return -1;

  if ( len_have < len_need ) {
   if ( roar_buffer_set_len(self->tx, len_need) == -1 ) {
    if ( roar_buffer_free(self->tx) == -1 )
     return -1;

    self->tx = NULL;
    return roar_vio_rtp_write(vio, buf, count); // restart ower self from the beginning with no buffer
   }
  }
 }

 ROAR_DBG("roar_vio_rtp_write(vio=%p, buf=%p, count=%llu) = ?", vio, buf, (long long unsigned)count);

 if ( roar_buffer_get_data(self->tx, &(data.vp)) == -1 )
  return -1;

 memset(data.vp, 0, len_need);

 self->header.seq_num++;

 data.cp[0]   = 2;
 data.cp[0]  |= self->header.csrc_count   << 4;
 data.cp[1]  |= self->header.payload_type << 1;

 data.u16[1]  = self->header.seq_num;

 data.u32[1]  = self->header.ts;
 data.u32[2]  = self->header.ssrc;

 for (i = 0; i < self->header.csrc_count; i++) {
  data.u32[3+i] = self->header.csrc[i];
 }

 dataoffset   = 3*4 + self->header.csrc_count*4;

 memcpy(data.vp + dataoffset, buf, count);

 ROAR_DBG("roar_vio_rtp_write(vio=%p, buf=%p, count=%llu) = ?", vio, buf, (long long unsigned)count);

 if ( (ret = roar_vio_write(self->vio, data.vp, count+dataoffset)) == -1 )
  return -1;

 len_have = ret - dataoffset;

 self->header.ts += len_have / self->bpf;

 return len_have;
}

off_t   roar_vio_rtp_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);

int     roar_vio_rtp_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_rtp_inst * self = vio->inst;

 ROAR_DBG("roar_vio_rtp_nonblock(vio=%p, state=%i) = ?", vio, state);

 return roar_vio_nonblock(self->vio, state);
}

int     roar_vio_rtp_sync    (struct roar_vio_calls * vio) {
 struct roar_rtp_inst * self = vio->inst;

 ROAR_DBG("roar_vio_rtp_sync(vio=%p) = ?", vio);

 return roar_vio_sync(self->vio);
}

int     roar_vio_rtp_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 struct roar_rtp_inst * self = vio->inst;
 struct roar_stream          * s  = NULL;
 struct roar_stream_server   * ss = NULL;

 ROAR_DBG("roar_vio_rtp_ctl(vio=%p, cmd=%i, data=%p) = ?", vio, cmd, data);

 if (vio == NULL || cmd == -1)
  return -1;

 ROAR_DBG("roar_vio_rtp_ctl(vio=%p, cmd=%i, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_SET_SSTREAM:
    s = ROAR_STREAM(ss = data);
   break;
  case ROAR_VIO_CTL_SET_STREAM:
    s = ROAR_STREAM(data);
   break;
  case ROAR_VIO_CTL_GET_NEXT:
    *(struct roar_vio_calls **)data = self->vio;
    return 0;
   break;
  case ROAR_VIO_CTL_SET_NEXT:
    self->vio = *(struct roar_vio_calls **)data;
    return 0;
   break;
 }

 ROAR_DBG("roar_vio_rtp_ctl(vio=%p, cmd=%i, data=%p) = ?", vio, cmd, data);

 if ( s != NULL ) {
  switch (s->info.codec) {
   case ROAR_CODEC_PCM_S_LE:
   case ROAR_CODEC_PCM_S_PDP:
     s->info.codec = ROAR_CODEC_PCM_S_BE;
    break;
   case ROAR_CODEC_PCM_U_LE:
   case ROAR_CODEC_PCM_U_PDP:
     s->info.codec = ROAR_CODEC_PCM_U_BE;
    break;
  }

  memcpy(&(self->info), &(s->info), sizeof(struct roar_audio_info));

  self->header.payload_type = _info2pt(&(s->info));

  self->bpf                 = s->info.channels * s->info.bits / 8;

  roar_vio_ctl(self->vio, cmd, data);
  return 0;
 }

 return roar_vio_ctl(self->vio, cmd, data);
}

int     roar_vio_rtp_close   (struct roar_vio_calls * vio) {
 struct roar_rtp_inst * self = vio->inst;
 int ret;

 ROAR_DBG("roar_vio_rtp_close(vio=%p) = ?", vio);

 ret = roar_vio_close(self->vio);

 if ( self->tx != NULL )
  if ( roar_buffer_free(self->tx) == -1 )
   ret = -1;

 roar_mm_free(self);

 ROAR_DBG("roar_vio_rtp_close(vio=%p) = %i", vio, ret);

 return ret;
}

//ll
