//streams.c:

#include "roard.h"

int streams_init (void) {
 int i;

 for (i = 0; i < ROAR_STREAMS_MAX; i++)
  g_streams[i] = NULL;

 return 0;
}

int streams_free (void) {
 int i;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   streams_delete(i);
  }
 }

 return 0;
}


int streams_new    (void) {
 int i, j;
 struct roar_stream * n = NULL;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] == NULL ) {
   n = malloc(sizeof(struct roar_stream_server));
   if ( n == NULL ) {
    ROAR_ERR("streams_new(void): can not allocate memory for new stream: %s", strerror(errno));
    ROAR_DBG("streams_new(void) = -1");
    return -1;
   }

   n->id         = i;
   n->fh         = -1;
//   n->pos_rel_id = i;
   n->database   = NULL;
   n->dataoff    = NULL;
   n->datalen    = 0;
   n->offset     = 0;

   ((struct roar_stream_server*)n)->client      = -1;
   ((struct roar_stream_server*)n)->buffer      = NULL;
   ((struct roar_stream_server*)n)->need_extra  =  0;
   ((struct roar_stream_server*)n)->output      = NULL;
   ((struct roar_stream_server*)n)->is_new      =  1;
   ((struct roar_stream_server*)n)->codecfilter = -1;
   ((struct roar_stream_server*)n)->mixer.scale = 65535;
   for (j = 0; j < ROAR_MAX_CHANNELS; j++)
    ((struct roar_stream_server*)n)->mixer.mixer[j] = 65535;
   for (j = 0; j < ROAR_META_MAX_PER_STREAM; j++) {
    ((struct roar_stream_server*)n)->meta[j].type   = ROAR_META_TYPE_NONE;
    ((struct roar_stream_server*)n)->meta[j].key[0] = 0;
    ((struct roar_stream_server*)n)->meta[j].value  = NULL;
   }

   g_streams[i] = (struct roar_stream_server*)n;
   ROAR_DBG("streams_new(void) = %i", i);
   return i;
  }
 }

 return -1;
}

int streams_delete (int id) {
 if ( g_streams[id] == NULL )
  return 0;

 ROAR_DBG("streams_delete(id=%i) = ?", id);

 if ( g_streams[id]->codecfilter != -1 ) {
  codecfilter_close(g_streams[id]->codecfilter_inst, g_streams[id]->codecfilter);
  g_streams[id]->codecfilter = -1;
 }

 if ( g_streams[id]->client != -1 ) {
  ROAR_DBG("streams_delete(id=%i): Stream is owned by client %i", id, g_streams[id]->client);
  client_stream_delete(g_streams[id]->client, id);
 }

 if ( g_streams[id]->buffer != NULL )
  roar_buffer_free(g_streams[id]->buffer);

 if ( g_streams[id]->output != NULL )
  free(g_streams[id]->output);

 if ( ROAR_STREAM(g_streams[id])->fh != -1 )
  close(ROAR_STREAM(g_streams[id])->fh);

 free(g_streams[id]);

 g_streams[id] = NULL;

 ROAR_DBG("streams_delete(id=%i) = 0", id);
 return 0;
}

int streams_set_client (int id, int client) {
 if ( g_streams[id] == NULL )
  return -1;

 g_streams[id]->client = client;

 return 0;
}

int streams_set_fh     (int id, int fh) {
 int dir;

 if ( g_streams[id] == NULL )
  return -1;

 ((struct roar_stream *)g_streams[id])->fh = fh;

 codecfilter_open(&(g_streams[id]->codecfilter_inst), &(g_streams[id]->codecfilter), NULL,
                  ((struct roar_stream *)g_streams[id])->info.codec, g_streams[id]);

 dir = ((struct roar_stream *)g_streams[id])->dir;

 if ( dir == ROAR_DIR_MONITOR || dir == ROAR_DIR_RECORD ) {
  shutdown(fh, SHUT_RD);
 }

 if ( dir == ROAR_DIR_FILTER ) {
  return 0;
 } else {
  return roar_socket_nonblock(fh, ROAR_SOCKET_NONBLOCK);
 }
}

int streams_get_fh     (int id) {
 if ( id < 0 )
  return -1;

 if ( g_streams[id] == NULL )
  return -1;

 return ((struct roar_stream *)g_streams[id])->fh;
}

int streams_get    (int id, struct roar_stream_server ** stream) {
 if ( g_streams[id] == NULL )
  return -1;

 *stream = g_streams[id];

 return 0;
}


