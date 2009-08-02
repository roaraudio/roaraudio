//streams.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
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
 */

#include "roard.h"

int streams_thru_num = 0;

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
 struct roar_stream        * n = NULL;
 struct roar_stream_server * s = NULL;

#ifdef ROAR_SUPPORT_LISTEN
 if ( g_terminate && !g_no_listen ) // don't accept new streams in case of termination state
  return -1;
#else
 if ( g_terminate )                 // don't accept new streams in case of termination state
  return -1;
#endif

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] == NULL ) {
   s = ROAR_STREAM_SERVER(n = ROAR_STREAM(malloc(sizeof(struct roar_stream_server))));
   if ( n == NULL ) {
    ROAR_ERR("streams_new(void): can not allocate memory for new stream: %s", strerror(errno));
    ROAR_DBG("streams_new(void) = -1");
    return -1;
   }

   n->id         = i;
   n->fh         = -1;
//   n->pos_rel_id = i;
/*
   n->database   = NULL;
   n->dataoff    = NULL;
   n->datalen    = 0;
   n->offset     = 0;
*/
   n->pos        = 0;

   s->name            = NULL;

   s->client          = -1;
   s->socktype        = ROAR_SOCKET_TYPE_UNKNOWN;
   s->buffer          = NULL;
   s->need_extra      =  0;
   s->output          = NULL;
   s->is_new          =  1;
   s->codecfilter     = -1;
   s->pre_underruns   =  0;
   s->post_underruns  =  0;
   s->delay           =  0;
   s->codec_orgi      = -1;
   s->primary         =  0;
   s->ready           =  0;
   s->outputbuffer    = NULL;

   s->mixer.scale     = 65535;
   s->mixer.rpg_mul   = 1;
   s->mixer.rpg_div   = 1;
   for (j = 0; j < ROAR_MAX_CHANNELS; j++)
    s->mixer.mixer[j] = 65535;

#ifdef ROAR_SUPPORT_META
   for (j = 0; j < ROAR_META_MAX_PER_STREAM; j++) {
    s->meta[j].type   = ROAR_META_TYPE_NONE;
    s->meta[j].key[0] = 0;
    s->meta[j].value  = NULL;
   }
#endif

   roar_vio_init_calls(&(s->vio));
   s->driver_id = -1;
   s->flags     =  ROAR_FLAG_NONE;

   //roardsp_fchain_init(&(s->fc));

   g_streams[i] = s;
   ROAR_DBG("streams_new(void): n->id=%i", n->id);
   ROAR_DBG("streams_new(void) = %i", i);
   return i;
  }
 }

 return -1;
}

int streams_delete (int id) {
 struct roar_stream_server * s;
 int prim;
 int no_vio_close = 0;
 int i;

 if ( (s = g_streams[id]) == NULL )
  return 0;

 ROAR_DBG("streams_delete(id=%i) = ?", id);
 ROAR_DBG("streams_delete(id=%i): g_streams[id]->id=%i", id, ROAR_STREAM(s)->id);

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == id ) {
   switch (ROAR_STREAM(g_streams[i])->dir) {
    case ROAR_DIR_THRU:
    case ROAR_DIR_RAW_IN:
      streams_delete(i);
     break;
    default:
      ROAR_STREAM(g_streams[i])->pos_rel_id = -1;
   }
  }
 }

 if ( ROAR_STREAM(s)->dir == ROAR_DIR_THRU )
  streams_thru_num--;

#ifdef ROAR_SUPPORT_META
 // delete meta data form other meta streams if needed
 if ( streams_get_flag(id, ROAR_FLAG_META) == 1 ) {
  ROAR_DBG("streams_delete(id=%i): deleting meta stream!", id);
  stream_meta_clear(id);
  stream_meta_finalize(id);
 }
#endif

 if ( s->codecfilter != -1 ) {
  codecfilter_close(s->codecfilter_inst, s->codecfilter);
  s->codecfilter_inst = NULL;
  s->codecfilter = -1;
 }

 if ( s->driver_id != -1 ) {
  driver_closevio(&(s->vio), s->driver_id);
  roar_vio_init_calls(&(s->vio));
  s->driver_id = -1;
  no_vio_close =  1;
 }

 //roardsp_fchain_uninit(&(s->fc));

 if ( s->client != -1 ) {
  ROAR_DBG("streams_delete(id=%i): Stream is owned by client %i", id, g_streams[id]->client);
  client_stream_delete(s->client, id);
 }

 stream_outputbuffer_destroy(id);

 if ( s->buffer != NULL )
  roar_buffer_free(s->buffer);

 if ( s->output != NULL )
  free(s->output);

