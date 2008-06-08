//driver_esd.h:

#ifndef _DRIVER_ESD_H_
#define _DRIVER_ESD_H_

int driver_esd_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
int driver_esd_close(DRIVER_USERDATA_T   inst);
int driver_esd_pause(DRIVER_USERDATA_T   inst, int newstate);
int driver_esd_write(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_esd_read(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_esd_flush(DRIVER_USERDATA_T   inst);

#endif

//ll
