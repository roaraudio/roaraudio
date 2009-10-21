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

#define _HAVE_CCFG(x) (self->codec_config != NULL && (self->codec_config->para_set & (x)))

int roar_xcoder_speex_init       (struct roar_xcoder * state) {
 struct roar_xcoder_speex * self = malloc(sizeof(struct roar_xcoder_speex));
 struct roar_audio_info  * info = &(state->info.pcm);
 int tmp;

 ROAR_DBG("roar_xcoder_speex_init(*): sizeof(*self) = %lu", (unsigned long)sizeof(*self));

 if ( self == NULL )
  return -1;

 // curruntly only 16 bit mode is supported
 if ( info->bits != 16 ) {
  free(self);
  return -1;
 }

 // only mono/stereo mode is supported
 switch (info->channels) {
  case 1: self->stereo = 0; break;
  case 2: self->stereo = 1; break;
  default:
    free(self);
    return -1;
 }

 memset(self, 0, sizeof(struct roar_xcoder_speex));

 state->inst = self;

 self->mode  = ROAR_SPEEX_MODE_UWB;

 if (state->encode) {
  self->codec_config = roar_libroar_config_codec_get(ROAR_CODEC_ROAR_SPEEX, 0);
  self->max_cc = ROAR_SPEEX_MAX_CC;

  ROAR_DBG("roar_xcoder_speex_init(*): self->codec_config=%p", self->codec_config);

  if ( _HAVE_CCFG(ROAR_LIBROAR_CONFIG_PSET_MAX_CC) ) {
   self->max_cc = self->codec_config->max_cc;
  }

  if ( info->rate < 12000 ) {
   self->mode = ROAR_SPEEX_MODE_NB;  // optimized for 8kHz
  } else if ( info->rate < 24000 ) {
   self->mode = ROAR_SPEEX_MODE_WB;  // optimized for 16kHz
  } else {
   self->mode = ROAR_SPEEX_MODE_UWB; // optimized for 32kHz
  }

  if ( _HAVE_CCFG(ROAR_LIBROAR_CONFIG_PSET_MODE) ) {
   // NB, WB and UWB mode numbers are the same for Speex and codec config.
   self->mode   = self->codec_config->mode;
  }

  switch (self->mode) {
   case ROAR_SPEEX_MODE_NB:  self->xcoder = speex_encoder_init(&speex_nb_mode);  break;
   case ROAR_SPEEX_MODE_WB:  self->xcoder = speex_encoder_init(&speex_wb_mode);  break;
   case ROAR_SPEEX_MODE_UWB: self->xcoder = speex_encoder_init(&speex_uwb_mode); break;
   default: return -1;
  }

  if ( _HAVE_CCFG(ROAR_LIBROAR_CONFIG_PSET_COMPLEXITY) ) {
   tmp = self->codec_config->complexity / 256;
  } else {
   tmp = 8;
  }
  speex_encoder_ctl(self->xcoder, SPEEX_SET_QUALITY,       &tmp);

  if ( _HAVE_CCFG(ROAR_LIBROAR_CONFIG_PSET_Q) ) {
   tmp = self->codec_config->q / 256;
   speex_encoder_ctl(self->xcoder, SPEEX_SET_QUALITY,      &tmp);
  }

  if ( _HAVE_CCFG(ROAR_LIBROAR_CONFIG_PSET_VBR) ) {
   tmp = self->codec_config->vbr ? 1 : 0;
   speex_encoder_ctl(self->xcoder, SPEEX_SET_VBR,          &tmp);
  }

  if ( _HAVE_CCFG(ROAR_LIBROAR_CONFIG_PSET_DTX) ) {
   tmp = self->codec_config->dtx ? 1 : 0;
   speex_encoder_ctl(self->xcoder, SPEEX_SET_DTX,          &tmp);
  }

  tmp = info->rate;
  speex_encoder_ctl(self->xcoder, SPEEX_SET_SAMPLING_RATE, &tmp);
  speex_encoder_ctl(self->xcoder, SPEEX_GET_FRAME_SIZE,    &(self->frame_size));
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

 if (!state->encode)
  if (state->stage != ROAR_XCODER_STAGE_OPENED)
   return -1;

 return _16BIT * self->frame_size * (self->stereo ? 2 : 1);
}

int roar_xcoder_speex_proc_header(struct roar_xcoder * state) {
 struct roar_xcoder_speex * self = state->inst;
 uint16_t tmp_net;
 int tmp;
 SpeexStereoState stereo = SPEEX_STEREO_STATE_INIT;
 SpeexCallback callback;
 char magic[ROAR_SPEEX_MAGIC_LEN];

 // we do allready open streams not considder an error.
 if ( state->stage == ROAR_XCODER_STAGE_OPENED )
  return 0;

 // everything else expected an INITED stream (OPENING, MAGIC,..)
 // is an error...
 if ( state->stage != ROAR_XCODER_STAGE_INITED )
  return -1;

 if ( state->encode ) {
  if ( roar_vio_write(state->backend, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) != ROAR_SPEEX_MAGIC_LEN )
   return -1;
  state->stage = ROAR_XCODER_STAGE_MAGIC;
  ROAR_DBG("roar_xcoder_speex_encode(*): Wrote MAGIC");

  state->stage = ROAR_XCODER_STAGE_OPENING;

  tmp_net = ROAR_HOST2NET16(self->mode);
  if ( roar_vio_write(state->backend, &tmp_net, 2) != 2 )
   return -1;

  state->stage = ROAR_XCODER_STAGE_OPENED;
 } else {
  ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): state->stage = INITED", state, buf, (unsigned long)len);
  if ( roar_vio_read(state->backend, magic, ROAR_SPEEX_MAGIC_LEN) != ROAR_SPEEX_MAGIC_LEN )
   return -1;

  if ( memcmp(magic, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) != 0 )
   return -1;

  state->stage = ROAR_XCODER_STAGE_MAGIC;
  ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): state->stage = MAGIC", state, buf, (unsigned long)len);

  if ( roar_vio_read(state->backend, &tmp_net, 2) != 2 )
   return -1;

  self->mode = ROAR_NET2HOST16(tmp_net);

  state->stage = ROAR_XCODER_STAGE_OPENING;
  ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): state->stage = OPENING", state, buf, (unsigned long)len);

  switch (self->mode) {
   case ROAR_SPEEX_MODE_NB:  self->xcoder = speex_decoder_init(&speex_nb_mode);  break;
   case ROAR_SPEEX_MODE_WB:  self->xcoder = speex_decoder_init(&speex_wb_mode);  break;
   case ROAR_SPEEX_MODE_UWB: self->xcoder = speex_decoder_init(&speex_uwb_mode); break;
   default: return -1;
  }

  ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): opened decoder state", state, buf, (unsigned long)len);

  tmp=1;
  speex_decoder_ctl(self->xcoder, SPEEX_SET_ENH, &tmp);
  tmp = state->info.pcm.rate;
  speex_decoder_ctl(self->xcoder, SPEEX_SET_SAMPLING_RATE, &tmp);
  speex_decoder_ctl(self->xcoder, SPEEX_GET_FRAME_SIZE, &(self->frame_size));

  ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): self->stereo = %i", state, buf, (unsigned long)len, self->stereo);

  if ( self->stereo ) {
   memcpy(&(self->stereo_state), &stereo, sizeof(self->stereo_state));

   callback.callback_id = SPEEX_INBAND_STEREO;
   callback.func = speex_std_stereo_request_handler;
   callback.data = &(self->stereo_state);

   speex_decoder_ctl(self->xcoder, SPEEX_SET_HANDLER, &callback);
  }

  state->stage = ROAR_XCODER_STAGE_OPENED;
  ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): state->stage = OPENED", state, buf, (unsigned long)len);
 }

 // on no error...
 return 0;
}