/*
 if ( ROAR_STREAM(s)->fh != -1 )
  close(ROAR_STREAM(s)->fh);
*/

 if ( !no_vio_close )
  roar_vio_close(&(s->vio));

 prim = s->primary;

 if ( s->name != NULL )
  free(s->name);

 free(s);

 g_streams[id] = NULL;

 if ( prim ) {
  alive = 0;
  clean_quit();
 }

 ROAR_DBG("streams_delete(id=%i) = 0", id);
 return 0;
}

int streams_set_client (int id, int client) {
 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_set_client(id=%i): g_streams[id]->id=%i", id, ROAR_STREAM(g_streams[id])->id);
 g_streams[id]->client = client;

 return 0;
}

int streams_get_client (int id) {
 if ( g_streams[id] == NULL )
  return -1;

 return g_streams[id]->client;
}

int streams_set_dir    (int id, int dir, int defaults) {
 struct roar_stream_server * ss;

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 ROAR_STREAM(ss)->dir = dir;

 if ( dir == ROAR_DIR_THRU )
  streams_thru_num++;

 if ( defaults ) {
  if ( dir <= 0 || dir >= ROAR_DIR_DIRIDS )
   return -1;

  ROAR_DBG("streams_set_dir(*): g_config->streams[dir=%i].flags = 0x%0.4x", dir, g_config->streams[dir].flags);

  if ( streams_set_flag(id, g_config->streams[dir].flags) == -1 ) {
   ROAR_DBG("streams_set_dir(*) = -1 // can not set stream flags");
   return -1;
  }

   ss->mixer.scale   = g_config->streams[dir].mixer.scale;
   ss->mixer.rpg_mul = g_config->streams[dir].mixer.rpg_mul;
   ss->mixer.rpg_div = g_config->streams[dir].mixer.rpg_div;
 }

 ROAR_DBG("streams_set_dir(*) = 0");
 return 0;
}

int streams_get_dir    (int id) {
 struct roar_stream_server * ss;

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 return ROAR_STREAM(ss)->dir;
}

int streams_set_fh     (int id, int fh) {
 struct roar_stream_server * ss;
 struct roar_stream        * s;
 int dir;

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 ROAR_DBG("streams_set_fh(id=%i): g_streams[id]->id=%i", id, s->id);

 s->fh = fh;

 ROAR_DBG("streams_set_fh(id=%i, fh=%i): driverID=%i", id, fh, ss->driver_id);

 if ( ss->driver_id == -1 && fh != -2 )
  roar_vio_set_fh(&(ss->vio), fh);

 if ( codecfilter_open(&(ss->codecfilter_inst), &(ss->codecfilter), NULL,
                  s->info.codec, ss) == -1 ) {
  return streams_delete(id); // TODO: FIXME: is this correct? shoudn't we return -1 in any case here?
 }

 if ( fh == -2 ) {
  ROAR_DBG("streams_set_fh(id=%i, fh=%i) = ?", id, fh);
  if ( roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_GET_READ_FH, &fh) == -1 ) {
   fh = -2;
  } else {
   ROAR_DBG("streams_set_fh(id=%i, fh=%i) = ?", id, fh);
   if ( fh < 0 ) {
    fh = -2;
   } else {
    s->fh = fh;
   }
  }
 }

 if ( fh == -1 || fh == -2 ) { // yes, this is valid, indecats full vio!
  ss->ready = 1;
  return 0;
 }

// roar_socket_recvbuf(fh, ROAR_OUTPUT_CALC_OUTBUFSIZE( &(ROAR_STREAM(g_streams[id])->info) )); // set recv buffer to minimum

 dir = ROAR_STREAM(ss)->dir;

 switch (dir) {
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_RECORD:
  case ROAR_DIR_OUTPUT:
  case ROAR_DIR_MIDI_OUT:
  case ROAR_DIR_LIGHT_OUT:
  case ROAR_DIR_RAW_OUT:
    ROAR_SHUTDOWN(fh, SHUT_RD);
   break;
 }

 if ( dir == ROAR_DIR_FILTER ) {
  ss->ready = 1;
  return 0;
 } else {
  if ( roar_socket_nonblock(fh, ROAR_SOCKET_NONBLOCK) == -1 )
   return -1;

  ss->ready = 1;
  return 0;
 }
}

int streams_get_fh     (int id) {
 if ( id < 0 )
  return -1;

 if ( g_streams[id] == NULL )
  return -1;

 return ROAR_STREAM(g_streams[id])->fh;
}

