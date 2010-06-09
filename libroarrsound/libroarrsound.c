//libroarrsound.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from RSound.
 *  They are copyrighted by Hans-Kristian 'maister' Arntzen.
 *
 *  This file is part of libroarrsound a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 */

#include "libroarrsound.h"

static size_t libroarrsound_fmt2fs (enum rsd_format format) {
 switch (format) {
  case RSD_S16_LE:
  case RSD_S16_BE:
  case RSD_S16_NE:
  case RSD_U16_LE:
  case RSD_U16_BE:
  case RSD_U16_NE:
    return 16;
   break;
  case RSD_U8:
  case RSD_S8:
  case RSD_ALAW:
  case RSD_MULAW:
    return 8;
   break;
  default:
    return 0;
   break;
 }
}

int rsd_init (rsound_t **rd) {
 struct libroarrsound * self;

 ROAR_DBG("rsd_init(rd=%p) = ?", rd);

 if ( rd == NULL )
  return -1;

 self = roar_mm_malloc(sizeof(struct libroarrsound));

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct libroarrsound));

 *rd = (rsound_t*)self;

 self->rsound.conn.socket     = -1;
 self->rsound.conn.ctl_socket = -1;
 self->rsound.channels        = ROAR_CHANNELS_DEFAULT;
 self->rsound.rate            = ROAR_RATE_DEFAULT;
 self->rsound.format          = RSD_S16_LE;

 strncpy(self->rsound.identity, "libroarrsound client", sizeof(self->rsound.identity) - 1);
 self->rsound.identity[sizeof(self->rsound.identity)-1] = 0;

 return 0;
}

/* Frees an rsound_t struct. */
int rsd_free (rsound_t *rd) {
 struct libroarrsound * self = (struct libroarrsound *)rd;
 int ret = 0;

 if ( self == NULL )
  return -1;

 if ( self->flags & LIBROARRSOUND_FLAGS_CONNECTED )
  if ( roar_disconnect(&(self->con)) == -1 )
   ret = -1;

 if ( self->flags & LIBROARRSOUND_FLAGS_STREAMING )
  if ( roar_vio_close(&(self->vio)) == -1 )
   ret = -1;

 if ( self->rsound.host != NULL )
  roar_mm_free(self->rsound.host);

 if ( self->rsound.port != NULL )
  roar_mm_free(self->rsound.port);

 roar_mm_free(self);

 return ret;
}

int rsd_set_param (rsound_t *rd, enum rsd_settings option, void* param) {
 struct libroarrsound * self = (struct libroarrsound *)rd;

 ROAR_DBG("rsd_set_param(rd=%p, option=%i, param=%p) = ?", rd, option, param);

 if ( self == NULL || param == NULL )
  return -1;

 switch (option) {
  // connection settings:
  case RSD_HOST:
    if ( self->rsound.host != NULL )
     roar_mm_free(self->rsound.host);

    self->rsound.host = roar_mm_strdup(param);
   break;
  case RSD_PORT:
    if ( self->rsound.port != NULL )
     roar_mm_free(self->rsound.port);

    self->rsound.port = roar_mm_strdup(param);
   break;
#ifdef RSD_IDENTITY
  case RSD_IDENTITY:
    strncpy(self->rsound.identity, param, sizeof(self->rsound.identity) - 1);
    self->rsound.identity[sizeof(self->rsound.identity)-1] = 0;
   break;
#endif
  // stream settings:
  case RSD_SAMPLERATE:
    self->rsound.rate = *(int*)param;
   break;
  case RSD_CHANNELS:
    self->rsound.channels = *(int*)param;
   break;
  case RSD_FORMAT:
    self->rsound.format = *(int*)param;
    self->rsound.framesize = libroarrsound_fmt2fs(self->rsound.format);
   break;
  default:
/*
   RSD_BUFSIZE,
   RSD_LATENCY,
*/
     ROAR_DBG("rsd_set_param(rd=%p, option=%i, param=%p) = -1", rd, option, param);
    return -1;
   break;
 }

 ROAR_DBG("rsd_set_param(rd=%p, option=%i, param=%p) = 0", rd, option, param);
 return 0;
}

