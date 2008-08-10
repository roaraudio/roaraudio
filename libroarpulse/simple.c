//simple.c:

#include <libroarpulse/libroarpulse.h>

/** Create a new connection to the server */
pa_simple* pa_simple_new(
    const char *server,                 /**< Server name, or NULL for default */
    const char *name,                   /**< A descriptive name for this client (application name, ...) */
    pa_stream_direction_t dir,          /**< Open this stream for recording or playback? */
    const char *dev,                    /**< Sink (resp. source) name, or NULL for default */
    const char *stream_name,            /**< A descriptive name for this client (application name, song title, ...) */
    const pa_sample_spec *ss,           /**< The sample type to use */
    const pa_channel_map *map,          /**< The channel map to use, or NULL for default */
    const pa_buffer_attr *attr,         /**< Buffering attributes, or NULL for default */
    int *error                          /**< A pointer where the error code is stored when the routine returns NULL. It is OK to pass NULL here. */
    ) {
 struct roarpulse_simple * s = malloc(sizeof(struct roarpulse_simple));
 int roar_dir;
 int codec = -1;
 struct roar_meta meta;

 if ( !s )
  return NULL;

 if ( dir == PA_STREAM_PLAYBACK ) {
  roar_dir = ROAR_DIR_PLAY;
 } else if ( dir == PA_STREAM_RECORD ) {
  roar_dir = ROAR_DIR_RECORD;
 } else {
  free(s);
  return NULL;
 }

 codec = roar_codec_pulse2roar(ss->format);

 if ( roar_simple_connect(&(s->con), (char*)server, (char*)name) == -1 ) {
  free(s);
  return NULL;
 }

 s->data_fh = roar_simple_new_stream_obj(&(s->con), &(s->stream), ss->rate, ss->channels,
                  16 /* does PulseAudio support something diffrent? */, codec, roar_dir);

 if ( s->data_fh == -1 ) {
  roar_disconnect(&(s->con));
  free(s);
  return NULL;
 }

 meta.value  = (char*)stream_name;
 meta.key[0] = 0;
 meta.type   = ROAR_META_TYPE_DESCRIPTION;

 roar_stream_meta_set(&(s->con), &(s->stream), ROAR_META_MODE_SET, &meta);

 return (pa_simple*) s;
}

/** Close and free the connection to the server. The connection objects becomes invalid when this is called. */
void pa_simple_free(pa_simple *s) {
 struct roarpulse_simple * ss = (struct roarpulse_simple*) s;
 if ( !s )
  return;

 close(ss->data_fh);
 roar_disconnect(&(ss->con));

 free(s);
}

/** Write some data to the server */
int pa_simple_write(pa_simple *s, const void*data, size_t length, int *error) {
 struct roarpulse_simple * ss = (struct roarpulse_simple*) s;
 if ( !s )
  return -1;

 return write(ss->data_fh, (char*) data, length);
}

/** Wait until all data already written is played by the daemon */
int pa_simple_drain(pa_simple *s, int *error) {
// struct roarpulse_simple * ss = (struct roarpulse_simple*) s;
 if ( !s )
  return -1;

 pa_simple_flush(s, NULL);

 return -1;
}

/** Read some data from the server */
int pa_simple_read(pa_simple *s, void*data, size_t length, int *error) {
 struct roarpulse_simple * ss = (struct roarpulse_simple*) s;
 if ( !s )
  return -1;

 return read(ss->data_fh, data, length);
}

/** Return the playback latency. \since 0.5 */
pa_usec_t pa_simple_get_latency(pa_simple *s, int *error) {
 struct roarpulse_simple * ss = (struct roarpulse_simple*) s;
 if ( !s )
  return -1;

 return -1;
}

/** Flush the playback buffer. \since 0.5 */
int pa_simple_flush(pa_simple *s, int *error) {
 struct roarpulse_simple * ss = (struct roarpulse_simple*) s;
 if ( !s )
  return -1;

 return fdatasync(ss->data_fh);
}

//ll
