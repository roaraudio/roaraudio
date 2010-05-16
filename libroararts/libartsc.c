//libartsc.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libarts*. They are mostly copyrighted by:
 *  Stefan Westerfeld <stefan@space.twc.de>
 *
 *  This file is part of libroararts a part of RoarAudio,
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <roaraudio.h>
#include <kde/artsc/artsc.h>

static struct roar_connection _libroarartsc_connection[1];

struct _libroarartsc_stream {
 struct roar_stream stream;
 int fh;
 int blocking;
 int block_size;
 int dir;
};

int arts_init(void) {
 return roar_simple_connect(_libroarartsc_connection, NULL, "libroarartsc client");
}

void arts_free(void) {
 roar_disconnect(_libroarartsc_connection);
}

/**
 * asks aRtsd to free the DSP device and return 1 if it was successful,
 * 0 if there were active non-suspendable modules
 */
int arts_suspend(void) {
 return roar_set_standby(_libroarartsc_connection, ROAR_STANDBY_ACTIVE) == 0 ? 1 : 0;
}

/**
 * asks aRtsd if the DSP device is free and return 1 if it is,
 * 0 if not
 */
int arts_suspended(void) {
 return roar_get_standby(_libroarartsc_connection) == ROAR_STANDBY_ACTIVE;
}


/**
 * converts an error code to a human readable error message
 *
 * @param errorcode the errorcode (from another arts function that failed)
 * @returns a text string with the error message
 */
const char *arts_error_text(int errorcode) {
 return strerror(errorcode);
}

/**
 * open a stream for playing
 *
 * @param rate the sampling rate (something like 44100)
 * @param bits how many bits each sample has (8 or 16)
 * @param channels how many channels, 1 is mono, 2 is stereo
 * @param name the name of the stream (these will be used so that the user can
 *          assign streams to effects/mixer channels and similar)
 *
 * @return a stream
 */
arts_stream_t arts_play_stream(int rate, int bits, int channels, const char *name) {
 struct _libroarartsc_stream * s = roar_mm_malloc(sizeof(struct _libroarartsc_stream));
 struct roar_meta meta;
 struct roar_stream_info info;

 if ( !s )
  return NULL;

 if ( (s->fh = roar_simple_new_stream_obj(_libroarartsc_connection, &(s->stream),
                                 rate, channels, bits, ROAR_CODEC_DEFAULT, ROAR_DIR_PLAY)) == -1 ) {
  roar_mm_free(s);
  return NULL;
 }

 s->dir = ROAR_DIR_PLAY;

 s->blocking = 1;

 if ( roar_stream_get_info(_libroarartsc_connection, &(s->stream), &info) != -1 ) {
  s->block_size = info.block_size;
 } else {
  close(s->fh);
  roar_mm_free(s);
  return NULL;
 }

 if ( name && *name ) {
  meta.value  = (char*)name;
  meta.key[0] = 0;
  meta.type   = ROAR_META_TYPE_DESCRIPTION;

  roar_stream_meta_set(_libroarartsc_connection, &(s->stream), ROAR_META_MODE_SET, &meta);
 }

 return (arts_stream_t) s;
}

/**
 * open a stream for recording
 *
 * @param rate the sampling rate (something like 44100)
 * @param bits how many bits each sample has (8 or 16)
 * @param channels how many channels, 1 is mono, 2 is stereo
 * @param name the name of the stream (these will be used so that the user can
 *          assign streams to effects/mixer channels and similar)
 *
 * @return a stream
 */