int streams_get_outputbuffer  (int id, void ** buffer, size_t size) {
 if ( g_streams[id] == NULL )
  return -1;

 // output buffer size does never change.
 if ( g_streams[id]->output != NULL ) {
  *buffer = g_streams[id]->output;
  return 0;
 }

 if ( (g_streams[id]->output = malloc(size)) == NULL ) {
  ROAR_ERR("streams_get_outputbuffer(*): Can not alloc: %s", strerror(errno));
  return -1;
 }

 *buffer = g_streams[id]->output;

 return 0;
}

int streams_fill_mixbuffer (int id, struct roar_audio_info * info) {
 // TODO: decide: is this the most complex, hacked, un-understadable,
 //               un-writeable and even worse: un-readable
 //               function in the whole project?
 size_t todo = ROAR_OUTPUT_CALC_OUTBUFSIZE(info);
 size_t todo_in;
 size_t len, outlen;
 size_t mul = 1, div = 1;
 void * rest = NULL;
 void * in   = NULL;
 struct roar_buffer     * buf;
 struct roar_audio_info * stream_info;
 int is_the_same = 0;

 if ( g_streams[id] == NULL )
  return -1;

 if ( streams_get_outputbuffer(id, &rest, todo) == -1 ) {
  return -1;
 }

 if ( rest == NULL ) {
  return -1;
 }

 // set up stream_info

 stream_info = &(((struct roar_stream*)g_streams[id])->info);

 // calc todo_in
 todo_in = ROAR_OUTPUT_CALC_OUTBUFSIZE(stream_info);

 // calc mul and div:
 mul = todo    / todo_in;
 div = todo_in / todo;

 if ( mul == 0 ) {
  mul = 1;
 } else {
  div = 1;
 }

 ROAR_DBG("streams_fill_mixbuffer(*): mul=%i, div=%i", mul, div);

 ROAR_DBG("streams_fill_mixbuffer(*): rest=%p, todo=%i->%i (in->out)", rest, todo_in, todo);
 // are both (input and output) of same format?


 ROAR_DBG("streams_fill_mixbuffer(*): stream_info:");
 roar_debug_audio_info_print(stream_info);
 ROAR_DBG("streams_fill_mixbuffer(*): info:");
 roar_debug_audio_info_print(info);

 is_the_same = stream_info->rate     == info->rate     && stream_info->bits  == info->bits &&
               stream_info->channels == info->channels && stream_info->codec == info->codec;

 ROAR_DBG("streams_fill_mixbuffer(*): is_the_same=%i", is_the_same);

/* How it works:
 *
 * set a counter to the number of samples we need.
 * loop until we have all samples done or no samples are
 * left in our input buffer.
 * If there a no samples left in the input buffer: fill the rest
 * of the output buffer with zeros.
 *
 * The loop:
 * get a buffer from the input.
 * if it's bigger than needed, set an offset.
 * The rest of the data:
 * 0) convert endianness (codec) from remote to local...
 * 1) change bits in of the samples
 * 2) change sample rate
 * 3) change the nummber of channels
 * 4) insert into output buffer
 */

/*
 // get our first buffer:

 if ( stream_shift_buffer(id, &buf) == -1 ) {
  return -1;
 }

 // first test for some basic simple cases...

 if ( buf == NULL ) { // we habe nothing in input queue
                      // we may memset() our output buffer OR
                      // just return with -1 so we are going to
                      // be ignored.
  return -1;
 }
*/

 while (todo) { // main loop
  ROAR_DBG("streams_fill_mixbuffer(*): looping...");
  // exit loop if nothing is left, even if we need more data..
  if ( stream_shift_buffer(id, &buf) == -1 )
   break;
  if ( buf == NULL )
   break;

  // read the data for this loop...
  roar_buffer_get_data(buf, &in);
  roar_buffer_get_len(buf, &len);

  ROAR_DBG("streams_fill_mixbuffer(*): len = %i", len);

  if ( len > todo_in ) {
   roar_buffer_set_offset(buf, todo_in);
   len = todo_in;
  } else {
   roar_buffer_set_len(buf, 0); // queue for deletation
  }

  // we now have 'len' bytes in 'in'

  // calc how much outlen this has...
  outlen = (len * mul) / div;

  ROAR_DBG("streams_fill_mixbuffer(*): outlen = %i, buf = %p, len = %i", outlen, in, len);

  if ( is_the_same ) {
/*
 * 0) convert endianness (codec) from remote to local...
 * 1) change bits in of the samples
 * 2) change sample rate
 * 3) change the nummber of channels
   \\==> skiping,...
 */
   // * 4) insert into output buffer
   ROAR_DBG("streams_fill_mixbuffer(*): memcpy: in->rest: %p -> %p", in, rest);
   if ( memcpy(rest, in, len) != rest ) {
    ROAR_ERR("streams_fill_mixbuffer(*): memcpy returned invalid pointer.");
   }

  } else {

/*
   // * 0) convert endianness (codec) from remote to local...
   if ( stream_info->codec != info->codec ) {
    // we neet to convert...
    return -1;
   }

   // * 1) change bits in of the samples
   if ( stream_info->bits != info->bits ) {
    return -1;
   }

   // * 2) change sample rate
   if ( stream_info->rate != info->rate ) {
    return -1;
   }

   // * 3) change the nummber of channels
   if ( stream_info->channels != info->channels ) {
    return -1;
   }

   // * 4) insert into output buffer
*/
  // hey! we have roar_conv() :)

  if ( roar_conv(rest, in, 8*len / stream_info->bits, stream_info, info) == -1 )
   return -1;
  }

  if ( change_vol(rest, info->bits, rest, 8*outlen / info->bits, info->channels, &(((struct roar_stream_server*)g_streams[id])->mixer)) == -1 )
   return -1;

  // we habe outlen bytes more...
  todo    -= outlen;
  rest    += outlen;
  todo_in -= len;

  roar_buffer_get_len(buf, &len);
  ROAR_DBG("streams_fill_mixbuffer(*): New length of buffer %p is %i", buf, len);
  if ( len == 0 ) {
   roar_buffer_delete(buf, NULL);
  } else {
   stream_unshift_buffer(id, buf);
  }
 }

//len = 0;
//roar_buffer_get_len(buf, &len);

/*
 if ( len > 0 ) // we still have some data in this buffer, re-inserting it to the input buffers...
  stream_unshift_buffer(id, buf);
 else
  buffer_delete(buf, NULL);
*/

 if ( todo > 0 ) { // zeroize the rest of teh buffer
  memset(rest, 0, todo);

  if ( todo != ROAR_OUTPUT_CALC_OUTBUFSIZE(info) ) {
   if ( !g_streams[id]->is_new )
    ROAR_WARN("streams_fill_mixbuffer(*): Underrun in stream: %i bytes missing, filling with zeros", todo);

   g_streams[id]->is_new = 0;
  }
 } else {
  g_streams[id]->is_new = 0;
 }

 return 0;
}