int streams_get    (int id, struct roar_stream_server ** stream) {
 if ( g_streams[id] == NULL )
  return -1;

 *stream = g_streams[id];

 return 0;
}

int streams_set_socktype (int id, int socktype) {
 if ( g_streams[id] == NULL )
  return -1;

 g_streams[id]->socktype = socktype;

 return 0;
}

int streams_get_socktype (int id) {
 if ( g_streams[id] == NULL )
  return -1;

 return g_streams[id]->socktype;
}

int streams_set_primary (int id, int prim) {
 if ( g_streams[id] == NULL )
  return -1;

 g_streams[id]->primary = prim;

 return 0;
}

int streams_mark_primary (int id) {
 return streams_set_primary(id, 1);
}

int streams_set_sync     (int id, int sync) {
 int fh = streams_get_fh(id);

 if ( fh != -1 ) {
  if ( roar_socket_nonblock(fh, sync ? ROAR_SOCKET_BLOCK : ROAR_SOCKET_NONBLOCK) == -1 )
   return -1;

#ifdef ROAR_FDATASYNC
  ROAR_FDATASYNC(fh);
#endif

  return 0;
 } else {
  return roar_vio_nonblock(&(g_streams[id]->vio), sync);
 }
}

int streams_set_mmap (int id, int reset) {
 int use = !reset;

 if ( g_streams[id] == NULL )
  return -1;

 return roar_vio_ctl(&(g_streams[id]->vio), ROAR_VIO_CTL_SET_UMMAP, &use);
}

int streams_set_flag     (int id, int flag) {
 if ( g_streams[id] == NULL )
  return -1;

 if ( flag & ROAR_FLAG_MMAP )
  if ( streams_set_mmap(id, 0) == -1 )
   flag -= ROAR_FLAG_MMAP;

 if ( flag & ROAR_FLAG_PRIMARY ) {
  streams_set_primary(id, 1);
  flag -= ROAR_FLAG_PRIMARY;
 }

 if ( flag & ROAR_FLAG_SYNC ) {
  switch (ROAR_STREAM(g_streams[id])->dir) {
   // for this stream types the flag is used in the subsystem:
   case ROAR_DIR_BRIDGE:
   case ROAR_DIR_MIDI_OUT:
    break;

   // the fh is updated as soon as teh fh get ready:
   case ROAR_DIR_FILTER:
    break;

   // normal behavor (vio blocking):
   default:
     if ( streams_set_sync(id, 1) == -1 )
      flag -= ROAR_FLAG_SYNC;
  }
 }

 if ( flag & ROAR_FLAG_HWMIXER ) { // currently not supported -> ignored
  g_streams[id]->flags |= flag;
  if ( streams_set_mixer(id) == -1 ) {
   g_streams[id]->flags -= flag;
   return -1;
  }
 }

 g_streams[id]->flags |= flag;

#ifdef ROAR_SUPPORT_META
 if ( flag & ROAR_FLAG_META )
  stream_meta_finalize(id);
#endif

 return 0;
}

int streams_reset_flag   (int id, int flag) {
 if ( g_streams[id] == NULL )
  return -1;

 if ( flag & ROAR_FLAG_MMAP )
  if ( streams_set_mmap(id, 1) == -1 )
   flag -= ROAR_FLAG_MMAP;

 if ( flag & ROAR_FLAG_PRIMARY ) {
  streams_set_primary(id, 0);
  flag -= ROAR_FLAG_PRIMARY;
 }

 if ( flag & ROAR_FLAG_SYNC ) {
  // we refuse to reset the flag on FILTER streams
  if ( streams_get_dir(id) == ROAR_DIR_FILTER ) {
//   flags -= ROAR_FLAG_SYNC;
   return -1;
  } else {
   streams_set_sync(id, 0);
  }
 }

 g_streams[id]->flags |= flag;
 g_streams[id]->flags -= flag;

 return 0;
}

int streams_get_flag     (int id, int flag) {
 if ( g_streams[id] == NULL )
  return -1;

 return g_streams[id]->flags & flag ? 1 : 0;
}

int streams_set_name     (int id, char * name) {
 char * str;

 if ( g_streams[id] == NULL )
  return -1;

 if ( (str = strdup(name)) == NULL )
  return -1;

 if ( g_streams[id]->name != NULL )
  free(g_streams[id]->name);

 g_streams[id]->name = str;

 return 0;
}

char * streams_get_name  (int id) {
 if ( g_streams[id] == NULL )
  return NULL;

 return g_streams[id]->name;
}