static int libroarrsound_connect (struct libroarrsound * self) {
 char * host;

 if ( self->flags & LIBROARRSOUND_FLAGS_CONNECTED )
  return 0;

 host = self->rsound.host;

 if ( host == NULL )
  host = getenv("RSD_SERVER");

 // FIXME: we currently ignore the port. :(

 ROAR_DBG("libroarrsound_connect(self=%p): try to connect to: %s", self, host);

 if ( roar_simple_connect(&(self->con), host, self->rsound.identity) == -1 ) {
  ROAR_DBG("libroarrsound_connect(self=%p) = -1 // can not connect to server", self);
  return -1;
 }

 self->flags |= LIBROARRSOUND_FLAGS_CONNECTED;

 ROAR_DBG("libroarrsound_connect(self=%p) = 0", self);
 return 0;
}

/* Establishes connection to server. Might fail if connection can't be established or that one of
   the mandatory options isn't set in rsd_set_param(). This needs to be called after params have been set
   with rsd_set_param(), and before rsd_write(). */
int rsd_start (rsound_t *rd) {
 struct libroarrsound * self = (struct libroarrsound *)rd;
 int bits  = 16;
 int codec;

 ROAR_DBG("rsd_start(rd=%p) = ?", rd);

 if ( self == NULL )
  return -1;

 if ( self->flags & LIBROARRSOUND_FLAGS_STREAMING )
  return 0;

 ROAR_DBG("rsd_start(rd=%p) = ?", rd);

 if ( !(self->flags & LIBROARRSOUND_FLAGS_CONNECTED) ) {
  if ( libroarrsound_connect(self) == -1 )
   return -1;
 }

 ROAR_DBG("rsd_start(rd=%p) = ?", rd);

 switch (self->rsound.format) {
  case RSD_S16_LE:
    codec = ROAR_CODEC_PCM_S_LE;
   break;
  case RSD_S16_BE:
    codec = ROAR_CODEC_PCM_S_BE;
   break;
  case RSD_S16_NE:
    codec = ROAR_CODEC_PCM_S;
   break;
  case RSD_U16_LE:
    codec = ROAR_CODEC_PCM_U_LE;
   break;
  case RSD_U16_BE:
    codec = ROAR_CODEC_PCM_U_BE;
   break;
  case RSD_U16_NE:
    codec = ROAR_CODEC_PCM_U;
   break;
  case RSD_S8:
    codec = ROAR_CODEC_PCM_S;
    bits  = 8;
   break;
  case RSD_U8:
    codec = ROAR_CODEC_PCM_U;
    bits  = 8;
   break;
  case RSD_ALAW:
    codec = ROAR_CODEC_ALAW;
    bits  = 8;
   break;
  case RSD_MULAW:
    codec = ROAR_CODEC_MULAW;
    bits  = 8;
   break;
  default:
    return -1;
   break;
 }

 ROAR_DBG("rsd_start(rd=%p) = ?", rd);

 if ( roar_vio_simple_new_stream_obj(&(self->vio), &(self->con), &(self->stream),
                                     self->rsound.rate, self->rsound.channels, bits, codec, ROAR_DIR_PLAY) == -1 )
  return -1;

 ROAR_DBG("rsd_start(rd=%p) = ?", rd);

 self->flags |= LIBROARRSOUND_FLAGS_STREAMING;

 ROAR_DBG("rsd_start(rd=%p) = 0", rd);

 return 0;
}

/* Shuts down the rsound data structures, but returns the file descriptor associated with the connection.
   The control socket will be shut down. If this function returns a negative number, the exec failed,
   but the data structures will not be teared down.
   Should a valid file descriptor be returned, it will always be non-blocking.  <<-- FIXME? */
