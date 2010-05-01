//driver_portaudio.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifdef _DRIVER_PORTAUDIO_CAN_OPERATE

int driver_portaudio_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 struct driver_portaudio * self;
 PaSampleFormat fmt;
#ifdef ROAR_HAVE_LIBPABLIO
 long flags = PABLIO_WRITE;
#endif

 switch (info->bits) {
  case 8:
    switch (info->codec) {
     case ROAR_CODEC_PCM_S_LE:
     case ROAR_CODEC_PCM_S_BE:
     case ROAR_CODEC_PCM_S_PDP:
       fmt = paInt8;
      break;
     case ROAR_CODEC_PCM_U_LE:
     case ROAR_CODEC_PCM_U_BE:
     case ROAR_CODEC_PCM_U_PDP:
       fmt = paUInt8;
      break;
     default:
       return -1;
      break;
    }
   break;
  case 16:
    if ( info->codec != ROAR_CODEC_DEFAULT )
     return -1;
    fmt = paInt16;
   break;
  case 24:
    if ( info->codec != ROAR_CODEC_DEFAULT )
     return -1;
    fmt = paPackedInt24;
   break;
  case 32:
    if ( info->codec != ROAR_CODEC_DEFAULT )
     return -1;
    fmt = paInt32;
   break;
  default:
    return -1;
 }

 if ( (self = roar_mm_malloc(sizeof(struct driver_portaudio))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct driver_portaudio));

 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->inst  = self;
 inst->close = driver_portaudio_close;
 inst->write = driver_portaudio_write;

 Pa_Initialize();

#ifdef ROAR_HAVE_LIBPABLIO
 switch (info->channels) {
  case 1: flags |= PABLIO_MONO;   break;
  case 2: flags |= PABLIO_STEREO; break;
  default:
    roar_mm_free(self);
    Pa_Terminate();
    return -1;
 }

 if ( OpenAudioStream(&(self->ostream), info->rate, fmt, flags) != paNoError ) {
  roar_mm_free(self);
  Pa_Terminate();
  return -1;
 }

 return 0;
#else
 return -1;
#endif
}

int     driver_portaudio_close        (struct roar_vio_calls * vio) {
 struct driver_portaudio * self = vio->inst;

#ifdef ROAR_HAVE_LIBPABLIO
 CloseAudioStream(self->ostream);

 Pa_Terminate();

 roar_mm_free(self);

 return 0;
#else
 return -1;
#endif
}

ssize_t driver_portaudio_write        (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_portaudio * self = vio->inst;

 ROAR_DBG("driver_portaudio_write(vio=%p, buf=%p, count=%llu) = ?", vio, buf, (long long unsigned int)count);

#ifdef ROAR_HAVE_LIBPABLIO
 count /= self->ostream->bytesPerFrame; // TODO: FIXME: do not access private members
 ROAR_DBG("driver_portaudio_write(vio=%p, buf=%p, count=%llu) = ? // PABLIO mode", vio, buf, (long long unsigned int)count);
 return WriteAudioStream(self->ostream, buf, count) * self->ostream->bytesPerFrame;
#else
 return -1;
#endif
}

#endif

//ll
