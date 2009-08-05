//codecfilter_speex.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"
#ifdef ROAR_HAVE_LIBSPEEX

/*
 Format:
 (all numbers are in network byte order)

 MAGIC of site MAGIC_LEN
 MODE NUMMBER of size 2 Byte
 { // frames
   FRAME LENGTH of size 2 Byte
   FRAME DATA of size FRAME LENGTH Byte
 }
*/

#define _FS (_16BIT * (self->stereo ? 2 : 1))

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_speex_inst * self = malloc(sizeof(struct codecfilter_speex_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 *inst = NULL;

 if (!self)
  return -1;

 s->info.codec    = ROAR_CODEC_DEFAULT;
 s->info.bits     = 16; // speex hardcoded

 switch (s->info.channels) {
  case 1: self->stereo = 0; break;
  case 2: self->stereo = 1; break;
  default:
    free(self);
    return -1;
 }

 // do as much to preper the startup of stereo encoder as possible
 if ( self->stereo ) {
  self->stereo_callback.callback_id = SPEEX_INBAND_STEREO;
  self->stereo_callback.func        = speex_std_stereo_request_handler;
  self->stereo_callback.data       = &(self->stereo_state);
 }

 self->encoder = NULL;
 self->decoder = NULL;

 self->stream  = info;

 self->cd      = NULL;

 self->i_rest  = NULL;
 self->fi_rest = 0;
 self->o_rest  = NULL;
 self->fo_rest = 0;

 speex_bits_init(&(self->bits));

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
 int still_todo = len / _FS;
 int ret = 0;
 int fs2; // = self->frame_size * _16BIT * channels;
 char magic[ROAR_SPEEX_MAGIC_LEN];
 SpeexStereoState stereo = SPEEX_STEREO_STATE_INIT;

 ROAR_DBG("cf_speex_read(inst=%p, buf=%p, len=%i) = ?", inst, buf, len);

 if ( ! self->decoder ) {
  ROAR_DBG("cf_speex_read(*): no decoder, starting one!");

  if ( stream_vio_s_read(self->stream, magic, ROAR_SPEEX_MAGIC_LEN) != ROAR_SPEEX_MAGIC_LEN )
   return 0;

  if ( memcmp(magic, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) != 0 )
   return -1;

  if ( stream_vio_s_read(self->stream, &ui, 2) != 2 )
   return 0;

  mode = ROAR_NET2HOST16(ui);

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

  if ( self->stereo ) {
   memcpy(&(self->stereo_state), &stereo, sizeof(self->stereo_state));
   speex_decoder_ctl(self->decoder, SPEEX_SET_HANDLER, &(self->stereo_callback));
  }


  speex_decoder_ctl(self->decoder, SPEEX_GET_FRAME_SIZE, &(self->frame_size));

  fs2 = self->frame_size * _FS;

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
 fs2 = self->frame_size * _FS;

 ROAR_DBG("cf_speex_read(*): Have a working decoder!");

 ROAR_DBG("cf_speex_read(*): frame_size=%i (%i bytes)", self->frame_size, fs2);
 ROAR_DBG("cf_speex_read(*): i_rest is %i bytes after cd", ((void*)self->i_rest - (void*)self->cd));


 if ( self->fi_rest ) {
  if ( self->fi_rest > (still_todo*_FS) ) {
   ROAR_DBG("cf_speex_read(*): using data from input rest buffer: len=%i (no need to read new data)", self->fi_rest);
   still_todo *= _FS; // we will set this to zero one way or another,
                         // so we don't need to care about soring a 'wrong' value here.
   memcpy(buf, self->i_rest, still_todo);
   memmove(self->i_rest, self->i_rest + still_todo, self->fi_rest - still_todo);
   self->fi_rest -= still_todo;
   ret += still_todo;
   still_todo = 0;
  } else {
   ROAR_DBG("cf_speex_read(*): using data from input rest buffer: len=%i", self->fi_rest);
   memcpy(buf, self->i_rest, self->fi_rest);
   buf += self->fi_rest;
   still_todo -= self->fi_rest/_FS;
   ret += self->fi_rest;
   self->fi_rest = 0;
  }
 }

 while (still_todo) {
  ROAR_DBG("cf_speex_read(*): we sill need %i frames", still_todo);
  if ( stream_vio_s_read(self->stream, &ui, 2) != 2 )
   return -1;

  ui = ROAR_NET2HOST16(ui);

  if ( ui > ROAR_SPEEX_MAX_CC )
   return 0;

  if ( stream_vio_s_read(self->stream, self->cc, ui) != ui )
   break;

  speex_bits_read_from(&(self->bits), self->cc, ui);

  speex_decode_int(self->decoder, &(self->bits), self->cd);

  if ( self->stereo) {
   speex_decode_stereo_int(self->cd, self->frame_size, &(self->stereo_state));
  }

  if ( self->frame_size > still_todo ) {
   memcpy(buf, self->cd, still_todo*_FS);
   ret += still_todo*_FS;
   self->fi_rest = (self->frame_size - still_todo)*_FS;
   ROAR_DBG("cf_speex_read(*): self->fi_rest=%i, off=%i", self->fi_rest, still_todo*_FS);
   memcpy(self->i_rest, (self->cd)+(still_todo*_FS), self->fi_rest);
   still_todo = 0;
  } else {
   memcpy(buf, self->cd, fs2);
   buf        += fs2;
   ret        += fs2;
   still_todo -= self->frame_size;
  }
 }

 if ( still_todo ) {
  ROAR_DBG("cf_speex_read(*): could not read all reqquested data, returning %i byte less", still_todo*_FS);
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

/*
 TODO: Befor this realy works there must be a working way to set the number of channels and bits
       for monetoring clients. Else this will produce some thing stange as a 'mono' file that realy
       contains stereo.
*/

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

  fs2 = self->frame_size * _FS;

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

 fs2 = self->frame_size * _FS;

 if ( self->fo_rest ) { // ignore the rest for the moment
  if ( (self->fo_rest + len) > fs2 ) {
   need_extra = fs2 - self->fo_rest;
   memcpy(self->o_rest + self->fo_rest, buf, need_extra);

   speex_bits_reset(&(self->bits));

   if ( self->stereo )
    speex_encode_stereo_int((spx_int16_t *) self->o_rest, self->frame_size, &(self->bits));

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

  if ( self->stereo )
   speex_encode_stereo_int((spx_int16_t *) buf, self->frame_size, &(self->bits));

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