int rsd_exec (rsound_t *rd) {
 struct libroarrsound * self = (struct libroarrsound *)rd;
 int fh;

 if ( !(self->flags & LIBROARRSOUND_FLAGS_STREAMING) )
  if ( rsd_start(rd) == -1 )
   return -1;

 if ( roar_vio_ctl(&(self->vio), ROAR_VIO_CTL_GET_FH, &fh) == -1 )
  return -1;

 if ( fh == -1 )
  return -1;

 if ( roar_stream_exec(&(self->con), &(self->stream)) == -1 )
  return -1;

 // reset flags:
 if ( self->flags & LIBROARRSOUND_FLAGS_CONNECTED )
  self->flags -= LIBROARRSOUND_FLAGS_CONNECTED;

 if ( self->flags & LIBROARRSOUND_FLAGS_STREAMING )
  self->flags -= LIBROARRSOUND_FLAGS_STREAMING;

 // we hope nothing goes wrong here:
 rsd_free(rd);

 return fh;
}

/* Disconnects from server. All audio data still in network buffer and other buffers will be dropped.
   To continue playing, you will need to rsd_start() again. */
int rsd_stop (rsound_t *rd) {
 struct libroarrsound * self = (struct libroarrsound *)rd;
 int ret;

 if ( self == NULL )
  return -1;

 if ( !(self->flags & LIBROARRSOUND_FLAGS_STREAMING) )
  return 0;

 ret = roar_vio_close(&(self->vio));

 self->flags -= LIBROARRSOUND_FLAGS_STREAMING;

 return ret;
}

/* Writes from buf to the internal buffer. Might fail if no connection is established,
   or there was an unexpected error. This function will block until all data has
   been written to the buffer. This function will return the number of bytes written to the buffer,
   or 0 should it fail (disconnection from server). You will have to restart the stream again should this occur. */
size_t rsd_write (rsound_t *rd, const void * buf, size_t size) {
 struct libroarrsound * self = (struct libroarrsound *)rd;
 ssize_t ret;

 if ( self == NULL )
  return -1;

 if ( !(self->flags & LIBROARRSOUND_FLAGS_STREAMING) )
  return 0;

 ret = roar_vio_write(&(self->vio), (void*)buf, size);

 if ( ret == -1 )
  return 0;

 return ret;
}

/* Gets the position of the buffer pointer.
   Not really interesting for normal applications.
   Might be useful for implementing rsound on top of other blocking APIs. */
size_t rsd_pointer (rsound_t *rd);

/* Aquires how much data can be written to the buffer without blocking */
size_t rsd_get_avail (rsound_t *rd);

/* Aquires the latency at the moment for the audio stream. It is measured in bytes. Useful for syncing video and audio. */
size_t rsd_delay (rsound_t *rd) {
 (void)rd;
 return 0; // TODO: FIXME: write some code to read pos from server.
}

/* Utility for returning latency in milliseconds. */
size_t rsd_delay_ms (rsound_t *rd) {
 if ( rd == NULL )
  return -1;

 if ( rd->rate <= 0 || rd->channels <= 0 )
  return -1;

 return (rsd_delay(rd) * 1000) / (rd->rate * rd->channels * rd->framesize);
}

/* Returns bytes per sample */
int rsd_samplesize( rsound_t *rd ) {
 if ( rd == NULL )
  return -1;

 return rd->framesize;
}

/* Will sleep until latency of stream reaches maximum allowed latency defined earlier by rsd_set_param - RSD_LATENCY
   Useful for hard headed blocking I/O design where user defined latency is needed. If rsd_set_param hasn't been set
   with RSD_LATENCY, this function will do nothing. */
void rsd_delay_wait(rsound_t *rd);


/* Pauses or unpauses a stream. pause -> enable = 1
   This function essentially calls on start() and stop(). This behavior might be changed later. */
int rsd_pause (rsound_t *rd, int enable) {
 struct libroarrsound * self = (struct libroarrsound *)rd;

 if ( self == NULL )
  return -1;

 if ( !(self->flags & LIBROARRSOUND_FLAGS_STREAMING) )
  return -1;

 return roar_stream_set_flags(&(self->con), &(self->stream), ROAR_FLAG_PAUSE, enable ? ROAR_SET_FLAG : ROAR_RESET_FLAG);
}

//ll
