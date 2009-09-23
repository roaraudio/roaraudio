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

#if defined(ROAR_HAVE_H_MMSYSTEM) && defined(ROAR_TARGET_WIN32)

int     driver_wmm_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 struct driver_wmm * self;
 WAVEFORMATEX wavefmt;

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

 wavefmt.wFormatTag      = WAVE_FORMAT_PCM;
 wavefmt.nChannels       = info->channels;
 wavefmt.wBitsPerSample  = info->bits;
 wavefmt.nSamplesPerSec  = info->rate;
 wavefmt.nBlockAlign     = (wavefmt.wBitsPerSample>>3)*wavefmt.nChannels;
 wavefmt.nAvgBytesPerSec = wavefmt.nSamplesPerSec*wavefmt.nBlockAlign;
 wavefmt.cbSize          = 0;

 /* $$$ later this should be optionnal parms */
  self->blocks      = 64;
  self->splPerBlock = 512;
  self->msPerBlock  =
    (self->splPerBlock * 1000 + info->rate - 1) / info->rate;

 return 0;
}

int     driver_wmm_close_vio(struct roar_vio_calls * vio) {
 struct driver_wmm * self;

 if ( vio == NULL )
  return -1;

 if ( (self = vio->inst) == NULL )
  return -1;

 free(self);

 return 0;
}

ssize_t driver_wmm_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 return -1;
}

#endif

//ll
