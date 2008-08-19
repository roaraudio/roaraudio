//codecfilter_wave.c:

#include "roard.h"

int cf_wave_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_wave_inst * self = malloc(sizeof(struct codecfilter_wave_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 if ( !self )
  return -1;

 self->stream               = info;
 self->opened               = 0;

 *inst = (CODECFILTER_USERDATA_T) self;

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));
 return 0;
}

int cf_wave_close(CODECFILTER_USERDATA_T   inst) {
// struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;

 if ( !inst )
  return -1;

 free(inst);
 return 0;
}

int cf_wave_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;
 int r = -1;
 char tbuf[44];
 struct roar_stream * s = ROAR_STREAM(self->stream);

 if ( self->opened ) {
  return stream_vio_s_read(self->stream, buf, len);
 } else {
  if (stream_vio_s_read(self->stream, tbuf, 44) != 44) {
   return -1;
  }

  // TODO: write better code here!

  memcpy(&(s->info.rate    ), tbuf+24, 4);
  memcpy(&(s->info.channels), tbuf+22, 2);
  memcpy(&(s->info.bits    ), tbuf+34, 2);

  self->opened = 1;

  errno = EAGAIN;
  return -1;
 }

 return r;
}

int cf_wave_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;

 return 0;
}

//ll
