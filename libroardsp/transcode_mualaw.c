//transcode_mualaw.c:

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

#define _CHECK() if ( state == NULL || state->info.pcm.bits != 16 || buf == NULL ) return -1

#define _CHECK_BUF(len) \
                        void * iobuf; \
                        size_t outbyte = (len)/2; \
                        _CHECK();     \
                        if ( (len) & 0x01 ) return -1; \
                        if ( provide_buffer(&iobuf, outbyte) == -1 ) \
                         return -1;

#define _SEND_RETURN()  if ( roar_vio_write(state->backend, iobuf, outbyte) != outbyte ) \
                         return -1; \
                        return 0;

#define _READ()         if ( roar_vio_read(state->backend, iobuf, outbyte) != outbyte ) \
                         return -1;

static int provide_buffer(void ** buf, size_t len) {
 static struct roar_buffer * bufbuf = NULL;
 size_t buflen;

 ROAR_DBG("provide_buffer(*) = ?");

 if ( bufbuf != NULL ) {
  if ( roar_buffer_get_len(bufbuf, &buflen) == -1 )
   return -1;

  if ( buflen >= len ) {
   if ( roar_buffer_get_data(bufbuf, buf) == -1 )
    return -1;

   ROAR_DBG("provide_buffer(*) = 0");
   return 0;
  } else {
   if ( roar_buffer_free(bufbuf) == -1 )
    return -1;
  }
 }

 if ( roar_buffer_new(&bufbuf, len) == -1 )
  return -1;

 if ( roar_buffer_get_data(bufbuf, buf) == -1 )
  return -1;

 ROAR_DBG("provide_buffer(*) = 0");
 return 0;
}

int roar_xcoder_alaw_encode(struct roar_xcoder * state, void * buf, size_t len) {
 _CHECK_BUF(len);

 ROAR_DBG("roar_xcoder_alaw_encode(*) = ?");

 if ( roardsp_conv_pcm162alaw(iobuf, buf, outbyte) == -1 )
  return -1;

 ROAR_DBG("roar_xcoder_alaw_encode(*) = ?");

 _SEND_RETURN();
}

int roar_xcoder_alaw_decode(struct roar_xcoder * state, void * buf, size_t len) {
 _CHECK_BUF(len);

 _READ();

 ROAR_DBG("roar_xcoder_alaw_decode(*): Start decoding..");

 if ( roardsp_conv_alaw2pcm16(buf, iobuf, outbyte) == -1 )
  return -1;

 ROAR_DBG("roar_xcoder_alaw_decode(*): Decoding sucessful");
 return 0;
}

int roar_xcoder_mulaw_encode(struct roar_xcoder * state, void * buf, size_t len) {
 _CHECK_BUF(len);

 if ( roardsp_conv_pcm162mulaw(iobuf, buf, outbyte) == -1 )
  return -1;

 _SEND_RETURN();
}

int roar_xcoder_mulaw_decode(struct roar_xcoder * state, void * buf, size_t len) {
 _CHECK_BUF(len);

 _READ();

 if ( roardsp_conv_mulaw2pcm16(buf, iobuf, outbyte) == -1 )
  return -1;

 return 0;
}

//ll
