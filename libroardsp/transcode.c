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

#define _FUNC(func)        (state->entry->func)
#define _CHECK_STATE()     (!(state == NULL || state->entry == NULL))
#define _CHECK_FUNC(func)  (_CHECK_STATE() && _FUNC(func) != NULL)
#define _CHECK_BASIC(func) if ( !_CHECK_FUNC(func) ) return -1
#define _CHECK()           if ( !_CHECK_STATE() ) return -1

static struct roar_xcoder_entry g_xcoders[] = {
 {ROAR_CODEC_ALAW,  roar_xcoder_dummy_inituninit, roar_xcoder_dummy_inituninit, roar_xcoder_dummy_packet_size_any,
                     roar_xcoder_alaw_encode,  roar_xcoder_alaw_decode},
 {ROAR_CODEC_MULAW, roar_xcoder_dummy_inituninit, roar_xcoder_dummy_inituninit, roar_xcoder_dummy_packet_size_any,
                     roar_xcoder_mulaw_encode, roar_xcoder_mulaw_decode},
#ifdef ROAR_HAVE_LIBCELT
 {ROAR_CODEC_ROAR_CELT, roar_xcoder_celt_init, roar_xcoder_celt_uninit, roar_xcoder_celt_packet_size,
                     roar_xcoder_celt_encode,  roar_xcoder_celt_decode},
#endif
#ifdef ROAR_HAVE_LIBSPEEX
 {ROAR_CODEC_ROAR_SPEEX, roar_xcoder_speex_init, roar_xcoder_speex_uninit, roar_xcoder_speex_packet_size,
                     roar_xcoder_speex_encode, roar_xcoder_speex_decode},
#endif
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

