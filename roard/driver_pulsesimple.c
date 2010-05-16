//driver_pulsesimple.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#ifdef ROAR_HAVE_LIBPULSE

int     driver_pulsesimple_open         (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 struct driver_pulsesimple * self;
 pa_stream_direction_t dir = PA_STREAM_PLAYBACK;
 pa_sample_spec        ss;

//    pa_sample_format_t format;     /**< The sample format */

 switch (info->codec) {
  case ROAR_CODEC_ALAW:
    ss.format = PA_SAMPLE_ALAW;
   break;
  case ROAR_CODEC_MULAW:
    ss.format = PA_SAMPLE_ULAW;
   break;
  case ROAR_CODEC_PCM_S_LE:
    switch (info->bits) {
     case 16:
       ss.format = PA_SAMPLE_S16LE;
      break;
     default:
       return -1;
      break;
    }
   break;
  case ROAR_CODEC_PCM_S_BE:
    switch (info->bits) {
     case 16:
       ss.format = PA_SAMPLE_S16BE;
      break;
     default:
       return -1;
      break;
    }
   break;
  case ROAR_CODEC_PCM_U_LE:
  case ROAR_CODEC_PCM_U_BE:
  case ROAR_CODEC_PCM_U_PDP:
    if ( info->bits == 8 ) {
     ss.format = PA_SAMPLE_U8;
    } else {
     return -1;
    }
   break;
  default:
    return -1;
   break;
 }

 ss.rate     = info->rate;
 ss.channels = info->channels;

 if ( (self = roar_mm_malloc(sizeof(struct driver_pulsesimple))) == NULL )
  return -1;

 self->handle = pa_simple_new(device, "roard", dir, NULL, "RoarAudio Sound Server", &ss, NULL, NULL, NULL);

 if ( self->handle == NULL ) {
  roar_mm_free(self);
  return -1;
 }

 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->inst  = self;
 inst->close = driver_pulsesimple_close;
 inst->write = driver_pulsesimple_write;
 inst->sync  = driver_pulsesimple_sync;
 inst->ctl   = driver_pulsesimple_ctl;

 return 0;
}

int     driver_pulsesimple_close        (struct roar_vio_calls * vio) {
 struct driver_pulsesimple * self = vio->inst;

 pa_simple_free(self->handle);

 roar_mm_free(self);

 return 0;
}

ssize_t driver_pulsesimple_write        (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_pulsesimple * self = vio->inst;

 return pa_simple_write(self->handle, buf, count, NULL);
}

int     driver_pulsesimple_sync         (struct roar_vio_calls * vio) {
 struct driver_pulsesimple * self = vio->inst;

 return pa_simple_drain(self->handle, NULL);
}

int     driver_pulsesimple_ctl          (struct roar_vio_calls * vio, int cmd, void * data) {
 struct driver_pulsesimple * self = vio->inst;

 (void)self;

 return -1;
}

#endif

//ll
