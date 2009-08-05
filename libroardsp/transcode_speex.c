//transcode_speex.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroardsp.h"

#ifdef ROAR_HAVE_LIBSPEEX

#define _16BIT (16/8)
#define _SIZE_LEN 2

int roar_xcoder_speex_init       (struct roar_xcoder * state) {
 struct roar_xcoder_speex * self = malloc(sizeof(struct roar_xcoder_speex));
 struct roar_audio_info  * info = &(state->info.pcm);
 int tmp;

 if ( self == NULL )
  return -1;

 // curruntly only 16 bit mode is supported
 if ( info->bits != 16 ) {
  free(self);
  return -1;
 }

 // curruntly only mono mode is supported
 if ( info->channels != 1 ) {
  free(self);
  return -1;
 }

 memset(self, 0, sizeof(struct roar_xcoder_speex));

 state->inst = self;

 self->mode  = ROAR_SPEEX_MODE_UWB;

 if (state->encode) {
  switch (self->mode) {
   case ROAR_SPEEX_MODE_NB:  self->xcoder = speex_encoder_init(&speex_nb_mode);  break;
   case ROAR_SPEEX_MODE_WB:  self->xcoder = speex_encoder_init(&speex_wb_mode);  break;
   case ROAR_SPEEX_MODE_UWB: self->xcoder = speex_encoder_init(&speex_uwb_mode); break;
  }
  tmp = 8;
  speex_encoder_ctl(self->xcoder, SPEEX_SET_QUALITY,    &tmp);
  speex_encoder_ctl(self->xcoder, SPEEX_GET_FRAME_SIZE, &(self->frame_size));
 } else {
  self->xcoder = NULL;
 }

 speex_bits_init(&(self->bits));

 return 0;
}

int roar_xcoder_speex_uninit     (struct roar_xcoder * state) {
 struct roar_xcoder_speex * self = state->inst;

 if ( self->xcoder != NULL ) {
  if (state->encode) {
   speex_encoder_destroy(self->xcoder);
  } else {
   speex_decoder_destroy(self->xcoder);
  }
 }

 speex_bits_destroy(&(self->bits));

 free(self);

 return 0;
}

int roar_xcoder_speex_packet_size(struct roar_xcoder * state, int samples) {
 struct roar_xcoder_speex * self = state->inst;
 if (!state->encode) {
  return -1;
 }

 return _16BIT * self->frame_size;
}

int roar_xcoder_speex_encode     (struct roar_xcoder * state, void * buf, size_t len) {
 struct roar_xcoder_speex * self = state->inst;
 uint16_t tmp_net;
 int pkg_len;

 if (!state->encode)
  return -1;

 ROAR_DBG("roar_xcoder_speex_encode(*): Encoding...");

 if ( state->stage == ROAR_XCODER_STAGE_INITED ) {
  if ( roar_vio_write(state->backend, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) != ROAR_SPEEX_MAGIC_LEN )
   return -1;
  state->stage = ROAR_XCODER_STAGE_MAGIC;
  ROAR_DBG("roar_xcoder_speex_encode(*): Wrote MAGIC");

  state->stage = ROAR_XCODER_STAGE_OPENING;

  tmp_net = ROAR_HOST2NET16(self->mode);
  if ( roar_vio_write(state->backend, &tmp_net, 2) != 2 )
   return -1;

  state->stage = ROAR_XCODER_STAGE_OPENED;
 }

 speex_bits_reset(&(self->bits));

 speex_encode_int(self->xcoder, (spx_int16_t *) buf, &(self->bits));

 pkg_len = speex_bits_write(&(self->bits), self->cc, ROAR_SPEEX_MAX_CC);

 tmp_net = ROAR_HOST2NET16(pkg_len);

 if ( roar_vio_write(state->backend, &tmp_net, 2) != 2 )
  return -1;

 if ( roar_vio_write(state->backend, self->cc, pkg_len) != pkg_len )
   return -1;

 return 0;
}

int roar_xcoder_speex_decode     (struct roar_xcoder * state, void * buf, size_t len) {
 return -1;
}

#endif

//ll