int streams_calc_delay    (int id) {
 struct roar_stream_server * ss;
 struct roar_stream        * s;
 register uint_least32_t d = 0;
 uint_least32_t t[1];
 uint64_t       tmp;

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 if ( ss->codecfilter != -1 ) {
  if ( codecfilter_delay(ss->codecfilter_inst, ss->codecfilter, t) != -1 )
   d += *t;
 }

 if ( ss->vio.ctl != NULL ) {
  if ( roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_GET_DELAY, t) != -1 ) { // *t is in byte
   ROAR_DBG("streams_calc_delay(id=%i): VIO delay in byte: %i", id, *t);
   tmp = *t;
   tmp *= 1000000; // musec per sec
   tmp /= s->info.rate * s->info.channels * (s->info.bits/8);
   ROAR_DBG("streams_calc_delay(id=%i): VIO delay in musec: %i", id, tmp);

   d += tmp;
  }
 }

 ROAR_DBG("streams_calc_delay(id=%i): delay in musec: %i", id, d);

 ss->delay = d;

 return 0;
}

int streams_set_mixer    (int id) {
 struct roar_stream_server * ss;

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( !streams_get_flag(id, ROAR_FLAG_HWMIXER) )
  return 0;

 if ( ss->driver_id == -1 )
  return 0;

 return driver_set_volume(id, &(ss->mixer));
}

int streams_ctl          (int id, int_least32_t cmd, void * data) {
 struct roar_stream_server * ss;
 int_least32_t comp;

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 comp = cmd & ROAR_STREAM_CTL_COMPMASK;

 cmd &= ~comp;

 ROAR_DBG("streams_ctl(id=%i, cmd=?, data=%p): comp=0x%.8x, cmd=0x%.4x", id, data, comp, cmd);

 switch (comp) {
  case ROAR_STREAM_CTL_COMP_BASE:
   break;
  case ROAR_STREAM_CTL_COMP_CF:
    return codecfilter_ctl(ss->codecfilter_inst, ss->codecfilter, cmd, data);
   break;
  case ROAR_STREAM_CTL_COMP_DRV:
   break;
  default:
   return -1;
 }

 return -1;
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
 size_t needed = todo;
 size_t todo_in;
 size_t len, outlen;
 size_t mul = 1, div = 1;
 void * rest = NULL;
 void * in   = NULL;
 struct roar_buffer     * buf;
 struct roar_audio_info * stream_info;
 struct roar_stream_server * stream = g_streams[id];
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

 stream_info = &(ROAR_STREAM(stream)->info);

 // calc todo_in
 todo_in = ROAR_OUTPUT_CALC_OUTBUFSIZE(stream_info);

 if ( todo_in == 0 ) {
  ROAR_WARN("streams_fill_mixbuffer(id=%i, info=%p{...}): todo_in == 0, this should not happen!", id, info);
  return -1;
 }

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

  if ( !streams_get_flag(id, ROAR_FLAG_HWMIXER) ) {
   if ( change_vol(rest, info->bits, rest, 8*outlen / info->bits, info->channels, &(stream->mixer)) == -1 )
    return -1;
  }

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

 ROAR_STREAM(g_streams[id])->pos =
      ROAR_MATH_OVERFLOW_ADD(ROAR_STREAM(g_streams[id])->pos,
          ROAR_OUTPUT_CALC_OUTBUFSAMP(info, needed-todo));
 //ROAR_WARN("stream=%i, pos=%u", id, ((struct roar_stream*)g_streams[id])->pos);

 if ( todo > 0 ) { // zeroize the rest of the buffer
  memset(rest, 0, todo);

  if ( todo != ROAR_OUTPUT_CALC_OUTBUFSIZE(info) ) {
   if ( g_streams[id]->is_new ) {
    stream->pre_underruns++;
   } else {
    ROAR_WARN("streams_fill_mixbuffer(*): Underrun in stream: %u bytes missing, filling with zeros", (unsigned int)todo);
    stream->post_underruns++;
   }

   stream->is_new = 0;
  }
 } else {
  stream->is_new = 0;
 }

 return 0;
}

