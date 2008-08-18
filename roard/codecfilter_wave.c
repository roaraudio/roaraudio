//codecfilter_wave.c:

#include "roard.h"

int cf_wave_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_wave_inst * self = malloc(sizeof(struct codecfilter_wave_inst));
 struct roar_stream * s = (struct roar_stream *) info;

 if ( !self )
  return -1;

 self->stream               = info;
 self->opened               = 0;

/*
 if ( s->dir == ROAR_DIR_PLAY ) {
   self->decoder = celt_decoder_create(self->mode);
 } else if ( s->dir == ROAR_DIR_MONITOR ) {
   self->encoder = celt_encoder_create(self->mode);
 } else {
  celt_mode_destroy(self->mode);
  free(self);
  return -1;
 }
*/

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
 int fh = ((struct roar_stream *)self->stream)->fh;
 int r = -1;
 char tbuf[44];
 struct roar_stream * s = ROAR_STREAM(self->stream);

 if ( self->opened ) {
  return read(fh, buf, len);
 } else {
  if (read(fh, tbuf, 44) != 44) {
   close(fh);
   return -1;
  }

  memcpy(&(s->info.rate    ), tbuf+24, 4);
  memcpy(&(s->info.channels), tbuf+22, 2);
  memcpy(&(s->info.bits    ), tbuf+34, 2);

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