int streams_get_mixbuffers (void *** bufferlist, struct roar_audio_info * info, unsigned int pos) {
 static void * bufs[ROAR_STREAMS_MAX+1];
 int i;
 int have = 0;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   if ( ((struct roar_stream *)g_streams[i])->dir != ROAR_DIR_PLAY )
    continue;

   if ( streams_get_outputbuffer(i, &bufs[have], ROAR_OUTPUT_CALC_OUTBUFSIZE(info)) == -1 ) {
    ROAR_ERR("streams_get_mixbuffer(*): Can not alloc output buffer for stream %i, BAD!", i);
    ROAR_ERR("streams_get_mixbuffer(*): Ignoring stream for this round.");
    continue;
   }
   if ( streams_fill_mixbuffer(i, info) == -1 ) {
    ROAR_ERR("streams_get_mixbuffer(*): Can not fill output buffer for stream %i, this should not happen", i);
    continue;
   }

//   printf("D: bufs[have=%i] = %p\n", have, bufs[have]);

   ROAR_DBG("streams_get_mixbuffers(*):  bufs[have] = %p", bufs[have]);
   ROAR_DBG("streams_get_mixbuffers(*): *bufs[have] = 0x%08x...", *(uint32_t*)bufs[have]);

   have++; // we have a new stream!
  }
 }

 bufs[have] = NULL;
 //printf("D: bufs[have=%i] = %p\n", have, bufs[have]);

 ROAR_DBG("streams_get_mixbuffers(*): have = %i", have);

 *bufferlist = bufs;
 return 0;
}


int stream_add_buffer  (int id, struct roar_buffer * buf) {
 ROAR_DBG("stream_add_buffer(id=%i, buf=%p) = ?", id, buf);

 if ( g_streams[id] == NULL )
  return -1;

 if ( g_streams[id]->buffer == NULL ) {
  g_streams[id]->buffer = buf;
  ROAR_DBG("stream_add_buffer(id=%i, buf=%p) = 0", id, buf);
  return 0;
 }

 ROAR_DBG("stream_add_buffer(id=%i, buf=%p) = ?", id, buf);
 return roar_buffer_add(g_streams[id]->buffer, buf);
}