int streams_fill_mixbuffer2 (int id, struct roar_audio_info * info) {
 size_t   outlen = ROAR_OUTPUT_CALC_OUTBUFSIZE(info);
 void   * outdata;
 size_t   inlen;
 size_t   inlen_got;
 void   * indata = NULL;
 size_t   buflen;
 void   * bufdata = NULL;
 struct roar_buffer * bufbuf = NULL;
 int      is_the_same = 0;
 struct roar_audio_info    * stream_info;
 struct roar_stream        * s;
 struct roar_stream_server * ss;

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 // set up stream_info
 stream_info = &(s->info);

 // calc todo_in
 inlen = ROAR_OUTPUT_CALC_OUTBUFSIZE(stream_info);

 buflen = ROAR_OUTPUT_CALC_OUTBUFSIZE_MAX(info, stream_info);

 if ( inlen == 0 ) {
  ROAR_WARN("streams_fill_mixbuffer2(id=%i, info=%p{...}): inlen == 0, this should not happen!", id, info);
  return -1;
 }

 if ( streams_get_outputbuffer(id, &outdata, outlen) == -1 ) {
  return -1;
 }

 if ( outdata == NULL ) {
  return -1;
 }

 ROAR_DBG("streams_fill_mixbuffer2(*): outdata=%p, len=%i->%i (in->out)", outdata, inlen, outlen);

 is_the_same = stream_info->rate     == info->rate     && stream_info->bits  == info->bits &&
               stream_info->channels == info->channels && stream_info->codec == info->codec;

 ROAR_DBG("streams_fill_mixbuffer2(*): is_the_same=%i", is_the_same);

 if ( !is_the_same && buflen > outlen ) {
/*
  // this is not supported at the moment
  memset(outdata, 0, outlen);
  return -1;
*/

  if ( roar_buffer_new(&bufbuf, buflen) == -1 )
   return -1;

  if ( roar_buffer_get_data(bufbuf, &bufdata) == -1 )
   return -1;
  indata  = bufdata;
 } else {
  indata  = outdata;
  bufdata = outdata;
 }

 inlen_got = inlen;

 ROAR_DBG("streams_fill_mixbuffer2(id=%i, info=...): inlen_got=%u", id, inlen_got);

 if ( stream_shift_out_buffer(id, indata, &inlen_got) == -1 ) {
  if ( ss->is_new ) {
   ss->pre_underruns++;
  } else {
   ROAR_WARN("streams_fill_mixbuffer2(id=%i, info=...): underrun in stream", id);
   ss->post_underruns++;
  }
  memset(outdata, 0, outlen);
  return 0;
 }

 if ( ss->is_new ) {
  ROAR_WARN("streams_fill_mixbuffer2(id=%i, info=...): stream state: new->old", id);
 }

 ss->is_new = 0;

 if ( is_the_same ) {
  if ( indata != outdata )
   memcpy(outdata, indata, inlen);

  if ( inlen < outlen )
   memset(outdata+inlen, 0, outlen-inlen);
 } else {
//  if ( roar_conv(outdata, indata, (8*inlen_got*info->rate)/(stream_info->rate * stream_info->bits), stream_info, info) == -1 ) {
  if ( roar_conv2(bufdata, indata, inlen, stream_info, info, buflen) == -1 ) {
   if ( bufbuf != NULL )
    roar_buffer_free(bufbuf);
   return -1;
  }

//  memset(outdata, 0, outlen);
 }

 if ( bufbuf != NULL ) {
  memcpy(outdata, bufdata, outlen);
  roar_buffer_free(bufbuf);
 }

 if ( !streams_get_flag(id, ROAR_FLAG_HWMIXER) ) {
  if ( change_vol(outdata, info->bits, outdata, 8*outlen / info->bits, info->channels, &(ss->mixer)) == -1 )
   return -1;
 }

 if ( streams_get_flag(id, ROAR_FLAG_ANTIECHO) ) {
  // we can ignore errors here:
  if ( stream_outputbuffer_request(id, &bufbuf, outlen) == 0 ) {
   if ( roar_buffer_get_data(bufbuf, &bufdata) != -1 )
    memcpy(bufdata, outdata, outlen);
  }
 }


 return 0;
}


