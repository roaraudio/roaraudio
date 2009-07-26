//transcode.c:

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

#define _FUNC(func) (state->entry->func)
#define _CHECK_STATE() (!(state == NULL || state->entry == NULL))
#define _CHECK_FUNC(func) (_CHECK_STATE() && _FUNC(func) != NULL)
#define _CHECK_BASIC(func) if ( !_CHECK_FUNC(func) ) return -1
#define _CHECK() if ( !_CHECK_STATE() ) return -1

static struct roar_xcoder_entry g_xcoders[] = {
 {ROAR_CODEC_ALAW,  roar_xcoder_dummy_inituninit, roar_xcoder_dummy_inituninit, roar_xcoder_dummy_packet_size_any,
                     NULL, NULL}, 
 {ROAR_CODEC_MULAW, roar_xcoder_dummy_inituninit, roar_xcoder_dummy_inituninit, roar_xcoder_dummy_packet_size_any,
                     NULL, NULL}, 
 {-1, NULL, NULL, NULL, NULL, NULL}
};

int roar_xcoder_init(struct roar_xcoder * state, int encoder, struct roar_audio_info * info, struct roar_vio_calls * vio) {
 int i;

 if ( state == NULL || info == NULL )
  return -1;

 memset(state, 0, sizeof(struct roar_xcoder));

 for (i = 0; g_xcoders[i].codec != -1; i++) {
  if ( g_xcoders[i].codec == info->codec ) {
   state->entry = &(g_xcoders[i]);
   break;
  }
 }

 if ( state->entry == NULL )
  return -1;

 state->stage      = ROAR_XCODER_STAGE_NONE;
 state->encode     = encoder;
 state->packet_len = -1;

 if ( roar_xcoder_set_backend(state, vio) == -1 )
  return -1;

 memcpy(&(state->info.coded), info, sizeof(struct roar_audio_info));
 memcpy(&(state->info.pcm  ), info, sizeof(struct roar_audio_info));

 state->info.pcm.codec = ROAR_CODEC_DEFAULT;

 if ( _FUNC(init) == NULL )
  return -1;

 if ( _FUNC(init)(state) != 0 )
  return -1;

 state->stage      = ROAR_XCODER_STAGE_INITED;

 return 0;
}

int roar_xcoder_set_backend(struct roar_xcoder * state, struct roar_vio_calls * vio) {
 _CHECK();

 if ( vio == NULL && state->backend != NULL )
  return -1;

 state->backend = vio;

 return 0;
}
int roar_xcoder_packet_size(struct roar_xcoder * state, int samples) {
 _CHECK_BASIC(packet_size);

 state->packet_len = state->entry->packet_size(state, samples);

 if ( state->packet_len == 0 ) {
  if ( samples < 1 ) {
   return ROAR_RATE_DEFAULT/100;
  } else {
   return samples;
  }
 }

 return state->packet_len;
}

int roar_xcoder_close      (struct roar_xcoder * state) {
 _CHECK_BASIC(uninit);


 if ( state->iobuffer != NULL ) {
  roar_xcoder_proc(state, NULL, 0); // try to flush
  roar_buffer_free(state->iobuffer);
 }

 return _FUNC(uninit)(state);
}

int roar_xcoder_proc_packet(struct roar_xcoder * state, void * buf, size_t len) {
 _CHECK();

 if ( state->packet_len > 0 && state->packet_len != len )
  return -1;

 if ( state->encode ) {
  _CHECK_BASIC(encode);
  return _FUNC(encode)(state, buf, len);
 } else {
  _CHECK_BASIC(decode);
  return _FUNC(decode)(state, buf, len);
 }
}

int roar_xcoder_proc       (struct roar_xcoder * state, void * buf, size_t len) {
 return -1;
}

int roar_bixcoder_init(struct roar_bixcoder * state, struct roar_audio_info * info, struct roar_vio_calls * vio);
int roar_bixcoder_packet_size (struct roar_bixcoder * state, int samples);
int roar_bixcoder_close       (struct roar_bixcoder * state);
int roar_bixcoder_read_packet (struct roar_bixcoder * state, void * buf, size_t len);
int roar_bixcoder_read        (struct roar_bixcoder * state, void * buf, size_t len);
int roar_bixcoder_write_packet(struct roar_bixcoder * state, void * buf, size_t len);
int roar_bixcoder_write       (struct roar_bixcoder * state, void * buf, size_t len);

// dummy functions used by some de/encoders:
int roar_xcoder_dummy_inituninit(struct roar_xcoder * state) {
 return 0;
}

int roar_xcoder_dummy_packet_size_any(struct roar_xcoder * state, int samples) {
 // the case samples=-1/samples!=-1 based things are done in the general func
 return 0;
}

//ll