int stream_shift_buffer   (int id, struct roar_buffer ** buf) {
 struct roar_buffer * next;

 if ( g_streams[id] == NULL )
  return -1;

 if ( g_streams[id]->buffer == NULL ) {
  *buf = NULL;
  return 0;
 }

 roar_buffer_get_next(g_streams[id]->buffer, &next);

 *buf                  = g_streams[id]->buffer;
 g_streams[id]->buffer = next;

 return 0;
}
int stream_unshift_buffer (int id, struct roar_buffer *  buf) {
 if ( g_streams[id] == NULL )
  return -1;

 if ( g_streams[id]->buffer == NULL ) {
  g_streams[id]->buffer = buf;
  return 0;
 }

 buf->next = NULL;

 roar_buffer_add(buf, g_streams[id]->buffer);

 g_streams[id]->buffer = buf;

 return 0;
}

int streams_check  (int id) {
 int fh;
 size_t req;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_buffer        *   b;
 char                      * buf;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_check(id=%i) = ?", id);

 s = (struct roar_stream *) (ss = g_streams[id]);

 if ( (fh = s->fh) == -1 )
  return 0;

 if ( s->dir != ROAR_DIR_PLAY )
  return 0;

 ROAR_DBG("streams_check(id=%i): fh = %i", id, fh);

 req  = ROAR_OUTPUT_BUFFER_SAMPLES * s->info.channels * s->info.bits / 8; // optimal size
 req += ss->need_extra; // bytes left we sould get....

 if ( roar_buffer_new(&b, req) == -1 ) {
  ROAR_ERR("streams_check(*): Can not alloc buffer space!");
  ROAR_DBG("streams_check(*) = -1");
  return -1;
 }

 roar_buffer_get_data(b, (void **)&buf);

 ROAR_DBG("streams_check(id=%i): buffer is up and ready ;)", id);

 if ( ss->codecfilter == -1 ) {
  req = read(fh, buf, req);
 } else {
  req = codecfilter_read(ss->codecfilter_inst, ss->codecfilter, buf, req);
 }

 if ( req > 0 ) {
  ROAR_DBG("streams_check(id=%i): got %i bytes", id, req);

  roar_buffer_set_len(b, req);

  if ( stream_add_buffer(id, b) != -1 )
   return 0;

  ROAR_ERR("streams_check(id=%i): something is wrong, could not add buffer to stream!", id);
  roar_buffer_free(b);
 } else {
  ROAR_DBG("streams_check(id=%i): EOF!", id);
  roar_buffer_free(b);
  streams_delete(id);
  ROAR_DBG("streams_check(id=%i) = 0", id);
  return 0;
 }


 ROAR_DBG("streams_check(id=%i) = -1", id);
 return -1;
}


int streams_send_mon   (int id) {
 int fh;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_send_mon(id=%i) = ?", id);

 s = (struct roar_stream *) (ss = g_streams[id]);

 if ( (fh = s->fh) == -1 )
  return 0;

 if ( s->dir != ROAR_DIR_MONITOR )
  return 0;

 ROAR_DBG("streams_send_mon(id=%i): fh = %i", id, fh);

 if ( write(fh, g_output_buffer, g_output_buffer_len) == g_output_buffer_len )
  return 0;

 if ( errno == EAGAIN ) {
  // ok, the client blocks for a moment, we try to sleep a bit an retry in the hope not to
  // make any gapes in any output because of this

  usleep(100); // 0.1ms

  if ( write(fh, g_output_buffer, g_output_buffer_len) == g_output_buffer_len )
   return 0;
 }

 // ug... error... delete stream!

 streams_delete(id);

 return -1;
}

int streams_send_filter(int id) {
 int fh;
 int have = 0;
 int len;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_send_filter(id=%i) = ?", id);

 s = (struct roar_stream *) (ss = g_streams[id]);

 if ( (fh = s->fh) == -1 )
  return 0;

 if ( s->dir != ROAR_DIR_FILTER )
  return 0;

 ROAR_DBG("streams_send_filter(id=%i): fh = %i", id, fh);

 if ( write(fh, g_output_buffer, g_output_buffer_len) == g_output_buffer_len ) {
  while ( have < g_output_buffer_len ) {
   if ( (len = read(fh, g_output_buffer+have, g_output_buffer_len-have)) < 1 ) {
    streams_delete(id);
    return -1;
   }
   have += len;
  }
  return 0;
 }

 // ug... error... delete stream!

 streams_delete(id);

 return -1;
}

//ll
