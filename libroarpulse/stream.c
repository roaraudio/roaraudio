//stream.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libpulse*. They are mostly copyrighted by:
 *  Lennart Poettering <poettering@users.sourceforge.net> and
 *  Pierre Ossman <drzeus@drzeus.cx>
 *
 *  This file is part of libroarpulse a part of RoarAudio,
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
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>

struct _roar_pa_stream_cb {
 union {
  pa_stream_notify_cb_t  ncb;
  pa_stream_request_cb_t rcb;
  pa_stream_success_cb_t scb;
 } cb;
 void * userdata;
};

struct pa_stream {
 size_t                  refc;
 pa_context            * c;
 struct roar_vio_calls   vio;
 struct roar_stream      stream;
 pa_stream_state_t       state;
 pa_sample_spec          sspec;
 pa_io_event           * io_event;
 pa_timing_info          timinginfo;
 pa_buffer_attr          bufattr;
 pa_stream_direction_t   dir;
 struct roar_buffer    * iobuffer;
 struct {
  size_t size;
  size_t num;
 } fragments;
 struct {
  struct _roar_pa_stream_cb change_state;
  struct _roar_pa_stream_cb write;
  struct _roar_pa_stream_cb read;
  struct _roar_pa_stream_cb overflow;
  struct _roar_pa_stream_cb underflow;
  struct _roar_pa_stream_cb latency;
  struct _roar_pa_stream_cb drain;
 } cb;
 struct {
  pa_operation * drain;
 } op;
};

typedef void pa_proplist;
void pa_stream_set_state(pa_stream *s, pa_stream_state_t st);

pa_stream* pa_stream_new_with_proplist(
        pa_context *c                     ,
        const char *name                  ,
        const pa_sample_spec *ss          ,
        const pa_channel_map *map         ,
        pa_proplist *p                    );

/** Create a new, unconnected stream with the specified name and sample type */
pa_stream* pa_stream_new(
        pa_context *c                     /**< The context to create this stream in */,
        const char *name                  /**< A name for this stream */,
        const pa_sample_spec *ss          /**< The desired sample format */,
        const pa_channel_map *map         /**< The desired channel map, or NULL for default */) {
 return pa_stream_new_with_proplist(c, name, ss, map, NULL);
}

pa_stream* pa_stream_new_with_proplist(
        pa_context *c                     ,
        const char *name                  ,
        const pa_sample_spec *ss          ,
        const pa_channel_map *map         ,
        pa_proplist *p                    ) {
 pa_stream * s;

 ROAR_DBG("pa_stream_new_with_proplist(c=%p, name='%s', ss=%p, map=%p, p=%p) = ?", c, name, ss, map, p);

 if ( p != NULL )
  return NULL;

 if ( (s = roar_mm_malloc(sizeof(pa_stream))) == NULL )
  return NULL;

 memset(s, 0, sizeof(pa_stream));

 memcpy(&(s->sspec), ss, sizeof(pa_sample_spec));

 ROAR_DBG("pa_stream_new_with_proplist(c=%p, name='%s', ss=%p, map=%p, p=%p) = ?", c, name, ss, map, p);

 s->fragments.num  = 4;
 s->fragments.size = 2048;

 s->state = PA_STREAM_UNCONNECTED;
 s->c     = c;
 pa_context_ref(c);

 ROAR_DBG("pa_stream_new_with_proplist(c=%p, name='%s', ss=%p, map=%p, p=%p) = ?", c, name, ss, map, p);

 return s;
}

static void _pa_stream_free(pa_stream * s) {
 pa_stream_disconnect(s);
 pa_context_unref(s->c);
 roar_mm_free(s);
}

/** Decrease the reference counter by one */
void pa_stream_unref(pa_stream *s) {
 if ( s == NULL )
  return;

 s->refc--;

 if (s->refc < 1 )
  _pa_stream_free(s);
}

/** Increase the reference counter by one */
pa_stream *pa_stream_ref(pa_stream *s) {
 if ( s == NULL )
  return NULL;

 s->refc++;

 return s;
}

/** Return the current state of the stream */
pa_stream_state_t pa_stream_get_state(pa_stream *p) {
 if ( p == NULL )
  return PA_STREAM_FAILED;

 return p->state;
}

/** Return the context this stream is attached to */
pa_context* pa_stream_get_context(pa_stream *p) {
 if ( p == NULL )
  return NULL;

 return p->c;
}

/** Return the device (sink input or source output) index this stream is connected to */
uint32_t pa_stream_get_index(pa_stream *s) {
 return 0;
}