int roar_xcoder_speex_encode     (struct roar_xcoder * state, void * buf, size_t len) {
 struct roar_xcoder_speex * self = state->inst;
 int pkg_len;

 if (!state->encode)
  return -1;

 ROAR_DBG("roar_xcoder_speex_encode(*): Encoding...");

 if ( state->stage == ROAR_XCODER_STAGE_INITED )
  if ( roar_xcoder_speex_proc_header(state) == -1 )
   return -1;

 speex_bits_reset(&(self->bits));

 if ( self->stereo )
  speex_encode_stereo_int((spx_int16_t *) buf, self->frame_size, &(self->bits));

 speex_encode_int(self->xcoder, (spx_int16_t *) buf, &(self->bits));

 pkg_len = speex_bits_write(&(self->bits), self->cc + 2, self->max_cc);

 *((uint16_t*)self->cc) = ROAR_HOST2NET16(pkg_len);

 if ( roar_vio_write(state->backend, self->cc, pkg_len + 2) != (pkg_len + 2) )
   return -1;

 return 0;
}

// TODO: move all the init thingys into a seperate function
int roar_xcoder_speex_decode     (struct roar_xcoder * state, void * buf, size_t len) {
 struct roar_xcoder_speex * self = state->inst;
 uint16_t tmp_net;
 int pkg_len;

 ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( state->stage == ROAR_XCODER_STAGE_INITED )
  if ( roar_xcoder_speex_proc_header(state) == -1 )
   return -1;

 ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu): state->stage = %s", state, buf, (unsigned long)len,
          state->stage == ROAR_XCODER_STAGE_OPENED ? "OPENED" : "???"
         );

 if ( roar_vio_read(state->backend, &tmp_net, 2) != 2 )
  return -1;

 pkg_len = ROAR_NET2HOST16(tmp_net);

 if ( pkg_len > ROAR_SPEEX_MAX_CC )
  return -1;

 ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( roar_vio_read(state->backend, self->cc, pkg_len) != pkg_len )
  return -1;

 speex_bits_read_from(&(self->bits), self->cc, pkg_len);

 speex_decode_int(self->xcoder, &(self->bits), buf);

 if ( self->stereo ) {
  speex_decode_stereo_int(buf, self->frame_size, &(self->stereo_state));
 }

 ROAR_DBG("roar_xcoder_speex_decode(state=%p, buf=%p, len=%lu) = 0", state, buf, (unsigned long)len);
 return 0;
}

#endif

//ll
