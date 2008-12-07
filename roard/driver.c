//driver.c:

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

struct roar_driver g_driver[] = {
 { "null", "null audio driver", "/dev/null", NULL, NULL, NULL, NULL, NULL, NULL, NULL},
#ifdef ROAR_HAVE_ESD
 { "esd", "EsounD audio driver", "localhost, remote.host.dom", NULL, driver_esd_close, driver_esd_pause, NULL, NULL, driver_esd_flush, driver_esd_open_vio},
#endif
 { "roar", "RoarAudio driver", "localhost, remote.host.dom", driver_roar_open, driver_roar_close, driver_roar_pause, driver_roar_write, driver_roar_read, driver_roar_flush, NULL},
 { "raw",  "RAW PCM driver", "/some/file", driver_raw_open, driver_raw_close, driver_roar_pause, driver_raw_write, driver_raw_read, driver_raw_flush, NULL},
#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
#ifndef ROAR_DEFAULT_OSS_DEV
#define ROAR_DEFAULT_OSS_DEV "no default device"
#endif
 { "oss", "Open Sound System", ROAR_DEFAULT_OSS_DEV, NULL, driver_oss_close, NULL, NULL, NULL, NULL, driver_oss_open},
#endif
#ifdef ROAR_HAVE_LIBAO
 { "ao", "libao audio driver", "DRIVER", driver_ao_open, driver_ao_close, driver_ao_pause, driver_ao_write, driver_ao_read, driver_ao_flush, NULL},
#endif
 {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL} // end of list
                                };

void print_driverlist (void) {
 int i;

 for (i = 0; g_driver[i].name != NULL; i++) {
  printf("  %-8s - %s (devices: %s)\n", g_driver[i].name, g_driver[i].desc, g_driver[i].devices);
 }
}

int driver_open (DRIVER_USERDATA_T * inst, int * driver_id, char * driver, char * device, struct roar_audio_info * info) {
 int i;

 if ( driver == NULL )
  driver = ROAR_DRIVER_DEFAULT;

 for (i = 0; g_driver[i].name != NULL; i++) {
  if ( strcmp(g_driver[i].name, driver) == 0 ) {
   ROAR_DBG("driver_open(*): found driver: id = %i", i);

   *driver_id = i;

   if ( g_driver[i].vio_init != NULL ) {
    if ( (*inst = malloc(sizeof(struct roar_vio_calls))) == NULL )
     return -1;

    memset(*inst, 0, sizeof(struct roar_vio_calls));

    if ( (i = g_driver[i].vio_init(*inst, device, info, -1)) == -1 ) {
     free(*inst);
     return -1;
    }
    return i;
   }

   ROAR_WARN("driver_open(*): driver uses old non-vio interface!");

   if ( g_driver[i].open )
    return g_driver[i].open(inst, device, info);
   return 0;
  }
 }

 return -1;
}

int driver_close(DRIVER_USERDATA_T   inst, int driver) {
 int ret = 0;
 ROAR_DBG("driver_close(inst=%p, driver=%i) = ?", inst, driver);

 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].close )
  ret = g_driver[driver].close(inst);

 if ( g_driver[driver].vio_init != NULL )
  free(inst);

 return ret;
}

int driver_pause(DRIVER_USERDATA_T   inst, int driver, int newstate) {
 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].pause )
  return g_driver[driver].pause(inst, newstate);

 return 0;
}

int driver_write(DRIVER_USERDATA_T   inst, int driver, char * buf, int len) {
 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].vio_init != NULL )
  return roar_vio_write((struct roar_vio_calls *) inst, buf, len);

 if ( g_driver[driver].write )
  return g_driver[driver].write(inst, buf, len);

 return 0;
}

int driver_read (DRIVER_USERDATA_T   inst, int driver, char * buf, int len) {
 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].vio_init != NULL )
  return roar_vio_read((struct roar_vio_calls *) inst, buf, len);

 if ( g_driver[driver].read )
  return g_driver[driver].read(inst, buf, len);

 return 0;
}

int driver_flush(DRIVER_USERDATA_T   inst, int driver) {
 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].flush )
  return g_driver[driver].flush(inst);

 return 0;
}

//ll