static void _roar_pa_stream_ioecb(pa_mainloop_api     * ea,
                                  pa_io_event         * e,
                                  int                   fd,
                                  pa_io_event_flags_t   events,
                                  void                * userdata) {
 pa_stream * s = userdata;
 void * data;
 size_t len;
 size_t ret;

 ROAR_DBG("_roar_pa_stream_ioecb(*) = ?");

 switch (s->dir) {
  case PA_STREAM_PLAYBACK:
    if ( s->iobuffer != NULL ) {
     if ( roar_buffer_get_data(s->iobuffer, &data) == -1 )
      return;

     if ( roar_buffer_get_len(s->iobuffer, &len) == -1 )
      return;

     if ( (ret = roar_vio_write(&(s->vio), data, len)) == -1 )
      return;

     ROAR_DBG("_roar_pa_stream_ioecb(*): vio write() = %lli", (long long int) ret);

     // TODO: handle errors
     if ( ret == len ) {
      roar_buffer_next(&(s->iobuffer));
     } else {
      roar_buffer_set_offset(s->iobuffer, ret);
     }
    }

    if ( s->iobuffer == NULL ) {
     ROAR_DBG("_roar_pa_stream_ioecb(*): disable IO events");
     ea->io_enable(e, PA_IO_EVENT_HANGUP|PA_IO_EVENT_ERROR);

     if ( s->cb.write.cb.rcb != NULL )
      s->cb.write.cb.rcb(s, pa_stream_writable_size(s), s->cb.write.userdata);

     if ( s->cb.drain.cb.scb != NULL )
      s->cb.drain.cb.scb(s, 1, s->cb.drain.userdata);
    }
   break;
  case PA_STREAM_RECORD:
   break;
  default:
   return;
 }

 ROAR_DBG("_roar_pa_stream_ioecb(*) = (void)");
}

static int _roar_pa_stream_open (pa_stream *s,
                                 const char *dev,
                                 const pa_buffer_attr *attr,
                                 pa_stream_flags_t flags,
                                 pa_cvolume *volume,
                                 pa_stream *sync_stream,
                                 pa_stream_direction_t dir) {
 struct roar_connection * con;
 pa_mainloop_api * api;
 pa_io_event_flags_t event_flags = PA_IO_EVENT_HANGUP|PA_IO_EVENT_ERROR;
 int fh;
 int ctl = -1;

 ROAR_DBG("_roar_pa_stream_open(s=%p, dev='%s', attr=%p, flags=%i, volume=%p, sync_stream=%p, dir=%i) = ?", s, dev, attr, flags, volume, sync_stream, dir);

 if ( s == NULL )
  return -1;

 volume = NULL;

 if ( attr != NULL || flags != 0 || volume != NULL || sync_stream != NULL ) {
  pa_stream_set_state(s, PA_STREAM_FAILED);
  return -1;
 }

 if ( (con = roar_pa_context_get_con(s->c)) == NULL ) {
  pa_stream_set_state(s, PA_STREAM_FAILED);
  return -1;
 }

 s->dir = dir;

 switch (dir) {
  case PA_STREAM_PLAYBACK:
    s->stream.dir = ROAR_DIR_PLAY;
    ctl           = ROAR_VIO_CTL_GET_SELECT_WRITE_FH;
    event_flags  |= PA_IO_EVENT_OUTPUT;
   break;
  case PA_STREAM_RECORD:
    s->stream.dir = ROAR_DIR_MONITOR;
    ctl           = ROAR_VIO_CTL_GET_SELECT_READ_FH;
    event_flags  |= PA_IO_EVENT_INPUT;
   break;
  default:
    pa_stream_set_state(s, PA_STREAM_FAILED);
    return -1;
   break;
 }

 if ( roar_pa_sspec2auinfo(&(s->stream.info), &(s->sspec)) == -1 ) {
  pa_stream_set_state(s, PA_STREAM_FAILED);
  return -1;
 }

 if ( roar_vio_simple_new_stream_obj(&(s->vio), con, &(s->stream),
                                     s->stream.info.rate, s->stream.info.channels,
                                     s->stream.info.bits, s->stream.info.codec,
                                     s->stream.dir) == -1 ) {
  pa_stream_set_state(s, PA_STREAM_FAILED);
  return -1;
 }

 api = roar_pa_context_get_api(s->c);

 if ( api != NULL && api->io_new != NULL ) {
  if ( roar_vio_ctl(&(s->vio), ctl, &fh) != -1 ) {
   s->io_event = api->io_new(api, fh, event_flags, _roar_pa_stream_ioecb, s);
  }
 }

 // TODO: update s->fragments.

 s->bufattr.maxlength = s->fragments.size * s->fragments.num;
 s->bufattr.tlength   = s->fragments.size;
 s->bufattr.prebuf    = 0;
 s->bufattr.minreq    = 1;
 s->bufattr.fragsize  = s->fragments.size;

 pa_stream_set_state(s, PA_STREAM_READY);

 return 0;
}