 ROAR_DBG("roar_xcoder_proc_packet(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( state->backend == NULL )
  return -1;

 if ( state->packet_len > 0 && state->packet_len != len )
  return -1;

 ROAR_DBG("roar_xcoder_proc_packet(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( state->encode ) {
  _CHECK_BASIC(encode);
  ROAR_DBG("roar_xcoder_proc_packet(state=%p, buf=%p, len=%lu) = ? // _CHECK_BASIC(encode) -> OK", state, buf, (unsigned long)len);
  return _FUNC(encode)(state, buf, len);
 } else {
  _CHECK_BASIC(decode);
  ROAR_DBG("roar_xcoder_proc_packet(state=%p, buf=%p, len=%lu) = ? // _CHECK_BASIC(decode) -> OK", state, buf, (unsigned long)len);
  return _FUNC(decode)(state, buf, len);
 }
}

int roar_xcoder_proc       (struct roar_xcoder * state, void * buf, size_t len) {
 struct roar_buffer_stats ringstats;
 struct roar_buffer * bufbuf;
 void               * bufdata;
 size_t               curlen;

 ROAR_DBG("roar_xcoder_proc(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( state == NULL )
  return -1;

 if ( buf == NULL && len != 0 )
  return -1;

 if ( state->packet_len == -1 )
  if ( roar_xcoder_packet_size(state, -1) == -1 )
   return -1;

 ROAR_DBG("roar_xcoder_proc(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( state->packet_len == 0 )
  return roar_xcoder_proc_packet(state, buf, len);

 ROAR_DBG("roar_xcoder_proc(state=%p, buf=%p, len=%lu): state->packet_len=%li", state, buf, (unsigned long)len, (long int) state->packet_len);

 if ( state->iobuffer == NULL ) {
  while ( len >= state->packet_len ) {
   if ( roar_xcoder_proc_packet(state, buf, state->packet_len) == -1 ) {
    ROAR_DBG("roar_xcoder_proc(state=%p, buf=%p, len=%lu) = -1 // roar_xcoder_proc_packet() error", state, buf, (unsigned long)len);
    return -1;
   }

   buf += state->packet_len;
   len -= state->packet_len;
  }

  if ( !len ) {
   ROAR_DBG("roar_xcoder_proc(state=%p, buf=%p, len=%lu) = 0", state, buf, (unsigned long)len);
   return 0;
  }

  ROAR_DBG("roar_xcoder_proc(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

  if ( state->encode ) {
   curlen = len;
  } else {
   curlen = state->packet_len;
  }

  if ( roar_buffer_new(&bufbuf, curlen) == -1 )
   return -1;

  if ( roar_buffer_get_data(bufbuf, &bufdata) == -1 ) {
   roar_buffer_free(bufbuf);
   return -1;
  }

  if ( state->encode ) {
   memcpy(bufdata, buf, len);
  } else {
   if ( roar_xcoder_proc_packet(state, bufdata, state->packet_len) == -1 ) {
    roar_buffer_free(bufbuf);
    return -1;
   }

   curlen = len;

   if ( roar_buffer_shift_out(&bufbuf, buf, &curlen) == -1 ) {
    roar_buffer_free(bufbuf);
    return -1;
   }

   if ( curlen < len ) { // this should never happen!
    roar_buffer_free(bufbuf);
    return -1;
   }
  }

  state->iobuffer = bufbuf;
 } else {
  if ( state->encode ) {
   if ( roar_buffer_new(&bufbuf, len) == -1 )
    return -1;

   if ( roar_buffer_get_data(bufbuf, &bufdata) == -1 ) {
    roar_buffer_free(bufbuf);
    return -1;
   }

   memcpy(bufdata, buf, len);

   if ( roar_buffer_add(state->iobuffer, bufbuf) == -1 ) {
    roar_buffer_free(bufbuf);
    return -1;
   }

   if ( roar_buffer_ring_stats(state->iobuffer, &ringstats) == -1 )
    return -1;

   if ( roar_buffer_new(&bufbuf, state->packet_len) == -1 )
    return -1;

   if ( roar_buffer_get_data(bufbuf, &bufdata) == -1 ) {
    roar_buffer_free(bufbuf);
    return -1;
   }

   while ( ringstats.bytes > state->packet_len ) {
    curlen = state->packet_len;
    if ( roar_buffer_shift_out(&(state->iobuffer), bufdata, &curlen) == -1 ) {
     roar_buffer_free(bufbuf);
     return -1;
    }

    if ( curlen < state->packet_len ) { // this should not happen...
     roar_buffer_free(bufbuf);
     return -1;
    }

    if ( roar_xcoder_proc_packet(state, bufdata, state->packet_len) == -1 ) {
     roar_buffer_free(bufbuf);
     return -1;
    }

    if ( roar_buffer_ring_stats(state->iobuffer, &ringstats) == -1 ) {
     roar_buffer_free(bufbuf);
     return -1;
    }
   }

   if ( roar_buffer_free(bufbuf) == -1 )
    return -1;
  } else {
   curlen = len;

   if ( roar_buffer_shift_out(&(state->iobuffer), buf, &curlen) == -1 ) {
    return -1;
   }

   if ( curlen == len )
    return -1;

   // we now have curlen < len and state->iobuffer == NULL
   // as no data is left in the buffer, need to get some new data.
   // we simply call ourself to get some more data...

   if ( state->iobuffer == NULL ) {
    ROAR_WARN("roar_xcoder_proc(state=%p, buf=%p, len=%lu): iobuffer != NULL, "
                                "This is a bug in libroar{dsp,} or some hardware is broken",
                   state, buf, (unsigned long)len);
    return -1;
   }

   len -= curlen;
   buf += curlen;

   return roar_xcoder_proc(state, buf, len);
  }
 }

 return 0;
}

int roar_bixcoder_init(struct roar_bixcoder * state, struct roar_audio_info * info, struct roar_vio_calls * vio) {
 if ( state == NULL || info == NULL || vio == NULL )
  return -1;

 memset(state, 0, sizeof(struct roar_bixcoder));

 if ( roar_xcoder_init(&(state->encoder), 1, info, vio) == -1 )
  return -1;

 if ( roar_xcoder_init(&(state->decoder), 0, info, vio) == -1 ) {
  roar_xcoder_close(&(state->encoder));
  return -1;
 }

 return 0;
}

int roar_bixcoder_packet_size (struct roar_bixcoder * state, int samples) {
 int ret;

 ROAR_DBG("roar_bixcoder_packet_size(state=%p, samples=%i) = ?", state, samples);

 if ( state == NULL )
  return -1;

 if ( (ret = roar_xcoder_packet_size(&(state->encoder), samples)) == -1 )
  return -1;

 ROAR_DBG("roar_bixcoder_packet_size(state=%p, samples=%i): ret=%i", state, samples, ret);

// TODO: we need a lot hope here...
/*
 if ( roar_xcoder_packet_size(&(state->decoder), ret) != ret )
  return -1;
*/

 ROAR_DBG("roar_bixcoder_packet_size(state=%p, samples=%i) = %i", state, samples, ret);
 return ret;
}

int roar_bixcoder_close       (struct roar_bixcoder * state) {
 int ret = 0;

 if ( state == NULL )
  return -1;

 ret = roar_xcoder_close(&(state->encoder));

 if ( roar_xcoder_close(&(state->decoder)) == -1 )
  return -1;

 return ret;
}

int roar_bixcoder_read_packet (struct roar_bixcoder * state, void * buf, size_t len) {

 ROAR_DBG("roar_bixcoder_read_packet(state=%p, buf=%p, len=%lu) = ?", state, buf, (unsigned long)len);

 if ( state == NULL )
  return -1;

 return roar_xcoder_proc_packet(&(state->decoder), buf, len);
}

int roar_bixcoder_read        (struct roar_bixcoder * state, void * buf, size_t len) {
 if ( state == NULL )
  return -1;

 return roar_xcoder_proc(&(state->decoder), buf, len);
}

int roar_bixcoder_write_packet(struct roar_bixcoder * state, void * buf, size_t len) {
 if ( state == NULL )
  return -1;

 return roar_xcoder_proc_packet(&(state->encoder), buf, len);
}

int roar_bixcoder_write       (struct roar_bixcoder * state, void * buf, size_t len) {
 if ( state == NULL )
  return -1;

 return roar_xcoder_proc(&(state->encoder), buf, len);
}

// dummy functions used by some de/encoders:
int roar_xcoder_dummy_inituninit(struct roar_xcoder * state) {
 return 0;
}

int roar_xcoder_dummy_packet_size_any(struct roar_xcoder * state, int samples) {
 // the case samples=-1/samples!=-1 based things are done in the general func
 return 0;
}

//ll
