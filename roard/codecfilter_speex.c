//codecfilter_speex.c:

#include "roard.h"
#ifdef ROAR_HAVE_LIBSPEEX

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_speex_inst * self = malloc(sizeof(struct codecfilter_speex_inst));
 struct roar_stream * s = (struct roar_stream *) info;

 *inst = NULL;

 if (!self)
  return -1;

 self->encoder = NULL;
 self->decoder = NULL;

 self->stream  = info;

 self->cd      = NULL;

 self->i_rest  = NULL;
 self->fi_rest = 0;

 speex_bits_init(&(self->bits));

 s->info.codec    = ROAR_CODEC_DEFAULT;
 s->info.bits     = 16; // speex hardcoded
 s->info.channels = 1; // only mono support at the moment

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

 if ( self->cd )
  free(self->cd);

 if ( self->i_rest )
  free(self->i_rest);

 free((void*)self);

 return 0;
}

int cf_speex_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;
 struct roar_stream * s = (struct roar_stream *) self->stream;
 int mode;
 uint16_t ui;
 int tmp;
 int still_todo = len / 2 /* 16 bit */;
 int ret = 0;

 if ( ! self->decoder ) {
  ROAR_DBG("cf_speex_read(*): no decoder, starting one!");
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

  tmp=1;
  speex_decoder_ctl(self->decoder, SPEEX_SET_ENH, &tmp);

  speex_decoder_ctl(self->decoder, SPEEX_GET_FRAME_SIZE, &(self->frame_size));



  ROAR_WARN("cf_speex_read(*): frame_size=%i (%i bytes)", self->frame_size, self->frame_size*2);

  if ( !self->cd ) {
   self->cd = malloc((self->frame_size)*2);
   if ( !self->cd )
    return 0;
  }

  if ( !self->i_rest ) {
   self->i_rest = malloc((self->frame_size)*2);
   if ( !self->i_rest )
    return 0;
  }
 }

 ROAR_WARN("cf_speex_read(*): Have a working decoder!");

 ROAR_WARN("cf_speex_read(*): frame_size=%i (%i bytes)", self->frame_size, self->frame_size*2);
 ROAR_WARN("cf_speex_read(*): i_rest is %i bytes after cd", self->i_rest - self->cd);


 if ( self->fi_rest ) {
  if ( self->fi_rest > still_todo ) {
   ROAR_WARN("cf_speex_read(*): discarding input rest data: buffer too long!");
   self->fi_rest = 0;
  } else {
   ROAR_WARN("cf_speex_read(*): using data from input rest buffer: len=%i", self->fi_rest);
   memcpy(buf, self->i_rest, self->fi_rest);
   buf += self->fi_rest;
   self->fi_rest = 0;
  }
 }

 while (still_todo) {
  ROAR_WARN("cf_speex_read(*): we sill need %i frames", still_todo);
  if ( read(s->fh, &ui, 2) != 2 )
   return -1;

  ui = ntohs(ui);

  if ( ui > ROAR_SPEEX_MAX_CC )
   return 0;

  if ( read(s->fh, self->cc, ui) != ui )
   break;

  speex_bits_read_from(&(self->bits), self->cc, ui);

  speex_decode_int(self->decoder, &(self->bits), self->cd);

  if ( self->frame_size > still_todo ) {
   memcpy(buf, self->cd, still_todo*2);
   ret += still_todo*2;
   self->fi_rest = (self->frame_size - still_todo)*2;
   ROAR_WARN("cf_speex_read(*): self->fi_rest=%i, off=%i", self->fi_rest, still_todo*2);
   memcpy(self->i_rest, (self->cd)+(still_todo*2), self->fi_rest);
   still_todo = 0;
  } else {
   memcpy(buf, self->cd, self->frame_size*2);
   buf        += self->frame_size*2;
   ret        += self->frame_size*2;
   still_todo -= self->frame_size;
  }
 }

 return ret;
}

int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 return -1;
}

#endif

//ll
