//driver_raw.c:

#include "roard.h"

int driver_raw_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
 int * di = malloc(sizeof(int));

 if ( di == NULL )
  return -1;

 *inst = (DRIVER_USERDATA_T)di;

 if ( device == NULL ) {
  free(di);
  *inst = NULL;
  return -1;
 }

 *di = open(device, O_CREAT|O_TRUNC|O_WRONLY, 0644);

 if ( *di == -1 ) {
  free(di);
  *inst = NULL;
  return -1;
 }

 return 0;
}

int driver_raw_close(DRIVER_USERDATA_T   inst) {
 int fh = *(int*)inst;

 free((void*)inst);

 return close(fh);
}

int driver_raw_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

int driver_raw_write(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return write(*(int*)inst, buf, len);
}

int driver_raw_read(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return read(*(int*)inst, buf, len);
}

int driver_raw_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

//ll