int streams_get_mixbuffers (void *** bufferlist, struct roar_audio_info * info, unsigned int pos) {
 static void * bufs[ROAR_STREAMS_MAX+1];
 int i;
 int have = 0;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   if ( ROAR_STREAM(g_streams[i])->dir != ROAR_DIR_PLAY && ROAR_STREAM(g_streams[i])->dir != ROAR_DIR_BIDIR )
    continue;

   if ( streams_get_outputbuffer(i, &bufs[have], ROAR_OUTPUT_CALC_OUTBUFSIZE(info)) == -1 ) {
    ROAR_ERR("streams_get_mixbuffer(*): Can not alloc output buffer for stream %i, BAD!", i);
    ROAR_ERR("streams_get_mixbuffer(*): Ignoring stream for this round.");
    continue;
   }
   if ( streams_fill_mixbuffer2(i, info) == -1 ) {
    ROAR_ERR("streams_get_mixbuffer(*): Can not fill output buffer for stream %i, this should not happen", i);
    continue;
   }

//   printf("D: bufs[have=%i] = %p\n", have, bufs[have]);

   ROAR_DBG("streams_get_mixbuffers(*):  bufs[have] = %p", bufs[have]);
   ROAR_DBG("streams_get_mixbuffers(*): *bufs[have] = 0x%08x...", *(uint32_t*)bufs[have]);

   if ( !streams_get_flag(i, ROAR_FLAG_MUTE) )
    have++; // we have a new stream!
  }
 }

 bufs[have] = NULL;
 //printf("D: bufs[have=%i] = %p\n", have, bufs[have]);

 ROAR_DBG("streams_get_mixbuffers(*): have = %i", have);

 *bufferlist = bufs;
 return have;
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