arts_stream_t arts_record_stream(int rate, int bits, int channels, const char *name) {
 struct _libroarartsc_stream * s = roar_mm_malloc(sizeof(struct _libroarartsc_stream));
 struct roar_meta meta;
 struct roar_stream_info info;

 if ( !s )
  return NULL;

 if ( (s->fh = roar_simple_new_stream_obj(_libroarartsc_connection, &(s->stream),
                                 rate, channels, bits, ROAR_CODEC_DEFAULT, ROAR_DIR_RECORD)) == -1 ) {
  roar_mm_free(s);
  return NULL;
 }

 s->dir = ROAR_DIR_RECORD;

 s->blocking = 1;

 if ( roar_stream_get_info(_libroarartsc_connection, &(s->stream), &info) != -1 ) {
  s->block_size = info.block_size;
 } else {
  close(s->fh);
  roar_mm_free(s);
  return NULL;
 }

 if ( name && *name ) {
  meta.value  = (char*)name;
  meta.key[0] = 0;
  meta.type   = ROAR_META_TYPE_DESCRIPTION;

  roar_stream_meta_set(_libroarartsc_connection, &(s->stream), ROAR_META_MODE_SET, &meta);
 }

 return (arts_stream_t) s;
}

/**
 * close a stream
 */
void arts_close_stream(arts_stream_t stream) {
 struct _libroarartsc_stream * s = (struct _libroarartsc_stream *) stream;
 if ( !stream )
  return;

 close(s->fh);

 roar_mm_free(stream);
}

/**
 * read samples from stream
 *
 * @param stream a previously opened record stream
 * @param buffer a buffer with sample data
 * @param count the number of bytes contained in the buffer
 *
 * @returns number of read bytes on success or error code
 */
int arts_read(arts_stream_t stream, void *buffer, int count) {
 struct _libroarartsc_stream * s = (struct _libroarartsc_stream *) stream;
 if ( !stream )
  return -1;

 return read(s->fh, buffer, count);
}

/**
 * write samples to to stream
 *
 * @param stream a previously opened play stream
 * @param buffer a buffer with sample data
 * @param count the number of bytes contained in the buffer
 *
 * @returns number of written bytes on success or error code
 */
int arts_write(arts_stream_t stream, const void *buffer, int count) {
 struct _libroarartsc_stream * s = (struct _libroarartsc_stream *) stream;
 if ( !stream )
  return -1;

 return write(s->fh, buffer, count);
}

/**
 * configure a parameter of a stream
 *
 * @param stream an opened record or play stream
 * @param parameter the parameter you want to modify
 * @param value the new value
 *
 * @returns the new value of the parameter (which may or may not be the value
 *          you wanted to have), or an error code if something went wrong
 */
int arts_stream_set(arts_stream_t stream, arts_parameter_t param, int value) {
 struct _libroarartsc_stream * s = (struct _libroarartsc_stream *) stream;
 if ( !stream )
  return -1;

 if ( param == ARTS_P_BLOCKING ) {
  if ( roar_socket_nonblock(s->fh, value ? ROAR_SOCKET_BLOCK : ROAR_SOCKET_NONBLOCK) == -1 )
   return -1;
  return arts_stream_get(stream, param);
 }

 return arts_stream_get(stream, param);
}

/**
 * query a parameter of a stream
 *
 * @param stream an opened record or play stream
 * @param parameter the parameter you want to query
 *
 * @returns the value of the parameter, or an error code
 */
int arts_stream_get(arts_stream_t stream, arts_parameter_t param) {
 struct _libroarartsc_stream * s = (struct _libroarartsc_stream *) stream;
 fd_set sl;
 struct timeval tv;

 if ( !stream )
  return -1;

 if ( param == ARTS_P_PACKET_SIZE ) {
   return s->block_size;
 } else if ( param == ARTS_P_BUFFER_SPACE ) {
  FD_ZERO(&sl);
  FD_SET(s->fh, &sl);

  tv.tv_sec  = 0;
  tv.tv_usec = 1;

  if ( s->dir == ROAR_DIR_PLAY ) {
   if (select(s->fh + 1, NULL, &sl, NULL, &tv) > 0)
    return s->block_size;
  } else {
   if (select(s->fh + 1, &sl, NULL, NULL, &tv) > 0)
    return s->block_size;
  }

  return 0;
 } else if ( param == ARTS_P_BUFFER_SIZE ) {
   return s->block_size*2;
 } else if ( param == ARTS_P_PACKET_COUNT ) {
  return 1;
 } else if ( param == ARTS_P_BLOCKING ) {
  return s->blocking;
 }

 return -1;
}

//ll
