//codecfilter_speex.c:

#include "roard.h"
#ifdef ROAR_HAVE_LIBSPEEX

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_speex_inst * self = malloc(sizeof(struct codecfilter_speex_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 *inst = NULL;

 if (!self)
  return -1;

 self->encoder = NULL;
 self->decoder = NULL;

 self->stream  = info;

 self->cd      = NULL;

 self->i_rest  = NULL;
 self->fi_rest = 0;
 self->o_rest  = NULL;
 self->fo_rest = 0;

 speex_bits_init(&(self->bits));

 s->info.codec    = ROAR_CODEC_DEFAULT;
 s->info.bits     = 16; // speex hardcoded
 s->info.channels =  1; // only mono support at the moment

 *inst = (void*) self;

 return 0;
}

int cf_speex_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 if (!self)
  return -1;

 if ( self->encoder )
  speex_encoder_destroy(self->encoder);

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
 int mode;
 uint16_t ui;
 int tmp;
 int still_todo = len / 2 /* 16 bit */;
 int ret = 0;
 int fs2; // = self->frame_size * 2;
 char magic[ROAR_SPEEX_MAGIC_LEN];

 ROAR_DBG("cf_speex_read(inst=%p, buf=%p, len=%i) = ?", inst, buf, len);

 if ( ! self->decoder ) {
  ROAR_DBG("cf_speex_read(*): no decoder, starting one!");

  if ( stream_vio_s_read(self->stream, magic, ROAR_SPEEX_MAGIC_LEN) != ROAR_SPEEX_MAGIC_LEN )
   return 0;

  if ( memcmp(magic, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) != 0 )
   return -1;

  if ( stream_vio_s_read(self->stream, &ui, 2) != 2 )
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

  fs2 = self->frame_size * 2;

  ROAR_DBG("cf_speex_read(*): frame_size=%i (%i bytes)", self->frame_size, fs2);

  if ( !self->cd ) {
   self->cd = malloc(fs2);
   if ( !self->cd )
    return 0;
  }

  if ( !self->i_rest ) {
   self->i_rest = malloc(fs2);
   if ( !self->i_rest )
    return 0;
  }
 }
 fs2 = self->frame_size * 2;

 ROAR_DBG("cf_speex_read(*): Have a working decoder!");

 ROAR_DBG("cf_speex_read(*): frame_size=%i (%i bytes)", self->frame_size, fs2);
 ROAR_DBG("cf_speex_read(*): i_rest is %i bytes after cd", ((void*)self->i_rest - (void*)self->cd));


 if ( self->fi_rest ) {
  if ( self->fi_rest > (still_todo*2) ) {
   ROAR_DBG("cf_speex_read(*): using data from input rest buffer: len=%i (no need to read new data)", self->fi_rest);
   still_todo *= 2; // we will set this to zero one way or another,
                    // so we don't need to care about soring a 'warong' value here.
   memcpy(buf, self->i_rest, still_todo);
   memmove(self->i_rest, self->i_rest + still_todo, self->fi_rest - still_todo);
   self->fi_rest -= still_todo;
   ret += still_todo;
   still_todo = 0;
  } else {
   ROAR_DBG("cf_speex_read(*): using data from input rest buffer: len=%i", self->fi_rest);
   memcpy(buf, self->i_rest, self->fi_rest);
   buf += self->fi_rest;
   still_todo -= self->fi_rest/2;
   ret += self->fi_rest;
   self->fi_rest = 0;
  }
 }

 while (still_todo) {
  ROAR_DBG("cf_speex_read(*): we sill need %i frames", still_todo);
  if ( stream_vio_s_read(self->stream, &ui, 2) != 2 )
   return -1;

  ui = ntohs(ui);

  if ( ui > ROAR_SPEEX_MAX_CC )
   return 0;

  if ( stream_vio_s_read(self->stream, self->cc, ui) != ui )
   break;

  speex_bits_read_from(&(self->bits), self->cc, ui);

  speex_decode_int(self->decoder, &(self->bits), self->cd);

  if ( self->frame_size > still_todo ) {
   memcpy(buf, self->cd, still_todo*2);
   ret += still_todo*2;
   self->fi_rest = (self->frame_size - still_todo)*2;
   ROAR_DBG("cf_speex_read(*): self->fi_rest=%i, off=%i", self->fi_rest, still_todo*2);
   memcpy(self->i_rest, (self->cd)+(still_todo*2), self->fi_rest);
   still_todo = 0;
  } else {
   memcpy(buf, self->cd, fs2);
   buf        += fs2;
   ret        += fs2;
   still_todo -= self->frame_size;
  }
 }

 if ( still_todo ) {
  ROAR_DBG("cf_speex_read(*): could not read all reqquested data, returning %i byte less", still_todo*2);
 }

 ROAR_DBG("cf_speex_read(*) = %i", ret);

 return ret;
}