/** Connect the stream to a sink */
int pa_stream_connect_playback(
        pa_stream *s                  /**< The stream to connect to a sink */,
        const char *dev               /**< Name of the sink to connect to, or NULL for default */ ,
        const pa_buffer_attr *attr    /**< Buffering attributes, or NULL for default */,
        pa_stream_flags_t flags       /**< Additional flags, or 0 for default */,
        pa_cvolume *volume            /**< Initial volume, or NULL for default */,
        pa_stream *sync_stream        /**< Synchronize this stream with the specified one, or NULL for a standalone stream*/) {
 return _roar_pa_stream_open(s, dev, attr, flags, volume, sync_stream, PA_STREAM_PLAYBACK);
}

/** Connect the stream to a source */
int pa_stream_connect_record(
        pa_stream *s                  /**< The stream to connect to a source */ ,
        const char *dev               /**< Name of the source to connect to, or NULL for default */,
        const pa_buffer_attr *attr    /**< Buffer attributes, or NULL for default */,
        pa_stream_flags_t flags       /**< Additional flags, or 0 for default */) {
 return _roar_pa_stream_open(s, dev, attr, flags, NULL, NULL, PA_STREAM_RECORD);
}

/** Disconnect a stream from a source/sink */
int pa_stream_disconnect(pa_stream *s) {
 pa_mainloop_api * api;

 if ( s == NULL )
  return -1;

 if ( s->state != PA_STREAM_READY )
  return -1;

 if ( s->io_event != NULL ) {
  api = roar_pa_context_get_api(s->c);

  if ( api != NULL && api->io_free != NULL ) {
   api->io_free(s->io_event);
   s->io_event = NULL;
  }
 }

 roar_vio_close(&(s->vio));

 pa_stream_set_state(s, PA_STREAM_TERMINATED);

 return 0;
}

/** Write some data to the server (for playback sinks), if free_cb is
 * non-NULL this routine is called when all data has been written out
 * and an internal reference to the specified data is kept, the data
 * is not copied. If NULL, the data is copied into an internal
 * buffer. The client my freely seek around in the output buffer. For
 * most applications passing 0 and PA_SEEK_RELATIVE as arguments for
 * offset and seek should be useful.*/
int pa_stream_write(
        pa_stream *p             /**< The stream to use */,
        const void *data         /**< The data to write */,
        size_t length            /**< The length of the data to write */,
        pa_free_cb_t free_cb     /**< A cleanup routine for the data or NULL to request an internal copy */,
        int64_t offset,          /**< Offset for seeking, must be 0 for upload streams */
        pa_seek_mode_t seek      /**< Seek mode, must be PA_SEEK_RELATIVE for upload streams */) {
 pa_mainloop_api    * api;
 struct roar_buffer * buf;
 void               * bufdata;

 ROAR_DBG("pa_stream_write(p=%p, data=%p, length=%llu, free_cb=%p, offset=%lli, seek=%i) = ?", p, data, (long long unsigned int) length, free_cb, offset, seek);

 // TODO: implement seeking in output buffer

 if ( p == NULL )
  return -1;

 if ( offset != 0 || seek != PA_SEEK_RELATIVE )
  return -1;

 if ( data == NULL ) {
  if ( length == 0 ) {
   if ( free_cb != NULL )
    free_cb(NULL);

   return 0;
  } else {
   return -1;
  }
 }

 // seems we have a valid write from here.

 if ( roar_buffer_new(&buf, length) == -1 ) {
  if ( free_cb != NULL )
   free_cb((void*)data);

  return -1;
 }

 if ( roar_buffer_get_data(buf, &bufdata) == -1 ) {
  if ( free_cb != NULL )
   free_cb((void*)data);

  return -1;
 }

 memcpy(bufdata, data, length);
 if ( free_cb != NULL )
  free_cb((void*)data);

 if ( p->iobuffer == NULL ) {
  p->iobuffer = buf;
 } else {
  if ( roar_buffer_add(p->iobuffer, buf) == -1 )
   return -1;
 }

 if ( p->io_event != NULL ) {
  api = roar_pa_context_get_api(p->c);
  if ( api != NULL ) {
   ROAR_DBG("pa_stream_write(*): enable IO events");
   api->io_enable(p->io_event, PA_IO_EVENT_OUTPUT|PA_IO_EVENT_HANGUP|PA_IO_EVENT_ERROR);
  }
 }

 ROAR_DBG("pa_stream_write(p=%p, data=%p, length=%llu, free_cb=%p, offset=%lli, seek=%i) = 0", p, data, (long long unsigned int) length, free_cb, offset, seek);
 return 0;
}

