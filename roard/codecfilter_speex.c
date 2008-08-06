//codecfilter_speex.c:

#include "roard.h"
#ifdef ROAR_HAVE_LIBSPEEX

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_speex_inst * self = malloc(sizeof(struct codecfilter_speex_inst));

 *inst = NULL;

 if (!self)
  return -1;

 self->encoder = NULL;
 self->decoder = NULL;

 self->stream  = info;

 speex_bits_init(&(self->bits));

 *inst = (void*) self;

 return 0;
}

int cf_speex_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 if (!self)
  return -1;

 if ( self->encoder )
  speex_encoder_destroy(self->decoder);

 self->encoder = NULL;

 if ( self->decoder )
  speex_decoder_destroy(self->decoder);

 self->decoder = NULL;

 speex_bits_destroy(&(self->bits));

 free((void*)self);

 return 0;
}

int cf_speex_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;
  struct roar_stream * s = (struct roar_stream *) self->stream;
 int mode;
 uint16_t ui;

 if ( ! self->decoder ) {
  if ( read(s->fh, &ui, 2) != 2 )
   return 0;

  mode = ntohs(ui);

  if ( mode == ROAR_SPEEX_MODE_NB ) {
   self->decoder = speex_decoder_init(&speex_nb_mode);
  } else if ( mode == ROAR_SPEEX_MODE_WB ) {
   self->decoder = speex_decoder_init(&speex_wb_mode);
  } else if ( mode == ROAR_SPEEX_MODE_UWB ) {
   self->decoder = speex_decoder_init(&speex_uwb_mode);
  } else {
   return 0;
  }
 }

 return -1;
}

int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 return -1;
}

#endif

//ll
