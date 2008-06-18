//driver_raw.h:

#ifndef _DRIVER_RAW_H_
#define _DRIVER_RAW_H_

int driver_raw_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
int driver_raw_close(DRIVER_USERDATA_T   inst);
int driver_raw_pause(DRIVER_USERDATA_T   inst, int newstate);
int driver_raw_write(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_raw_read(DRIVER_USERDATA_T   inst, char * buf, int len);
int driver_raw_flush(DRIVER_USERDATA_T   inst);

#endif

//ll