int stream_shift_out_buffer   (int id, void * data, size_t * len) {
 if ( g_streams[id] == NULL )
  return -1;

 if ( g_streams[id]->buffer == NULL )
  return -1;

 return roar_buffer_shift_out(&(g_streams[id]->buffer), data, len);
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

int stream_outputbuffer_request(int id, struct roar_buffer ** buf, size_t len) {
 register struct roar_stream_server *  ss;
 size_t buflen;
 void * bufdata;
 int ret;

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( buf != NULL ) /* just be be sure */
  *buf = NULL;

 if ( ss->outputbuffer != NULL ) {
  if ( roar_buffer_get_len(ss->outputbuffer, &buflen) == 0 ) {
   if ( buflen == len ) {
    if ( buf != NULL )
     *buf = ss->outputbuffer;
    return 0;
   } else if ( buflen > len ) {
    if ( roar_buffer_set_len(ss->outputbuffer, len) == 0 ) {
     if ( buf != NULL )
      *buf = ss->outputbuffer;
     return 0;
    }
   }
  }

  // if the buffer is not suitable:

  ret = roar_buffer_free(ss->outputbuffer);
  ss->outputbuffer = NULL;
  if ( ret == -1 )
   return ret;
 }

 if ( roar_buffer_new(&(ss->outputbuffer), len) == -1 )
  return -1;

 if ( roar_buffer_get_data(ss->outputbuffer, &bufdata) == -1 ) {
  roar_buffer_free(ss->outputbuffer);
  ss->outputbuffer = NULL;
  return -1;
 }

 memset(bufdata, 0, len);

 if ( buf != NULL )
  *buf = ss->outputbuffer;

 return 0;
}

int stream_outputbuffer_destroy(int id) {
 int ret;
 register struct roar_stream_server *  ss;

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( ss->outputbuffer != NULL ) {
  ret = roar_buffer_free(ss->outputbuffer);
  ss->outputbuffer = NULL;
  return ret;
 }

 return 0;
}

int streams_check  (int id) {
 int fh;
 ssize_t req, realreq, done;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_buffer        *   b;
 char                      * buf;
// char                        tmp;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_check(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 if ( (fh = s->fh) == -1 )
  return 0;

 if ( streams_get_flag(id, ROAR_FLAG_PAUSE) )
  return 0;

 switch (s->dir) {
  case ROAR_DIR_LIGHT_IN:
    return light_check_stream(id);
   break;
  case ROAR_DIR_MIDI_IN:
    return midi_check_stream(id);
   break;
  case ROAR_DIR_RAW_IN:
    return raw_check_stream(id);
   break;
  case ROAR_DIR_PLAY:
  case ROAR_DIR_BIDIR:
   break;
  default:
/*
    ROAR_WARN("streams_check(id=%i): Read event on non input stream of type/dir %s", id, roar_dir2str(s->dir));
    errno = 0;
    req = stream_vio_s_read(ss, &tmp, 1);
    ROAR_DBG("streams_check(id=%i): stream_vio_s_read(ss, &tmp, 1) = %li // errno=%s(%i)", id, req, strerror(errno), errno);
    if ( req == 1 ) {
     ROAR_ERR("streams_check(id=%i): Client violates protocol, got one byte of data on output stream, kicking stream");
     streams_delete(id);
     return -1;
    }
*/
    return 0;
   break;
 }

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
  realreq = req;
/*
  req = read(fh, buf, req);
  if ( req < realreq ) { // we can do this as the stream is in nonblocking mode!
   if ( (realreq = read(fh, buf+req, realreq-req)) > 0 )
    req += realreq;
  }
*/
  done = 0;
  while (req > 0 && done != realreq) {
   if ( (req = stream_vio_s_read(ss, buf+done, realreq-done)) > 0 )
    done += req;
  }
  req = done;

  roar_buffer_get_data(b, (void **)&buf);
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
  ROAR_DBG("streams_check(id=%i): read() = %i // errno: %s", id, req, strerror(errno));
#ifdef ROAR_HAVE_LIBVORBISFILE
  if ( errno != EAGAIN && errno != ESPIPE ) { // libvorbis file trys to seek a bit ofen :)
#else
  if ( errno != EAGAIN ) {
#endif
   ROAR_DBG("streams_check(id=%i): EOF!", id);
   streams_delete(id);
   ROAR_DBG("streams_check(id=%i) = 0", id);
  }
  roar_buffer_free(b);
  return 0;
 }


 ROAR_DBG("streams_check(id=%i) = -1", id);
 return -1;
}


#define _return(x) return (x)
int streams_send_mon   (int id) {
// int fh;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_buffer        *  bufbuf = NULL;
 struct roar_remove_state     removalstate;
 void  * ip;
 void  * obuf;
 int     olen;
 int     is_the_same     = 1;
 int     is_vol_eq       = 1;
 int     antiecho        = 0;
 ssize_t ret;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_send_mon(id=%i) = ?", id);

 s = ROAR_STREAM((ss = g_streams[id]));

/*
 if ( (fh = s->fh) == -1 )
  return 0;
*/

 if ( !ss->ready )
  return 0;

 if ( streams_get_flag(id, ROAR_FLAG_PAUSE) )
  return 0;

 switch (s->dir) {
  case ROAR_DIR_LIGHT_OUT:
    return light_send_stream(id);
   break;
  case ROAR_DIR_MIDI_OUT:
    return midi_send_stream(id);
   break;
  case ROAR_DIR_OUTPUT:
    if ( g_standby )
     return 0;
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_BIDIR:
   break;

  default:
    return 0;
   break;
 }


 ROAR_DBG("streams_send_mon(id=%i): fh = %i", id, s->fh);

 if ( s->info.channels != g_sa->channels || s->info.bits  != g_sa->bits ||
      s->info.rate     != g_sa->rate     || s->info.codec != g_sa->codec  )
  is_the_same = 0;

 if ( !streams_get_flag(id, ROAR_FLAG_HWMIXER) ) {
  is_vol_eq = need_vol_change(g_sa->channels, &(ss->mixer)) ? 0 : 1;
 }

 if ( streams_get_flag(id, ROAR_FLAG_ANTIECHO) )
  antiecho = 1;

 if ( !is_the_same || !is_vol_eq || antiecho ) {
  olen = ROAR_OUTPUT_CALC_OUTBUFSIZE(&(s->info)); // we hope g_output_buffer_len
                                                  // is ROAR_OUTPUT_CALC_OUTBUFSIZE(g_sa) here
  if ( stream_outputbuffer_request(id, &bufbuf, olen) == -1 )
   return -1;

  if ( roar_buffer_get_data(bufbuf, &obuf) == -1 ) {
   _return(-1);
  }

  ROAR_DBG("streams_send_mon(id=%i): obuf=%p, olen=%i", id, obuf, olen);
 } else {
  obuf = g_output_buffer;
  olen = g_output_buffer_len;
 }

 ip = g_output_buffer;

 if ( antiecho ) {
  if ( roar_remove_init(&removalstate) == -1 ) {
   _return(-1);
  }

  if ( roar_remove_so(obuf, ip, ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels, g_sa->bits, &removalstate) == -1 ) {
   _return(-1);
  }
 }

 if ( !is_vol_eq ) {
  if ( change_vol(obuf, g_sa->bits, ip, ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels, g_sa->channels, &(ss->mixer)) == -1 ) {
   _return(-1);
  }

  ip = obuf;
 }

 if ( !is_the_same ) {
  if ( roar_conv(obuf, ip, ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels, g_sa, &(s->info)) == -1 ) {
   _return(-1);
  }
 }

 errno = 0;

 if ( ss->codecfilter == -1 ) {
  ROAR_DBG("streams_send_mon(id=%i): not a CF stream", id);
  if ( s->fh == -1 && roar_vio_get_fh(&(ss->vio)) == -1 ) {
   _return(0);
  }

  if ( (ret = stream_vio_s_write(ss, obuf, olen)) == olen ) {
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), olen)*s->info.channels);
   _return(0);
  }

  if ( ret > 0 && errno == 0 ) {
   ROAR_WARN("streams_send_mon(id=%i): Overrun in stream: wrote %i of %i bytes, %i bytes missing", id, (int)ret, olen, olen-(int)ret);
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), ret)*s->info.channels);
   _return(0);
  }
 } else {
  errno = 0;
  if ( codecfilter_write(ss->codecfilter_inst, ss->codecfilter, obuf, olen)
            == olen ) {
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), olen)*s->info.channels);
   _return(0);
  } else { // we cann't retry on codec filetered streams
   if ( errno != EAGAIN ) {
    streams_delete(id);
    _return(-1);
   }
  }
 }

 if ( errno == EAGAIN ) {
  // ok, the client blocks for a moment, we try to sleep a bit an retry in the hope not to
  // make any gapes in any output because of this

#ifdef ROAR_HAVE_USLEEP
  usleep(100); // 0.1ms
#endif

  if ( stream_vio_s_write(ss, obuf, olen) == olen ) {
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), olen)*s->info.channels);
   _return(0);
  } else if ( errno == EAGAIN ) {
   ROAR_WARN("streams_send_mon(id=%i): Can not send data to client: %s", id, strerror(errno));
   _return(0);
  }
 }

 // ug... error... delete stream!

 streams_delete(id);

 _return(-1);
}
#undef _return