/** Read the next fragment from the buffer (for recording).
 * data will point to the actual data and length will contain the size
 * of the data in bytes (which can be less than a complete framgnet).
 * Use pa_stream_drop() to actually remove the data from the
 * buffer. If no data is available will return a NULL pointer  \since 0.8 */
int pa_stream_peek(
        pa_stream *p                 /**< The stream to use */,
        const void **data            /**< Pointer to pointer that will point to data */,
        size_t *length              /**< The length of the data read */) {
 if ( data == NULL || length == NULL )
  return -1;

 *data   = NULL;
 *length = 0;

 if ( p == NULL )
  return -1;

 if ( p->iobuffer == NULL )
  return 0;

 if ( roar_buffer_get_len(p->iobuffer, length) == -1 ) {
  *length = 0;
  return -1;
 }

 if ( roar_buffer_get_data(p->iobuffer, (void**)data) == -1 ) {
  *length = 0;
  *data   = NULL;
  return -1;
 }

 return 0;
}

/** Remove the current fragment on record streams. It is invalid to do this without first
 * calling pa_stream_peek(). \since 0.8 */
int pa_stream_drop(pa_stream *p) {
 if ( p == NULL )
  return -1;

 if ( p->iobuffer == NULL )
  return -1;

 return roar_buffer_next(&(p->iobuffer));
}

/** Return the nember of bytes that may be written using pa_stream_write() */
size_t pa_stream_writable_size(pa_stream *p) {
 struct roar_buffer_stats stats;

 ROAR_DBG("pa_stream_writable_size(p=%p) = ?", p);

 if ( p == NULL ) {
  ROAR_DBG("pa_stream_writable_size(p=%p) = 0", p);
  return 0;
 }

 if ( p->iobuffer == NULL ) {
  ROAR_DBG("pa_stream_writable_size(p=%p) = %llu", p, (long long unsigned)(p->fragments.num*p->fragments.size));
  return p->fragments.num * p->fragments.size / 2;
 }

 if ( roar_buffer_ring_stats(p->iobuffer, &stats) == -1 ) {
  ROAR_DBG("pa_stream_writable_size(p=%p) = 0", p);
  return 0;
 }

 ROAR_DBG("pa_stream_writable_size(p=%p): stats={.parts=%i, .bytes=%i, ...}", p, stats.parts, stats.bytes);

 if ( stats.parts > p->fragments.num ) {
  ROAR_DBG("pa_stream_writable_size(p=%p) = 0", p);
  return 0;
 }

 if ( stats.parts > (p->fragments.num/2) )
  stats.parts = p->fragments.num / 2;

 ROAR_DBG("pa_stream_writable_size(p=%p) = %llu", p, (long long unsigned)((p->fragments.num - stats.parts)*p->fragments.size));
 return (p->fragments.num - stats.parts)*p->fragments.size;
}

/** Return the number of bytes that may be read using pa_stream_read() \since 0.8 */
size_t pa_stream_readable_size(pa_stream *p) {
 struct roar_buffer_stats stats;

 if ( p == NULL )
  return 0;

 if ( p->iobuffer == NULL )
  return 0;

 if ( roar_buffer_ring_stats(p->iobuffer, &stats) == -1 )
  return 0;

 return stats.bytes;
}

/** Drain a playback stream. Use this for notification when the buffer is empty */
pa_operation* pa_stream_drain(pa_stream *s, pa_stream_success_cb_t cb, void *userdata) {
 if ( s == NULL )
  return NULL;

 s->cb.drain.cb.scb   = cb;
 s->cb.drain.userdata = userdata;

 if ( s->op.drain == NULL ) {
  s->op.drain = roar_pa_operation_new(PA_OPERATION_RUNNING);
 }

 pa_operation_ref(s->op.drain);

 return s->op.drain;
}

