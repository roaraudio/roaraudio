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

static size_t libroarrsound_fmt2fs (enum format format) {
 switch (format) {
  case RSD_S16_LE:
  case RSD_S16_BE:
  case RSD_U16_LE:
  case RSD_U16_BE:
    return 16;
   break;
  case RSD_U8:
  case RSD_S8:
    return 8;
   break;
  default:
    return 0;
   break;
 }
}

int rsd_init (rsound_t **rd) {
 struct libroarrsound * self;

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

int rsd_set_param (rsound_t *rd, int option, void* param) {
 struct libroarrsound * self = (struct libroarrsound *)rd;

 if ( self == NULL || param == NULL )
  return -1;

 switch ((enum settings)option) {
  // connection settings:
  case RSD_HOST:
    self->rsound.host = roar_mm_strdup(param);
   break;
  case RSD_PORT:
    self->rsound.port = roar_mm_strdup(param);
   break;
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
  default:
/*
   RSD_BUFSIZE,
   RSD_LATENCY,
*/
    return -1;
   break;
 }

 return 0;
}

static int libroarrsound_connect (struct libroarrsound * self) {
 char * host = NULL;

 if ( self->flags & LIBROARRSOUND_FLAGS_CONNECTED )
  return 0;

 if ( self->rsound.host != NULL )
  host = self->rsound.host;

 // FIXME: we currently ignore the port. :(

 if ( roar_simple_connect(&(self->con), host, "libroarrsound client") == -1 )
  return -1;

 self->flags |= LIBROARRSOUND_FLAGS_CONNECTED;

 return 0;
}

/* Establishes connection to server. Might fail if connection can't be established or that one of
   the mandatory options isn't set in rsd_set_param(). This needs to be called after params have been set
   with rsd_set_param(), and before rsd_write(). */
int rsd_start (rsound_t *rd) {
 struct libroarrsound * self = (struct libroarrsound *)rd;
 int bits  = 16;
 int codec;

 if ( self == NULL )
  return -1;

 if ( self->flags & LIBROARRSOUND_FLAGS_STREAMING )
  return 0;

 if ( !(self->flags & LIBROARRSOUND_FLAGS_CONNECTED) ) {
  if ( libroarrsound_connect(self) == -1 )
   return -1;
 }

 switch (self->rsound.format) {
  case RSD_S16_LE:
    codec = ROAR_CODEC_PCM_S_LE;
   break;
  case RSD_S16_BE:
    codec = ROAR_CODEC_PCM_S_BE;
   break;
  case RSD_U16_LE:
    codec = ROAR_CODEC_PCM_U_LE;
   break;
  case RSD_U16_BE:
    codec = ROAR_CODEC_PCM_U_BE;
   break;
  case RSD_S8:
    codec = ROAR_CODEC_PCM_S;
    bits  = 8;
   break;
  case RSD_U8:
    codec = ROAR_CODEC_PCM_U;
    bits  = 8;
   break;
  default:
    return -1;
   break;
 }

 if ( roar_vio_simple_new_stream_obj(&(self->vio), &(self->con), &(self->stream),
                                     self->rsound.rate, self->rsound.channels, bits, codec, ROAR_DIR_PLAY) == -1 )
  return -1;

 self->flags |= LIBROARRSOUND_FLAGS_STREAMING;

 return 0;
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
size_t rsd_write (rsound_t *rd, const char* buf, size_t size) {
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
size_t rsd_delay (rsound_t *rd);
/* Will sleep until latency of stream reaches maximum allowed latency defined earlier by rsd_set_param - RSD_LATENCY
   Useful for hard headed blocking I/O design where user defined latency is needed. If rsd_set_param hasn't been set
   with RSD_LATENCY, this function will do nothing. */
void rsd_delay_wait(rsound_t *rd);


/* Pauses or unpauses a stream. pause -> enable = 1
   This function essentially calls on start() and stop(). This behavior might be changed later. */
int rsd_pause (rsound_t *rd, int enable);

//ll