int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;
 uint16_t mode = ROAR_SPEEX_MODE_UWB;
 int tmp;
 int fs2;
 int ret = 0;
 int need_extra;

 ROAR_DBG("cf_speex_write(inst=%p, buf=%p, len=%i) = ?", inst, buf, len);

 if ( ! self->encoder ) {
  if ( stream_vio_s_write(self->stream, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) != ROAR_SPEEX_MAGIC_LEN )
   return -1;

  if ( mode == ROAR_SPEEX_MODE_NB ) {
   self->encoder = speex_encoder_init(&speex_nb_mode);
  } else if ( mode == ROAR_SPEEX_MODE_WB ) {
   self->encoder = speex_encoder_init(&speex_wb_mode);
  } else if ( mode == ROAR_SPEEX_MODE_UWB ) {
   self->encoder = speex_encoder_init(&speex_uwb_mode);
  }

  mode = ROAR_HOST2NET16(mode);

  if ( stream_vio_s_write(self->stream, &mode, 2) != 2 )
   return -1;

  tmp = 8;
  speex_encoder_ctl(self->encoder, SPEEX_SET_QUALITY,    &tmp);
  speex_encoder_ctl(self->encoder, SPEEX_GET_FRAME_SIZE, &(self->frame_size));

  fs2 = self->frame_size * 2;

  if ( !self->cd ) {
   self->cd = malloc(fs2);
   if ( !self->cd )
    return 0;
  }

  if ( !self->o_rest ) {
   self->o_rest = malloc(fs2);
   if ( !self->o_rest )
    return 0;
  }
 }

 fs2 = self->frame_size * 2;

 if ( self->fo_rest ) { // ignore the rest for the moment
  if ( (self->fo_rest + len) > fs2 ) {
   need_extra = fs2 - self->fo_rest;
   memcpy(self->o_rest + self->fo_rest, buf, need_extra);

   speex_bits_reset(&(self->bits));

   speex_encode_int(self->encoder, (spx_int16_t *) self->o_rest, &(self->bits));

   tmp = mode = speex_bits_write(&(self->bits), self->cd, fs2);

   mode = ROAR_HOST2NET16(mode);
   stream_vio_s_write(self->stream, &mode, 2);
   if ( stream_vio_s_write(self->stream, self->cd, tmp) != tmp )
    return -1;

   buf += need_extra;
   ret += need_extra;
   len -= need_extra;
   self->fo_rest = 0;
  } else { // just add the data to o_rest
   memcpy(self->o_rest + self->fo_rest, buf, len);
   self->fo_rest += len;
   return len;
  }
 }

 // TODO: do we realy need such a loop?
 while (len > fs2) {
//  ROAR_WARN("cf_speex_write(*): Discarding a full block of data as non-o_rest encoding is not supported!");
//  ROAR_WARN("cf_speex_write(*): Block info: len=%i, fs2=%i", len, fs2);

  speex_bits_reset(&(self->bits));

  speex_encode_int(self->encoder, (spx_int16_t *) buf, &(self->bits));

  tmp = mode = speex_bits_write(&(self->bits), self->cd, fs2);

  mode = ROAR_HOST2NET16(mode);

  stream_vio_s_write(self->stream, &mode, 2);

  if ( stream_vio_s_write(self->stream, self->cd, tmp) != tmp )
   return -1;

  len -= fs2;
  buf += fs2;
  ret += fs2;
 }

 if ( len ) { // we still have some data, add this to o_rest
  memcpy(self->o_rest, buf, len);
  self->fo_rest = len;
  ret += len;
 }

 return ret;
}

#endif

//ll
