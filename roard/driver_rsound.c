//driver_rsound.c:

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

#define _DRV_NEED_RSOUND_H
#include "roard.h"

#ifdef ROAR_HAVE_LIBRSOUND

int     driver_rsound_open         (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 rsound_t * self;
 int tmp, tmp2;

 if ( fh != -1 )
  return -1;

 if ( rsd_init(&self) == -1 ) {
  ROAR_DBG("driver_rsound_open(*): rsd_init(&self) failed.");
  return -1;
 }

 if ( device != NULL ) {
  if ( rsd_set_param(self, RSD_HOST, device) == -1 ) {
   ROAR_DBG("driver_rsound_open(*): rsd_set_param(self, RSD_HOST, device) failed.");
   rsd_free(self);
   return -1;
  }
 }

 tmp = info->channels;
 if ( rsd_set_param(self, RSD_CHANNELS, &tmp) == -1 ) {
  ROAR_DBG("driver_rsound_open(*): rsd_set_param(self, RSD_CHANNELS, &tmp) failed.");
  rsd_free(self);
  return -1;
 }

 if ( tmp != info->channels ) {
  rsd_free(self);
  return -1;
 }

 tmp = info->rate;
 if ( rsd_set_param(self, RSD_SAMPLERATE, &tmp) == -1 ) {
  ROAR_DBG("driver_rsound_open(*): rsd_set_param(self, RSD_SAMPLERATE, &tmp) failed.");
  rsd_free(self);
  return -1;
 }

 if ( tmp != info->rate ) {
  rsd_free(self);
  return -1;
 }

 tmp = -1; // unknown by RSound

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
    switch (info->bits) {
     case  8: tmp = RSD_S8;     break;
     case 16: tmp = RSD_S16_LE; break;
    }
   break;
  case ROAR_CODEC_PCM_S_BE:
    switch (info->bits) {
     case  8: tmp = RSD_S8;     break;
     case 16: tmp = RSD_S16_BE; break;
    }
   break;
  case ROAR_CODEC_PCM_S_PDP:
    switch (info->bits) {
     case  8: tmp = RSD_S8;     break;
    }
   break;
  case ROAR_CODEC_PCM_U_LE:
    switch (info->bits) {
     case  8: tmp = RSD_U8;     break;
     case 16: tmp = RSD_U16_LE; break;
    }
   break;
  case ROAR_CODEC_PCM_U_BE:
    switch (info->bits) {
     case  8: tmp = RSD_U8;     break;
     case 16: tmp = RSD_U16_BE; break;
    }
   break;
  case ROAR_CODEC_PCM_U_PDP:
    switch (info->bits) {
     case  8: tmp = RSD_U8;     break;
    }
   break;
 }

 if ( tmp == -1 ) {
  ROAR_DBG("driver_rsound_open(*): Codec/Bits not supported by RSound");
  rsd_free(self);
  return -1;
 }

 tmp2 = tmp;

 if ( rsd_set_param(self, RSD_FORMAT, &tmp) == -1 ) {
  ROAR_DBG("driver_rsound_open(*): rsd_set_param(self, RSD_FORMAT, &tmp={0x%x->0x%x}) failed.", tmp2, tmp);
  rsd_free(self);
  return -1;
 }

 if ( tmp != tmp2 ) {
  rsd_free(self);
  return -1;
 }

 if ( rsd_start(self) == -1 ) {
  ROAR_DBG("driver_rsound_open(*): rsd_start(self) failed.");
  rsd_free(self);
  return -1;
 }

 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->inst  = self;
 inst->close = driver_rsound_close;
 inst->write = driver_rsound_write;

 return 0;
}

int     driver_rsound_close        (struct roar_vio_calls * vio) {
 int r = 0;

 if ( rsd_stop(vio->inst) == -1 )
  r = -1;

 if ( rsd_free(vio->inst) == -1 )
  r = -1;

 return r;
}

ssize_t driver_rsound_write        (struct roar_vio_calls * vio, void *buf, size_t count) {
 size_t ret;

 ret = rsd_write(vio->inst, buf, count);

 if ( ret == 0 )
  return -1;

 return ret;
}

#endif

//ll
