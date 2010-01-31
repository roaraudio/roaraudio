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
 { "null", "null audio driver", "/dev/null", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
   NULL, NULL, NULL},
#ifdef ROAR_HAVE_ESD
 { "esd", "EsounD audio driver", "localhost, remote.host.dom", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
   NULL, driver_esd_close, driver_esd_open_vio},
#endif
 { "roar", "RoarAudio driver", "localhost, remote.host.dom", DRV_FLAG_NONE,
   ROAR_SUBSYS_WAVEFORM|ROAR_SUBSYS_MIDI|ROAR_SUBSYS_LIGHT|ROAR_SUBSYS_COMPLEX,
   NULL, driver_roar_close, driver_roar_open_vio},
#ifdef ROAR_HAVE_IO_POSIX
 { "raw",  "RAW driver", "/some/file", DRV_FLAG_FHSEC,
   ROAR_SUBSYS_WAVEFORM|ROAR_SUBSYS_MIDI|ROAR_SUBSYS_LIGHT|ROAR_SUBSYS_RAW|ROAR_SUBSYS_COMPLEX,
   NULL, NULL, driver_raw_open_vio},
 { "dstr", "VIO DSTR driver", "/some/file", DRV_FLAG_FHSEC,
   ROAR_SUBSYS_WAVEFORM|ROAR_SUBSYS_MIDI|ROAR_SUBSYS_LIGHT|ROAR_SUBSYS_RAW|ROAR_SUBSYS_COMPLEX,
   NULL, NULL, driver_dstr_open_vio},
#endif
#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
#ifndef ROAR_DEFAULT_OSS_DEV
#define ROAR_DEFAULT_OSS_DEV "no default device"
#endif
 { "oss", "Open Sound System", ROAR_DEFAULT_OSS_DEV, DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
   NULL, NULL, driver_oss_open},
#endif
#ifdef ROAR_HAVE_LIBAO
 { "ao", "libao audio driver", "DRIVER", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
   NULL, driver_ao_close, driver_ao_open_vio},
#endif
#ifdef ROAR_HAVE_LIBSHOUT
 {"shout", "libshout streaming", "http://user:pw@host:port/mount.ogg", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
  NULL, driver_shout_close, driver_shout_open_vio},
#endif
#ifdef ROAR_HAVE_LIBSNDIO
 {"sndio", "OpenBSD sndio", "/dev/audio, /tmp/aucat-<uid>/default", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM|ROAR_SUBSYS_MIDI,
  NULL, NULL, driver_sndio_open},
#endif
#ifdef ROAR_HAVE_LIBWINMM
 {"wmm", "Win32 MM", "???", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM, NULL, NULL, driver_wmm_open_vio},
#endif
#ifndef ROAR_WITHOUT_DCOMP_DMX
 {"dmx", "DMX512 driver", "/dev/dmx", DRV_FLAG_FHSEC, ROAR_SUBSYS_LIGHT,
  NULL, NULL, driver_dmx_open_vio},
#endif
#ifndef ROAR_WITHOUT_DCOMP_PWMLED
 {"pwmled", "PWM LED driver", "/dev/ttyS0", DRV_FLAG_FHSEC, ROAR_SUBSYS_LIGHT,
  NULL, NULL, driver_pwmled_open_vio},
#endif
#ifdef ROAR_HAVE_DRIVER_SYSCLOCK
 {"sysclock", "System Clock Clock Source", "(none)", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
  NULL, NULL, driver_sysclock_open_vio},
#endif
#ifndef ROAR_WITHOUT_DCOMP_CDRIVER
 {"cdriver", "RoarAudio Client driver", "driver#device", DRV_FLAG_NONE, ROAR_SUBSYS_WAVEFORM,
  NULL, NULL, driver_cdriver_open},
#endif
 {NULL, NULL, NULL, DRV_FLAG_NONE, 0, NULL, NULL, NULL} // end of list
                                };