int streams_send_filter(int id) {
 int fh;
 int have = 0;
 int len;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("streams_send_filter(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 if ( (fh = s->fh) == -1 )
  return 0;

 if ( s->dir != ROAR_DIR_FILTER )
  return 0;

 if ( streams_get_flag(id, ROAR_FLAG_PAUSE) )
  return 0;


 ROAR_DBG("streams_send_filter(id=%i): fh = %i", id, fh);

 if ( stream_vio_s_write(ss, g_output_buffer, g_output_buffer_len) == g_output_buffer_len ) {
  while ( have < g_output_buffer_len ) {
   if ( (len = stream_vio_s_read(ss, g_output_buffer+have, g_output_buffer_len-have)) < 1 ) {
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


// VIO:

ssize_t stream_vio_read (int stream, void *buf, size_t count) {
 struct roar_stream_server * s = g_streams[stream];

 if ( !s )
  return -1;

 return stream_vio_s_read(s, buf, count);
}

ssize_t stream_vio_write(int stream, void *buf, size_t count) {
 struct roar_stream_server * s = g_streams[stream];

 if ( !s )
  return -1;

 return stream_vio_s_write(s, buf, count);
}


ssize_t stream_vio_s_read (struct roar_stream_server * stream, void *buf, size_t count) {
 void    * orig_buf = buf;
  size_t   len      =  0;
 ssize_t   r        = -1;
 int       i;

 errno = 0;

 if ( !stream )
  return -1;

 //roar_vio_set_fh(&(stream->vio), ROAR_STREAM(stream)->fh);

 if ( ! stream->vio.read )
  return -1;

 while ( (r = roar_vio_read(&(stream->vio), buf, count)) > 0 ) {
  len   += r;
  buf   += r;
  count -= r;
  if ( count == 0 )
   break;
 }

 if ( len == 0 && r == -1 )
  return -1;

 if ( streams_thru_num )
  for (i = 0; i < ROAR_STREAMS_MAX; i++)
   if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == ROAR_STREAM(stream)->id )
    if ( ROAR_STREAM(g_streams[i])->dir == ROAR_DIR_THRU )
     if ( g_streams[i]->ready )
      if ( stream_vio_write(i, orig_buf, len) != len )
       streams_delete(i);

 return len;
}

ssize_t stream_vio_s_write(struct roar_stream_server * stream, void *buf, size_t count) {
 int i;

 errno = 0;

 if ( !stream )
  return -1;

/*
 if ( roar_vio_get_fh(&(stream->vio)) == -1 && ROAR_STREAM(stream)->fh != -1 )
  roar_vio_set_fh(&(stream->vio), ROAR_STREAM(stream)->fh);
*/

// ROAR_WARN("stream_vio_s_write(*): writing...");

 if ( streams_thru_num )
  for (i = 0; i < ROAR_STREAMS_MAX; i++)
   if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == ROAR_STREAM(stream)->id )
    if ( ROAR_STREAM(g_streams[i])->dir == ROAR_DIR_THRU )
     if ( g_streams[i]->ready )
      if ( stream_vio_write(i, buf, count) != count )
       streams_delete(i);

 return roar_vio_write(&(stream->vio), buf, count);
}

//ll
