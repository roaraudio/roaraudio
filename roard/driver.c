

#include "roard.h"

struct roar_driver g_driver[] = {
 { "null", "null audio driver", "/dev/null", NULL, NULL, NULL, NULL, NULL, NULL},
#ifdef ROAR_HAVE_ESD
 { "esd", "EsounD audio driver", "localhost, remote.host.dom", driver_esd_open, driver_esd_close, driver_esd_pause, driver_esd_write, driver_esd_read, driver_esd_flush},
#endif
 { "roar", "RoarAudio driver", "localhost, remote.host.dom", driver_roar_open, driver_roar_close, driver_roar_pause, driver_roar_write, driver_roar_read, driver_roar_flush},
#ifdef ROAR_HAVE_LIBAO
 { "ao", "libao audio driver", "DRIVER", driver_ao_open, driver_ao_close, driver_ao_pause, driver_ao_write, driver_ao_read, driver_ao_flush},
#endif
 {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL} // end of list
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

   if ( g_driver[i].open )
    return g_driver[i].open(inst, device, info);
   return 0;
  }
 }

 return -1;
}

int driver_close(DRIVER_USERDATA_T   inst, int driver) {
 ROAR_DBG("driver_close(inst=%p, driver=%i) = ?", inst, driver);

 if ( driver == -1 )
  return -1;

 if ( g_driver[driver].close )
  return g_driver[driver].close(inst);

 return 0;
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

 if ( g_driver[driver].write )
  return g_driver[driver].write(inst, buf, len);

 return 0;
}

int driver_read (DRIVER_USERDATA_T   inst, int driver, char * buf, int len) {
 if ( driver == -1 )
  return -1;

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
