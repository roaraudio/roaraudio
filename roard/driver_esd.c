//driver_esd.c:

#include "roard.h"

/*
 We could use inst as our fh directly. But esd works with unsigned at 8 bits and
 signed at 16 bits per sample. (why???) so we need to convert because we get signed at both,
 8 and 16 bits per sample. so we use inst as an array of two ints: 0: fh, 1: are we in 8 bit mode?
*/

int driver_esd_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
 esd_format_t format = ESD_STREAM | ESD_PLAY;
 char name[80] = "roard";
 int * di = malloc(sizeof(int)*2);

 if ( di == NULL )
  return -1;

 *inst = (DRIVER_USERDATA_T)di;

 format |= info->bits     == 16 ? ESD_BITS16 : ESD_BITS8;
 format |= info->channels ==  2 ? ESD_STEREO : ESD_MONO;

 di[1] = info->bits == 8;

 di[0] = esd_play_stream_fallback(format, info->rate, device, name);

 if ( di[0] == -1 ) {
  free(di);
  *inst = NULL;
  return -1;
 }

 return 0;
}

int driver_esd_close(DRIVER_USERDATA_T   inst) {
 int fh = *(int*)inst;

 free((void*)inst);

 return esd_close(fh);
}

int driver_esd_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

int driver_esd_write(DRIVER_USERDATA_T   inst, char * buf, int len) {
 int * di = (int*)inst;

 if ( di[1] )
  roar_conv_codec_s2u8(buf, buf, len);

 return write(di[0], buf, len);
}

int driver_esd_read(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return read(*(int*)inst, buf, len);
}

int driver_esd_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

//ll
