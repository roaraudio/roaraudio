//driver_esd.c:

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