/** Request a timing info structure update for a stream. Use
 * pa_stream_get_timing_info() to get access to the raw timing data,
 * or pa_stream_get_time() or pa_stream_get_latency() to get cleaned
 * up values. */
pa_operation* pa_stream_update_timing_info(pa_stream *p, pa_stream_success_cb_t cb, void *userdata) {
 int suc = 1;

 if ( p == NULL )
  return NULL;

 if ( roar_get_stream(roar_pa_context_get_con(p->c), &(p->stream), p->stream.id) == -1 ) {
  suc = 0;
 }

 // p->timinginfo
 pa_gettimeofday(&(p->timinginfo.timestamp)); // we should interpolate between time before call and after

 p->timinginfo.synchronized_clocks    = 0;
 p->timinginfo.sink_usec              = 0;
 p->timinginfo.source_usec            = 0;
 p->timinginfo.transport_usec         = 0;
 p->timinginfo.playing                = p->iobuffer != NULL;
 p->timinginfo.write_index_corrupt    = 1;
 p->timinginfo.write_index            = p->stream.pos * pa_frame_size(&(p->sspec));
 p->timinginfo.read_index_corrupt     = 1;
 p->timinginfo.read_index             = p->stream.pos * pa_frame_size(&(p->sspec));
#if 0 /* newer versions */
 p->timinginfo.configured_sink_usec   = p->timinginfo.sink_usec;
 p->timinginfo.configured_source_usec = p->timinginfo.source_usec;
 p->timinginfo.since_underrun         = 0;
#endif

 if ( cb != NULL ) {
  cb(p, suc, userdata);
 }

 return roar_pa_op_new_done();
}

/** Set the callback function that is called whenever the state of the stream changes */
void pa_stream_set_state_callback(pa_stream *s, pa_stream_notify_cb_t cb, void *userdata) {
 ROAR_DBG("pa_stream_set_state_callback(s=%p, cb=%p, userdata=%p) = ?", s, cb, userdata);

 if ( s == NULL )
  return;

 s->cb.change_state.cb.ncb    = cb;
 s->cb.change_state.userdata  = userdata;
}

void pa_stream_set_state(pa_stream *s, pa_stream_state_t st) {
 if ( s == NULL )
  return;

 ROAR_DBG("pa_stream_set_state(s=%p, st=%i): State: %i->%i", s, st, s->state, st);

 s->state = st;

 if ( s->cb.change_state.cb.ncb != NULL ) {
  ROAR_DBG("pa_stream_set_state(s=%p, st=%i): calling callback at %p", s, st, s->cb.change_state.cb.ncb);
  s->cb.change_state.cb.ncb(s, s->cb.change_state.userdata);
 }
 ROAR_DBG("pa_stream_set_state(s=%p, st=%i) = (void)", s, st);
}

/** Set the callback function that is called when new data may be
 * written to the stream. */
void pa_stream_set_write_callback(pa_stream *p, pa_stream_request_cb_t cb, void *userdata) {
 if ( p == NULL )
  return;

 p->cb.write.cb.rcb    = cb;
 p->cb.write.userdata  = userdata;
}

/** Set the callback function that is called when new data is available from the stream.
 * Return the number of bytes read. \since 0.8 */
void pa_stream_set_read_callback(pa_stream *p, pa_stream_request_cb_t cb, void *userdata) {
 if ( p == NULL )
  return;

 p->cb.read.cb.rcb    = cb;
 p->cb.read.userdata  = userdata;
}

/** Set the callback function that is called when a buffer overflow happens. (Only for playback streams) \since 0.8 */
void pa_stream_set_overflow_callback(pa_stream *p, pa_stream_notify_cb_t cb, void *userdata) {
 if ( p == NULL )
  return;

 p->cb.overflow.cb.ncb    = cb;
 p->cb.overflow.userdata  = userdata;
}

/** Set the callback function that is called when a buffer underflow happens. (Only for playback streams) \since 0.8 */
void pa_stream_set_underflow_callback(pa_stream *p, pa_stream_notify_cb_t cb, void *userdata) {
 if ( p == NULL )
  return;

 p->cb.underflow.cb.ncb    = cb;
 p->cb.underflow.userdata  = userdata;
}

/** Set the callback function that is called whenever a latency information update happens. Useful on PA_STREAM_AUTO_TIMING_UPDATE streams only. (Only for playback streams) \since 0.8.2 */
void pa_stream_set_latency_update_callback(pa_stream *p, pa_stream_notify_cb_t cb, void *userdata) {
 if ( p == NULL )
  return;

 p->cb.latency.cb.ncb    = cb;
 p->cb.latency.userdata  = userdata;
}

