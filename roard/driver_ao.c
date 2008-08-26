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

int driver_ao_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
 ao_device        * aodevice;
 ao_sample_format     format;
 int driver;

 ao_initialize();

 if ( device == NULL ) {
  driver = ao_default_driver_id();
 } else {
  if ( (driver = ao_driver_id(device)) == -1 ) {
   ROAR_ERR("Can not open audio device via libao's driver '%s'", device);
   return -1;
  }
 }

 format.bits        = info->bits;
 format.channels    = info->channels;
 format.rate        = info->rate;
 format.byte_format = AO_FMT_NATIVE;

 aodevice = ao_open_live(driver, &format, NULL /* no options */);

 if ( aodevice == NULL ) {
  ROAR_ERR("Can not open audio device via libao.");
  return -1;
 }

 *((ao_device**)inst) = aodevice;

 return 0;
}

int driver_ao_close(DRIVER_USERDATA_T   inst) {

 ao_close((ao_device*)inst);

 ao_shutdown();

 return -1;
}

int driver_ao_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

int driver_ao_write(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return ao_play((ao_device*)inst, buf, len);
}

int driver_ao_read(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return -1;
}

int driver_ao_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

#endif
//ll