void print_driverlist (void) {
 int i;
 char subsys[7] = "      ";

 printf("  Driver   Flag Subsys - Description (devices)\n");
 printf("------------------------------------------------------\n");

 for (i = 0; g_driver[i].name != NULL; i++) {
  strncpy(subsys, "      ", 6);

  if ( g_driver[i].subsystems & ROAR_SUBSYS_WAVEFORM )
   subsys[0] = 'W';
  if ( g_driver[i].subsystems & ROAR_SUBSYS_MIDI )
   subsys[1] = 'M';
  if ( g_driver[i].subsystems & ROAR_SUBSYS_CB )
   subsys[2] = 'C';
  if ( g_driver[i].subsystems & ROAR_SUBSYS_LIGHT )
   subsys[3] = 'L';
  if ( g_driver[i].subsystems & ROAR_SUBSYS_RAW )
   subsys[4] = 'R';
  if ( g_driver[i].subsystems & ROAR_SUBSYS_COMPLEX )
   subsys[5] = 'X';

  printf("  %-9s %c%c%c %6s - %s (devices: %s)\n", g_driver[i].name,
                g_driver[i].flags & DRV_FLAG_FHSEC                                                         ? 's' : ' ',
                g_driver[i].open     != NULL || (g_driver[i].open == NULL && g_driver[i].vio_init == NULL) ? 'S' : ' ',
                g_driver[i].vio_init != NULL || (g_driver[i].open == NULL && g_driver[i].vio_init == NULL) ? 'V' : ' ',
                subsys,
                g_driver[i].desc, g_driver[i].devices);
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

    if ( (i = g_driver[i].vio_init(*inst, device, info, -1, NULL)) == -1 ) {
     free(*inst);
     return -1;
    }
    return i;
   }

   if ( g_driver[i].open ) {
    ROAR_WARN("driver_open(*): driver(%s) uses old non-vio interface!", driver);
    return g_driver[i].open(inst, device, info);
   }

   return 0;
  }
 }

 return -1;
}

int driver_openvio(struct roar_vio_calls * calls,
                 int * driver_id, char * driver /* NOTE: this is not part of struct roar_driver's def! */,
                 char * device, struct roar_audio_info * info, int fh,
                 struct roar_stream_server * sstream) {
 int i;

 if ( driver == NULL )
  driver = ROAR_DRIVER_DEFAULT;

 ROAR_DBG("driver_openvio(*): searching for driver '%s'...", driver);

 for (i = 0; g_driver[i].name != NULL; i++) {
  if ( strcmp(g_driver[i].name, driver) == 0 ) {
   ROAR_DBG("driver_open(*): found driver: id = %i", i);

   *driver_id = i;

   ROAR_DBG("driver_openvio(*): driver found: %s -> %i", driver, i);

   if ( g_driver[i].vio_init == NULL ) {
    if ( g_driver[i].open == NULL ) { // this is the null driver
     memset(calls, 0, sizeof(struct roar_vio_calls));
     calls->read  = roar_vio_null_rw;
     calls->write = roar_vio_null_rw;
     return 0;
    }

    ROAR_WARN("driver_open(*): driver(%s) uses old non-vio interface!", driver);
    ROAR_ERR("driver_openvio(calls=%p, driver_id={%i}, driver='%s', device='%s', info=%p, fh=%i): not a VIO driver!",
        calls, i, driver, device, info, fh);
    return -1;
   }

   ROAR_DBG("driver_openvio(*): Opening VIO driver %s(%i)...", driver, i);
   return g_driver[i].vio_init(calls, device, info, fh, sstream);
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

int driver_closevio(struct roar_vio_calls * calls, int driver) {
 ROAR_DBG("driver_closevio(calls=%p, driver=%i) = ?", calls, driver);

 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].close )
  return g_driver[driver].close((DRIVER_USERDATA_T)calls);

 if ( calls->close != NULL )
  roar_vio_close(calls);

 return 0;
}

int driver_write(DRIVER_USERDATA_T   inst, int driver, char * buf, int len) {
 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].vio_init != NULL )
  return roar_vio_write((struct roar_vio_calls *) inst, buf, len);

 return 0;
}

int driver_read (DRIVER_USERDATA_T   inst, int driver, char * buf, int len) {
 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].vio_init != NULL )
  return roar_vio_read((struct roar_vio_calls *) inst, buf, len);

 return 0;
}

int driver_set_volume(int stream, struct roar_mixer_settings * mixer) {
 struct roar_stream_server * ss;

 if ( (ss = g_streams[stream]) == NULL )
  return -1;

 if ( !streams_get_flag(stream, ROAR_FLAG_HWMIXER) )
  return 0;

 if ( ss->driver_id == -1 )
  return -1;

 return roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_VOLUME, (void*)mixer);
}


#ifndef ROAR_WITHOUT_DCOMP_CDRIVER
int driver_cdriver_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 char * driver;
 char * delm;
 int ret;

 ROAR_DBG("driver_cdriver_open(inst=%p, device='%s', info=%p, fh=%i) = ?", inst, device, info, fh);

 if (device == NULL) {
  driver = NULL;
  return -1;
 } else {
  driver = strdup(device);

  if ( (delm = strstr(driver, "#")) == NULL ) {
   device = NULL;
  } else {
   *delm  = 0;
   device = strstr(device, "#") + 1;
  }
 }

 ROAR_DBG("driver_cdriver_open(*): CALL roar_cdriver_open(inst=%p, driver='%s', device='%s', info=%p, dir=ROAR_DIR_PLAY)", inst, driver, device, info);
 ret = roar_cdriver_open(inst, driver, device, info, ROAR_DIR_PLAY);
 ROAR_DBG("driver_cdriver_open(*): RET %i", ret);

 if ( driver != NULL )
  free(driver);

 return ret;
}
#endif

//ll
