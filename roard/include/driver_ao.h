//driver_ao.h:

#ifndef _DRIVER_AO_H_
#define _DRIVER_AO_H_

int driver_ao_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
int driver_ao_close(DRIVER_USERDATA_T   inst);
int driver_ao_pause(DRIVER_USERDATA_T   inst, int newstate);
int driver_ao_write(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_ao_read(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_ao_flush(DRIVER_USERDATA_T   inst);

#endif

//ll
