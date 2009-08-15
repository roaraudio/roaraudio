//driver_raw.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

#ifdef ROAR_HAVE_DRIVER_SYSCLOCK
int driver_sysclock_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 struct driver_sysclock * self = malloc(sizeof(struct driver_sysclock));

 if ( self == NULL )
  return -1;

 if ( device != NULL ) {
  return -1;
 }

 memset(self, 0, sizeof(struct driver_sysclock));
 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->inst  = self;
 inst->close = driver_sysclock_close;
 inst->write = driver_sysclock_write;

 self->bps   = (info->bits / 8) * info->channels * info->rate;

 if (!self->bps) {
  free(self);
  return -1;
 }

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
  case ROAR_CODEC_PCM_S_BE:
  case ROAR_CODEC_PCM_S_PDP:
  case ROAR_CODEC_PCM_U_LE:
  case ROAR_CODEC_PCM_U_BE:
  case ROAR_CODEC_PCM_U_PDP:
   break;
  case ROAR_CODEC_ALAW:
  case ROAR_CODEC_MULAW:
    // one byte per sample
    self->bps *= 8;
    self->bps /= info->bits;
   break;
  default:
    free(self);
    return -1;
   break;
 }

 gettimeofday(&(self->lasttime), NULL);

 return 0;
}

int     driver_sysclock_close   (struct roar_vio_calls * vio) {
 struct driver_sysclock * self = vio->inst;

 if ( self == NULL )
  return -1;

 free(self);

 return 0;
}

ssize_t driver_sysclock_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_sysclock * self = vio->inst;
 struct timeval now;
 unsigned long long diff = (1000000 * count / self->bps);
 unsigned long long ago;

 gettimeofday(&now, NULL);

 ago  = now.tv_usec - self->lasttime.tv_usec;
 ago += 1000000*(now.tv_sec - self->lasttime.tv_sec);

 memcpy(&(self->lasttime), &now, sizeof(now));

 ROAR_DBG("driver_sysclock_write(*): count=%u, bps=%u, diff=%llu, ago=%llu", count, self->bps, diff, ago);

 if ( diff <= ago )
  return count;

 diff -= ago;

 usleep(diff);

 return count;
}

#endif

//ll
