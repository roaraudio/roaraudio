//driver_ao.c:

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
#ifdef ROAR_HAVE_LIBAO

int _driver_ao_usage_counter = 0;

void driver_ao_init (void) {
 if ( _driver_ao_usage_counter++ == 0 )
  ao_initialize();
}

void driver_ao_uninit (void) {
 if ( _driver_ao_usage_counter-- == 1 )
  ao_shutdown();
}

int driver_ao_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
//int driver_ao_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
 ao_device        * aodevice;
 ao_sample_format     format;
 int driver;

 ROAR_WARN("The libao driver is obsolete, use another!");

 if ( fh != -1 )
  return -1;

 driver_ao_init();

 if ( device == NULL ) {
  driver = ao_default_driver_id();
 } else {
  if ( (driver = ao_driver_id(device)) == -1 ) {
   ROAR_ERR("Can not open audio device via libao's driver '%s'", device);
   driver_ao_uninit();
   return -1;
  }
 }

 format.bits        = info->bits;
 format.channels    = info->channels;
 format.rate        = info->rate;

 if ( info->bits == 8 ) {
  ROAR_WARN("This is the libao driver in 8 bit mode, It's not known to me if I need to provide data in signed or in unsigned mode. If your musik sounds strange try -oO codec=pcm_s_le or -oO codec=pcm_u_le");
/* NOTE: the following is only true for EsounD driver, not for OSS driver, don't know for the others
  switch (info->codec) {
   case ROAR_CODEC_PCM_U_LE:
   case ROAR_CODEC_PCM_U_BE:
   case ROAR_CODEC_PCM_U_PDP:
     format.byte_format = AO_FMT_NATIVE;
    break;
   default:
     ROAR_ERR("Can not open audio device via libao: codec not supported. You may want to try -oO codec=pcm_u_le or -oO codec=pcm,bits=16");
     driver_ao_uninit();
     return -1;
    break;
  }
*/
 }

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
    format.byte_format = AO_FMT_LITTLE;
   break;
  case ROAR_CODEC_PCM_S_BE:
    format.byte_format = AO_FMT_BIG;
   break;
  default:
   ROAR_ERR("Can not open audio device via libao: codec not supported. You may want to try -oO codec=pcm");
   driver_ao_uninit();
   return -1;
  break;
 }

 aodevice = ao_open_live(driver, &format, NULL /* no options */);

 if ( aodevice == NULL ) {
  ROAR_ERR("Can not open audio device via libao.");
  driver_ao_uninit();
  return -1;
 }

 memset(inst, 0, sizeof(struct roar_vio_calls));
 inst->inst  = (void*) aodevice;
 inst->write = driver_ao_write;

 return 0;
}

int driver_ao_close(DRIVER_USERDATA_T   inst) {

 ao_close((ao_device*)(((struct roar_vio_calls *)inst)->inst));

 driver_ao_uninit();

 return 0;
}

ssize_t driver_ao_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( ao_play((ao_device*)(vio->inst), buf, count) == 0 )
  return -1;
 return count;
}

#endif
//ll
