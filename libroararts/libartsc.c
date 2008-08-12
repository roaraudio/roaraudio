//libartsc.c:

#include <roaraudio.h>
#include <kde/artsc/artsc.h>

struct roar_connection _libroarartsc_connection[1];

struct _libroarartsc_stream {
 struct roar_stream stream;
 int fh;
 int blocking;
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
 struct _libroarartsc_stream * s = malloc(sizeof(struct _libroarartsc_stream));
 struct roar_meta meta;

 if ( !s )
  return NULL;

 if ( (s->fh = roar_simple_new_stream_obj(_libroarartsc_connection, &(s->stream),
                                 rate, channels, bits, ROAR_CODEC_DEFAULT, ROAR_DIR_PLAY)) == -1 ) {
  free(s);
  return NULL;
 }

 s->blocking = 1;

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
 struct _libroarartsc_stream * s = malloc(sizeof(struct _libroarartsc_stream));
 struct roar_meta meta;

 if ( !s )
  return NULL;

 if ( (s->fh = roar_simple_new_stream_obj(_libroarartsc_connection, &(s->stream),
                                 rate, channels, bits, ROAR_CODEC_DEFAULT, ROAR_DIR_RECORD)) == -1 ) {
  free(s);
  return NULL;
 }

 s->blocking = 1;

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

 free(stream);
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

 return -1;
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
 struct roar_stream_info info;

 if ( !stream )
  return -1;

 if ( param == ARTS_P_PACKET_SIZE ) {
  if ( roar_stream_get_info(_libroarartsc_connection, &(s->stream), &info) != -1 ) {
   return info.block_size;
  }
  return -1;
 } else if ( param == ARTS_P_PACKET_COUNT ) {
  return 1;
 } else if ( param == ARTS_P_BLOCKING ) {
  return s->blocking;
 }

 return -1;
}

//ll
