//driver_esd.h:

#ifndef _DRIVER_ROAR_H_
#define _DRIVER_ROAR_H_

int driver_roar_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
int driver_roar_close(DRIVER_USERDATA_T   inst);
int driver_roar_pause(DRIVER_USERDATA_T   inst, int newstate);
int driver_roar_write(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_roar_read(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_roar_flush(DRIVER_USERDATA_T   inst);

#endif

//ll