/** Pause (or resume) playback of this stream temporarily. Available on both playback and recording streams. \since 0.3 */
pa_operation* pa_stream_cork(pa_stream *s, int b, pa_stream_success_cb_t cb, void *userdata);

/** Flush the playback buffer of this stream. Most of the time you're
 * better off using the parameter delta of pa_stream_write() instead of this
 * function. Available on both playback and recording streams. \since 0.3 */
pa_operation* pa_stream_flush(pa_stream *s, pa_stream_success_cb_t cb, void *userdata) {
 return pa_stream_drain(s, cb, userdata); // where is the differance to drain?
}
/** Reenable prebuffering as specified in the pa_buffer_attr
 * structure. Available for playback streams only. \since 0.6 */
pa_operation* pa_stream_prebuf(pa_stream *s, pa_stream_success_cb_t cb, void *userdata);

/** Request immediate start of playback on this stream. This disables
 * prebuffering as specified in the pa_buffer_attr
 * structure, temporarily. Available for playback streams only. \since 0.3 */
pa_operation* pa_stream_trigger(pa_stream *s, pa_stream_success_cb_t cb, void *userdata);

/** Rename the stream. \since 0.5 */
pa_operation* pa_stream_set_name(pa_stream *s, const char *name, pa_stream_success_cb_t cb, void *userdata);

/** Return the current playback/recording time. This is based on the
 * data in the timing info structure returned by
 * pa_stream_get_timing_info(). This function will usually only return
 * new data if a timing info update has been recieved. Only if timing
 * interpolation has been requested (PA_STREAM_INTERPOLATE_TIMING)
 * the data from the last timing update is used for an estimation of
 * the current playback/recording time based on the local time that
 * passed since the timing info structure has been acquired. The time
 * value returned by this function is guaranteed to increase
 * monotonically. (that means: the returned value is always greater or
 * equal to the value returned on the last call) This behaviour can
 * be disabled by using PA_STREAM_NOT_MONOTONOUS. This may be
 * desirable to deal better with bad estimations of transport
 * latencies, but may have strange effects if the application is not
 * able to deal with time going 'backwards'. \since 0.6 */
int pa_stream_get_time(pa_stream *s, pa_usec_t *r_usec) {
 if ( s == NULL || r_usec == NULL )
  return -1;

 *r_usec = s->stream.pos * 1000000 / s->stream.info.rate / s->stream.info.channels;

 return 0;
}

/** Return the total stream latency. This function is based on
 * pa_stream_get_time(). In case the stream is a monitoring stream the
 * result can be negative, i.e. the captured samples are not yet
 * played. In this case *negative is set to 1. \since 0.6 */
int pa_stream_get_latency(pa_stream *s, pa_usec_t *r_usec, int *negative) {
 // TODO: Fix this:
 // sinks: lateny of stream, mixer, output...
 // sources: mixer, output, negative

 if ( r_usec != NULL )
  *r_usec = 0;

 if ( negative != NULL )
  *negative = 0;

 return 0;
}

/** Return the latest raw timing data structure. The returned pointer
 * points to an internal read-only instance of the timing
 * structure. The user should make a copy of this structure if he
 * wants to modify it. An in-place update to this data structure may
 * be requested using pa_stream_update_timing_info(). If no
 * pa_stream_update_timing_info() call was issued before, this
 * function will fail with PA_ERR_NODATA. Please note that the
 * write_index member field (and only this field) is updated on each
 * pa_stream_write() call, not just when a timing update has been
 * recieved. \since 0.8 */
const pa_timing_info* pa_stream_get_timing_info(pa_stream *s) {
 if ( s == NULL )
  return NULL;

 return &(s->timinginfo);
}

/** Return a pointer to the stream's sample specification. \since 0.6 */
const pa_sample_spec* pa_stream_get_sample_spec(pa_stream *s) {
 if ( s == NULL )
  return NULL;

 return &(s->sspec);
}

/** Return a pointer to the stream's channel map. \since 0.8 */
const pa_channel_map* pa_stream_get_channel_map(pa_stream *s);

/** Return the buffer metrics of the stream. Only valid after the
 * stream has been connected successfuly and if the server is at least
 * PulseAudio 0.9. \since 0.9.0 */
const pa_buffer_attr* pa_stream_get_buffer_attr(pa_stream *s) {
 if ( s == NULL )
  return NULL;

 return &(s->bufattr);
}

//ll
