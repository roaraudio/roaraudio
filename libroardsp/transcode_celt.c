//transcode_celt.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroardsp.h"

#ifdef ROAR_HAVE_LIBCELT

#define _16BIT (16/8)
#define _SIZE_LEN 2

#ifdef ROAR_HAVE_CELT_VERSION_0_7_1
typedef celt_int16 celt_int16_t;
#endif

int roar_xcoder_celt_init       (struct roar_xcoder * state) {
 struct roar_xcoder_celt * self = roar_mm_malloc(sizeof(struct roar_xcoder_celt));
 struct roar_audio_info  * info = &(state->info.pcm);

 if ( self == NULL )
  return -1;

 // curruntly only 16 bit mode is supported
 if ( info->bits != 16 ) {
  roar_mm_free(self);
  return -1;
 }

 memset(self, 0, sizeof(struct roar_xcoder_celt));

 state->inst = self;

 self->frame_size           = 256;

 self->bufferlen            = info->channels * 32 + _SIZE_LEN;
 self->iobuffer             = roar_mm_malloc(self->bufferlen);

 if ( self->iobuffer == NULL ) {
  roar_mm_free(self);
  return -1;
 }

#ifdef ROAR_HAVE_CELT_VERSION_0_7_1
 self->mode                 = celt_mode_create(info->rate, self->frame_size, NULL);
#else
 self->mode                 = celt_mode_create(info->rate, info->channels, self->frame_size, NULL);
#endif

 if ( self->mode == NULL ) {
  roar_mm_free(self->iobuffer);
  roar_mm_free(self);
  return -1;
 }

 if (state->encode) {
#ifdef ROAR_HAVE_CELT_VERSION_0_7_1
  self->encoder = celt_encoder_create(self->mode, info->channels, NULL);
#else
  self->encoder = celt_encoder_create(self->mode);
#endif
  if ( self->encoder == NULL ) {
   roar_xcoder_celt_uninit(state);
   return -1;
  }
 } else {
#ifdef ROAR_HAVE_CELT_VERSION_0_7_1
  self->decoder = celt_decoder_create(self->mode, info->channels, NULL);
#else
  self->decoder = celt_decoder_create(self->mode);
#endif
  if ( self->decoder == NULL ) {
   roar_xcoder_celt_uninit(state);
   return -1;
  }
 }

 ROAR_DBG("roar_xcoder_celt_init(*) = 0");

 return 0;
}

int roar_xcoder_celt_uninit     (struct roar_xcoder * state) {
 struct roar_xcoder_celt * self = state->inst;

 if ( self->iobuffer )
  roar_mm_free(self->iobuffer);

 if ( self->encoder )
  celt_encoder_destroy(self->encoder);

 if ( self->decoder )
  celt_decoder_destroy(self->decoder);

 if ( self->mode )
  celt_mode_destroy(self->mode);

 roar_mm_free(self);

 ROAR_DBG("roar_xcoder_celt_uninit(*) = 0");

 return 0;
}

int roar_xcoder_celt_packet_size(struct roar_xcoder * state, int samples) {
 struct roar_xcoder_celt * self = state->inst;
 register int ret = self->frame_size * _16BIT * state->info.pcm.channels;

 ROAR_DBG("roar_xcoder_celt_packet_size(state=%p, samples=%i) = %i", state, samples, ret);

 return ret;
}

int roar_xcoder_celt_encode     (struct roar_xcoder * state, void * buf, size_t len) {
 struct roar_xcoder_celt * self = state->inst;
 uint16_t * lenp = self->iobuffer;
 void     * cp   = self->iobuffer + _SIZE_LEN;
 uint16_t   pkglen;

 ROAR_DBG("roar_xcoder_celt_encode(*): test if we are in encoding mode...");

 if (!state->encode)
  return -1;

 ROAR_DBG("roar_xcoder_celt_encode(*): Encoding...");

 if ( state->stage == ROAR_XCODER_STAGE_INITED ) {
  if ( roar_vio_write(state->backend, ROAR_CELT_MAGIC, ROAR_CELT_MAGIC_LEN) != ROAR_CELT_MAGIC_LEN )
   return -1;
  state->stage = ROAR_XCODER_STAGE_MAGIC;
  ROAR_DBG("roar_xcoder_celt_encode(*): Wrote MAGIC");
 }

 pkglen  = celt_encode(self->encoder, (celt_int16_t *) buf, NULL, cp, self->bufferlen - _SIZE_LEN);
 *lenp   = ROAR_HOST2NET16(pkglen);

 if ( roar_vio_write(state->backend, self->iobuffer, pkglen+2) == -1 )
  return -1;

 return 0;
}

int roar_xcoder_celt_decode     (struct roar_xcoder * state, void * buf, size_t len) {
 struct roar_xcoder_celt * self = state->inst;
 uint16_t * lenp = self->iobuffer;
 void     * cp   = self->iobuffer + _SIZE_LEN;
 uint16_t   pkglen;
 char       magic[ROAR_CELT_MAGIC_LEN];

 ROAR_DBG("roar_xcoder_celt_decode(*): test if we are in decoding mode...");

 if (state->encode)
  return -1;

 if ( state->stage == ROAR_XCODER_STAGE_INITED ) {
  if ( roar_vio_read(state->backend, magic, ROAR_CELT_MAGIC_LEN) != ROAR_CELT_MAGIC_LEN )
   return -1;

  if ( memcmp(magic, ROAR_CELT_MAGIC, ROAR_CELT_MAGIC_LEN) != 0 )
   return -1;

  state->stage = ROAR_XCODER_STAGE_MAGIC;
  ROAR_DBG("roar_xcoder_celt_decode(*): Found valid Magic");
 }

 if ( roar_vio_read(state->backend, lenp, _SIZE_LEN) != _SIZE_LEN )
  return -1;

 pkglen = ROAR_NET2HOST16(*lenp);

 if ( pkglen > (self->bufferlen - _SIZE_LEN) )
  return -1;

 if ( roar_vio_read(state->backend, cp, pkglen) != pkglen )
  return -1;

 if ( celt_decode(self->decoder, cp, pkglen, (celt_int16_t *) buf) < 0 )
  return -1;

 return 0;
}

#endif

//ll
