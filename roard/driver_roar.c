//driver_esd.c:

#include "roard.h"

int driver_roar_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
/*
 esd_format_t format = ESD_STREAM | ESD_PLAY;
 char name[80] = "roard";

 format |= info->bits     == 16 ? ESD_BITS16 : ESD_BITS8;
 format |= info->channels ==  2 ? ESD_STEREO : ESD_MONO;

 *(int*)inst = esd_play_stream_fallback(format, info->rate, device, name);

 if ( *(int*)inst == -1 )
  return -1;

 return 0;
*/

 *(int*)inst =  roar_simple_play(info->rate, info->channels, info->bits, info->codec, device, "roard");

 if ( *(int*)inst == -1 )
  return -1;

 return 0;
}

int driver_roar_close(DRIVER_USERDATA_T   inst) {
 return roar_simple_close((int)inst);
}

int driver_roar_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

int driver_roar_write(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return write((int)inst, buf, len);
}

int driver_roar_read(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return read((int)inst, buf, len);
}

int driver_roar_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

//ll
