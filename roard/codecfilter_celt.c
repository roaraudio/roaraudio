//codecfilter_celt.c:

#include "roard.h"

int cf_celt_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_celt_inst * self = malloc(sizeof(struct codecfilter_celt_inst));
 struct roar_stream * s = (struct roar_stream *) info;

 if ( !self )
  return -1;

/*
 CELTMode * mode;
 CELTEncoder * encoder;
 CELTDecoder * decoder;
 int frame_size;
 int lookahead;
 int out_size;
 char * ibuf;
 char * obuf;
 char * rest;
 int s_buf;
 int f_rest; /-* how much is in rest? *-/
*/

 self->stream               = info;
 self->frame_size           = 256;
 self->lookahead            = self->frame_size;
 self->encoder              = NULL;
 self->decoder              = NULL;
 self->s_buf                = s->info.channels * self->frame_size;
 self->ibuf                 = malloc(self->s_buf);
 self->obuf                 = malloc(self->s_buf);
 self->rest                 = malloc(self->s_buf);
 self->f_rest               = 0;

 if ( !(self->ibuf && self->obuf && self->rest) ) {
  if ( self->ibuf )
   free(self->ibuf);

  if ( self->obuf )
   free(self->obuf);

  if ( self->rest )
   free(self->rest);

  free(self);
  return -1;
 }
 
 self->mode                 = celt_mode_create(s->info.rate, s->info.channels, self->frame_size, self->lookahead, NULL);

 if ( !self->mode ) {
  free(self);
  return -1;
 }

 if ( s->dir == ROAR_DIR_PLAY ) {
   self->decoder = celt_decoder_create(self->mode);
 } else if ( s->dir == ROAR_DIR_MONITOR ) {
   self->encoder = celt_encoder_create(self->mode);
 } else {
  celt_mode_destroy(self->mode);
  free(self);
  return -1;
 }

 *inst = (CODECFILTER_USERDATA_T) self;

 s->info.codec = ROAR_CODEC_DEFAULT;
 s->info.bits  = 16; // CELT hardcoded

 return 0;
}

int cf_celt_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 if ( !inst )
  return -1;

 if ( self->encoder )
 celt_encoder_destroy(self->encoder);

 if ( self->decoder )
 celt_decoder_destroy(self->decoder);

 if ( self->mode )
  celt_mode_destroy(self->mode);

 if ( self->ibuf )
  free(self->ibuf);

 if ( self->obuf )
  free(self->obuf);

 if ( self->rest )
  free(self->rest);

 free(inst);
 return 0;
}

int cf_celt_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 return 0;
}

int cf_celt_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 return 0;
}

//ll
