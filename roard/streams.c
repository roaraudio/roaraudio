//streams.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#define _CHECK_SID_RET(id,ret) if ( (id) < 0 || (id) > ROAR_STREAMS_MAX || g_streams[(id)] == NULL ) return (ret)
#define _CHECK_SID(id)         _CHECK_SID_RET((id), -1)

int streams_thru_num     =  0;
int streams_recsource_id = -1;

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
   s = ROAR_STREAM_SERVER(n = ROAR_STREAM(roar_mm_malloc(sizeof(struct roar_stream_server))));
   if ( n == NULL ) {
    ROAR_ERR("streams_new(void): can not allocate memory for new stream: %s", strerror(errno));
    ROAR_DBG("streams_new(void) = -1");
    return -1;
   }

   memset(s, 0, sizeof(struct roar_stream_server));

   n->id         = i;
   n->fh         = -1;
   n->pos_rel_id = -1;
/*
   n->database   = NULL;
   n->dataoff    = NULL;
   n->datalen    = 0;
   n->offset     = 0;
*/
   n->pos        = 0;

   s->name            = NULL;

   s->state           = ROAR_STREAMSTATE_INITING;

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
   s->prethru         = NULL;
   s->mixer_stream    = -1;
   s->role            = ROAR_ROLE_UNKNOWN;

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
   roar_vio_init_calls(&(s->jumbo));
   s->viop      = &(s->vio);
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
 int client;

 _CHECK_SID(id);

 if ( (s = g_streams[id]) == NULL )
  return 0;

 ROAR_DBG("streams_delete(id=%i) = ?", id);
 ROAR_DBG("streams_delete(id=%i): g_streams[id]->id=%i", id, ROAR_STREAM(s)->id);

 // in case we are allready closing it...
 if ( s->state == ROAR_STREAMSTATE_CLOSING )
  return 0;

 s->state = ROAR_STREAMSTATE_CLOSING;

 if ( streams_get_flag(id, ROAR_FLAG_RECSOURCE) == 1 )
  streams_reset_flag(id, ROAR_FLAG_RECSOURCE);

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == id ) {
   switch (ROAR_STREAM(g_streams[i])->dir) {
    case ROAR_DIR_THRU:
    case ROAR_DIR_RAW_IN:
      if ( i != id )
       streams_delete(i);
     break;
    default:
      if ( streams_get_flag(i, ROAR_FLAG_VIRTUAL) == 1 ) {
       if ( i != id ) {
        ROAR_DBG("streams_delete(id=%i): Deleting virtual child stream %i", id, i);
        streams_delete(i);
       }
      } else {
       ROAR_STREAM(g_streams[i])->pos_rel_id = -1;
      }
   }
  }
 }

 if ( ROAR_STREAM(s)->dir == ROAR_DIR_THRU )
  streams_thru_num--;

 if ( streams_get_flag(id, ROAR_FLAG_VIRTUAL) == 1 ) {
  // we un-group the stream here to avoid a client deleting the parent deleting the client deleting ...
  i      = ROAR_STREAM(s)->pos_rel_id;
  if ( i != -1 ) {
   ROAR_STREAM(s)->pos_rel_id = -1;
   client = streams_get_client(id);
   streams_set_client(id, -1);
   ROAR_DBG("streams_delete(id=%i): Stream has flag virtual, notifying parent stream %i", id, i);
   streams_ctl(i, ROAR_CODECFILTER_CTL_VIRTUAL_DELETE|ROAR_STREAM_CTL_TYPE_INT, &id);
   ROAR_DBG("streams_delete(id=%i): Notify send to stream %i", id, i);
   streams_set_client(id, client);
   ROAR_STREAM(s)->pos_rel_id = i;
  }
 }

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
 stream_prethru_destroy(id);

 if ( s->buffer != NULL )
  roar_buffer_free(s->buffer);

 if ( s->output != NULL )
  free(s->output);

/*
 if ( ROAR_STREAM(s)->fh != -1 )
  close(ROAR_STREAM(s)->fh);
*/

 if ( !no_vio_close )
  roar_vio_close(s->viop);

 prim = s->primary;

 if ( s->name != NULL )
  free(s->name);

 roar_mm_free(s);

 g_streams[id] = NULL;

 if ( prim ) {
  alive = 0;
  clean_quit();
 }

 ROAR_DBG("streams_delete(id=%i) = 0", id);
 return 0;
}

int streams_set_client (int id, int client) {

 _CHECK_SID(id);

 ROAR_DBG("streams_set_client(id=%i): g_streams[id]->id=%i", id, ROAR_STREAM(g_streams[id])->id);
 g_streams[id]->client = client;

 return 0;
}

int streams_get_client (int id) {
 _CHECK_SID(id);

 return g_streams[id]->client;
}

int streams_set_dir    (int id, int dir, int defaults) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 ROAR_STREAM(ss)->dir = dir;

 if ( dir == ROAR_DIR_THRU )
  streams_thru_num++;

 if ( defaults ) {
  if ( dir <= 0 || dir >= ROAR_DIR_DIRIDS )
   return -1;

  ROAR_DBG("streams_set_dir(*): g_config->streams[dir=%i].flags = 0x%.4x", dir, g_config->streams[dir].flags);

  if ( streams_set_flag(id, g_config->streams[dir].flags) == -1 ) {
   ROAR_DBG("streams_set_dir(*) = -1 // can not set stream flags");
   return -1;
  }

   ss->mixer.scale   = g_config->streams[dir].mixer.scale;
   ss->mixer.rpg_mul = g_config->streams[dir].mixer.rpg_mul;
   ss->mixer.rpg_div = g_config->streams[dir].mixer.rpg_div;
 }

 if ( dir != ROAR_DIR_MIXING ) {
  switch (streams_get_subsys(id)) {
   case ROAR_SUBSYS_WAVEFORM:
     streams_set_mixer_stream(id, g_waveform_mixer.stream);
     roardsp_chanlist_init(ss->chanmap.in,  ROAR_STREAM(ss)->info.channels, ROARDSP_CHANLIST_MAP_ROARAUDIO);
    break;
#ifndef ROAR_WITHOUT_DCOMP_MIDI
   case ROAR_SUBSYS_MIDI:
     roardsp_chanlist_init(ss->chanmap.in,  ROAR_STREAM(ss)->info.channels, ROARDSP_CHANLIST_MAP_MIDI);
     streams_set_mixer_stream(id, g_midi_mixer.stream);
    break;
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
   case ROAR_SUBSYS_LIGHT:
     streams_set_mixer_stream(id, g_light_mixer.stream);
    break;
#endif
  }

  memcpy(ss->chanmap.out, ss->chanmap.in, sizeof(ss->chanmap.out));
  streams_set_map(id, NULL, 0);
 } else {
  streams_set_mixer_stream(id, id);
 }

 ROAR_DBG("streams_set_dir(*) = 0");
 return 0;
}

int streams_get_dir    (int id) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 return ROAR_STREAM(ss)->dir;
}

int streams_set_mixer_stream(int id, int mixer) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 ss->mixer_stream = mixer;

 return 0;
}

int streams_get_mixer_stream(int id, int mixer) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 return ss->mixer_stream;
}

int streams_set_role   (int id, int role) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 ss->role = role;

 return 0;
}

int streams_get_subsys (int id) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 switch (ROAR_STREAM(ss)->dir) {
  case ROAR_DIR_PLAY:
  case ROAR_DIR_RECORD:
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_FILTER:
  case ROAR_DIR_OUTPUT:
  case ROAR_DIR_BIDIR:
    return ROAR_SUBSYS_WAVEFORM;
   break;
  case ROAR_DIR_MIDI_IN:
  case ROAR_DIR_MIDI_OUT:
    return ROAR_SUBSYS_MIDI;
   break;
  case ROAR_DIR_LIGHT_IN:
  case ROAR_DIR_LIGHT_OUT:
    return ROAR_SUBSYS_LIGHT;
   break;
  case ROAR_DIR_RAW_IN:
  case ROAR_DIR_RAW_OUT:
    return ROAR_SUBSYS_RAW;
   break;
  case ROAR_DIR_COMPLEX_IN:
  case ROAR_DIR_COMPLEX_OUT:
    return ROAR_SUBSYS_COMPLEX;
   break;
  case ROAR_DIR_THRU:
    return streams_get_subsys(ROAR_STREAM(ss)->pos_rel_id);
   break;
 }

 return -1;
}

int streams_get_ssdir  (int id) {
 struct roar_stream_server * ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 switch (ROAR_STREAM(ss)->dir) {
  case ROAR_DIR_PLAY:
  case ROAR_DIR_MIDI_IN:
  case ROAR_DIR_LIGHT_IN:
  case ROAR_DIR_RAW_IN:
  case ROAR_DIR_COMPLEX_IN:
    return STREAM_DIR_IN;
   break;
  case ROAR_DIR_RECORD:
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_OUTPUT:
  case ROAR_DIR_MIDI_OUT:
  case ROAR_DIR_LIGHT_OUT:
  case ROAR_DIR_RAW_OUT:
  case ROAR_DIR_COMPLEX_OUT:
    return STREAM_DIR_OUT;
   break;
  case ROAR_DIR_MIXING:
    return STREAM_DIR_NONE;
   break;
  case ROAR_DIR_FILTER:
  case ROAR_DIR_BIDIR:
    return STREAM_DIR_BIDIR;
   break;
  case ROAR_DIR_THRU:
    return streams_get_ssdir(ROAR_STREAM(ss)->pos_rel_id);
   break;
 }

 return -1;
}

#define _err() streams_delete(id); return -1;
int streams_new_virtual (int parent, struct roar_stream_server ** stream) {
 struct roar_stream_server * parent_ss, * ss;
 struct roar_stream        * parent_s , *  s;
 int id = -1;
 int client, dir;

 if ( streams_get(parent, &parent_ss) == -1 )
  return -1;

 if ( (client = streams_get_client(parent)) == -1 )
  return -1;

 if ( (dir = streams_get_dir(parent)) == -1 )
  return -1;

 if ( (id = streams_new()) == -1 ) {
  return -1;
 }

 if ( client_stream_add(client, id) == -1 ) {
  _err();
 }

 if ( streams_get(id, &ss) == -1 ) {
  _err();
 }

 if ( streams_set_dir(id, dir, 1) == -1 ) {
  _err();
 }

 s        = ROAR_STREAM(       ss);
 parent_s = ROAR_STREAM(parent_ss);

 s->pos_rel_id = parent;

 if ( streams_set_rawflag(id, ROAR_FLAG_VIRTUAL) == -1 ) {
  _err();
 }

 if ( stream != NULL )
  *stream = ss;

 return id;
}
#undef _err

int streams_set_fh     (int id, int fh) {
 struct roar_stream_server * ss;
 struct roar_stream        * s;
 int dir;
 int nonblock = 1;

 _CHECK_SID(id);

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 dir = ROAR_STREAM(ss)->dir;

 ROAR_DBG("streams_set_fh(id=%i): g_streams[id]->id=%i", id, s->id);

 s->fh = fh;

 ROAR_DBG("streams_set_fh(id=%i, fh=%i): driverID=%i", id, fh, ss->driver_id);

 if ( ss->driver_id == -1 && fh != -2 ) {
#ifndef ROAR_TARGET_WIN32
  roar_vio_set_fh(&(ss->vio), fh);
#else
  roar_vio_open_fh_socket(&(ss->vio), fh);
#endif
 }

 ROAR_DBG("streams_set_fh(id=%i, fh=%i) = ?", id, fh);

 switch (dir) {
  case ROAR_DIR_THRU:
  case ROAR_DIR_RAW_IN:
  case ROAR_DIR_RAW_OUT:
   break;
  default:
    if ( codecfilter_open(&(ss->codecfilter_inst), &(ss->codecfilter), NULL,
                     s->info.codec, ss) == -1 ) {
     streams_delete(id); // TODO: FIXME: is this correct? shoudn't we return -1 in any case here?
     return -1;
    }
   break;
 }

 ROAR_DBG("streams_set_fh(id=%i, fh=%i) = ?", id, fh);

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

 ROAR_DBG("streams_set_fh(id=%i, fh=%i) = ?", id, fh);

 if ( fh == -1 || fh == -2 ) { // yes, this is valid, indecats full vio!
  ss->ready = 1;
  ss->state = ROAR_STREAMSTATE_NEW;
  return 0;
 }

 ROAR_DBG("streams_set_fh(id=%i, fh=%i) = ?", id, fh);

// roar_socket_recvbuf(fh, ROAR_OUTPUT_CALC_OUTBUFSIZE( &(ROAR_STREAM(g_streams[id])->info) )); // set recv buffer to minimum

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

 if ( dir >= ROAR_DIR_DIRIDS )
  return -1;

 if ( g_config->streams[dir].flags & ROAR_FLAG_SYNC ) {
  switch (dir) {
   case ROAR_DIR_BRIDGE:
   case ROAR_DIR_MIDI_OUT:
    break;
   default:
     nonblock = 0;
    break;
  }
 }


 if ( !nonblock ) {
  ss->ready = 1;
  ss->state = ROAR_STREAMSTATE_NEW;

  ROAR_DBG("streams_set_fh(id=%i, fh=%i) = 0", id, fh);
  return 0;
 } else {
#ifndef ROAR_TARGET_WIN32
  if ( roar_socket_nonblock(fh, ROAR_SOCKET_NONBLOCK) == -1 )
   return -1;
#endif

  ss->ready = 1;
  ss->state = ROAR_STREAMSTATE_NEW;

  ROAR_DBG("streams_set_fh(id=%i, fh=%i) = 0", id, fh);
  return 0;
 }
}

int streams_get_fh     (int id) {
 _CHECK_SID(id);

 return ROAR_STREAM(g_streams[id])->fh;
}

int streams_set_null_io(int id) {
 struct roar_stream_server * ss;
 struct roar_stream        * s;

 _CHECK_SID(id);

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 s->fh = -1;

 return 0;
}

int streams_get    (int id, struct roar_stream_server ** stream) {
 _CHECK_SID(id);

 *stream = g_streams[id];

 return 0;
}

int streams_set_socktype (int id, int socktype) {
 _CHECK_SID(id);

 g_streams[id]->socktype = socktype;

 return 0;
}

int streams_get_socktype (int id) {
 _CHECK_SID(id);

 return g_streams[id]->socktype;
}

int streams_set_primary (int id, int prim) {
 _CHECK_SID(id);

 g_streams[id]->primary = prim;

 return 0;
}

int streams_mark_primary (int id) {
 return streams_set_primary(id, 1);
}

int streams_set_sync     (int id, int sync) {
 int fh = streams_get_fh(id);

 _CHECK_SID(id);

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

 _CHECK_SID(id);

 return roar_vio_ctl(&(g_streams[id]->vio), ROAR_VIO_CTL_SET_UMMAP, &use);
}

int streams_set_flag     (int id, int flag) {
 int parent;

 _CHECK_SID(id);

 if ( flag & ROAR_FLAG_IMMUTABLE )
  flag |= ROAR_FLAG_PRIMARY;

 if ( flag & ROAR_FLAG_MMAP )
  if ( streams_set_mmap(id, 0) == -1 )
   flag -= ROAR_FLAG_MMAP;

 if ( flag & ROAR_FLAG_PRIMARY ) {
  streams_set_primary(id, 1);
  flag -= ROAR_FLAG_PRIMARY;
 }

 if ( flag & ROAR_FLAG_VIRTUAL ) {
  if ( (parent = ROAR_STREAM(g_streams[id])->pos_rel_id) == -1 )
   return -1;

  if ( streams_ctl(parent, ROAR_CODECFILTER_CTL_VIRTUAL_NEW|ROAR_STREAM_CTL_TYPE_INT, &id) == -1 ) {
//   flag -= ROAR_FLAG_VIRTUAL;
   return -1;
  }

  if ( client_stream_move(streams_get_client(parent), id) == -1 ) {
   return -1;
  }
 }

 if ( flag & ROAR_FLAG_SYNC ) {
  switch (ROAR_STREAM(g_streams[id])->dir) {
   // for this stream types the flag is used in the subsystem:
   case ROAR_DIR_BRIDGE:
   case ROAR_DIR_MIDI_OUT:
    break;

   // normal behavor (vio blocking):
   default:
     // the fh is updated as soon as the fh get ready in case the default ask to set sync
     if ( !g_streams[id]->ready && !(g_config->streams[ROAR_STREAM(g_streams[id])->dir].flags & ROAR_FLAG_SYNC) ) {
      if ( streams_set_sync(id, 1) == -1 )
       flag -= ROAR_FLAG_SYNC;
     }
  }
 }

 if ( flag & ROAR_FLAG_HWMIXER ) { // currently not supported -> ignored
  g_streams[id]->flags |= flag;
  if ( streams_set_mixer(id) == -1 ) {
   g_streams[id]->flags -= flag;
   return -1;
  }
 }

 if ( flag & ROAR_FLAG_RECSOURCE ) {
  if ( streams_recsource_id != -1 ) {
   if ( streams_reset_flag(streams_recsource_id, ROAR_FLAG_RECSOURCE) == -1 )
    return -1;
  }

  streams_recsource_id = id;
 }

 g_streams[id]->flags |= flag;

#ifdef ROAR_SUPPORT_META
 if ( flag & ROAR_FLAG_META )
  stream_meta_finalize(id);
#endif

 return 0;
}

int streams_set_rawflag  (int id, int flag) {
 _CHECK_SID(id);

 g_streams[id]->flags |= flag;

 return 0;
}

int streams_reset_flag   (int id, int flag) {
 _CHECK_SID(id);

 if ( g_streams[id]->flags & ROAR_FLAG_IMMUTABLE ) {
  flag |= ROAR_FLAG_PRIMARY;
  flag -= ROAR_FLAG_PRIMARY;
 }

 if ( flag & ROAR_FLAG_RECSOURCE )
  if ( streams_recsource_id == id )
   streams_recsource_id = -1;

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
 _CHECK_SID(id);

 return g_streams[id]->flags & flag ? 1 : 0;
}

int streams_set_name     (int id, char * name) {
 char * str;

 _CHECK_SID(id);

 if ( (str = strdup(name)) == NULL )
  return -1;

 if ( g_streams[id]->name != NULL )
  free(g_streams[id]->name);

 g_streams[id]->name = str;

 return 0;
}

char * streams_get_name  (int id) {
 _CHECK_SID_RET(id, NULL);

 return g_streams[id]->name;
}


int streams_calc_delay    (int id) {
 struct roar_stream_server * ss;
 struct roar_stream        * s;
 register uint_least32_t d = 0;
 uint_least32_t t[1];
 uint64_t       tmp;

 _CHECK_SID(id);

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 // mixer store there value in ss->delay directly
 if ( s->dir == ROAR_DIR_MIXING )
  return 0;

 if ( ss->mixer_stream != id ) {
  if ( streams_calc_delay(ss->mixer_stream) != -1 ) {
   d += g_streams[ss->mixer_stream]->delay; // in case we can calc the delay
                                            // the stream must exist, so no check here
  }
 }

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
   ROAR_DBG("streams_calc_delay(id=%i): VIO delay in musec: %llu", id, tmp);

   d += tmp;
  }
 }

 ROAR_DBG("streams_calc_delay(id=%i): delay in musec: %i", id, d);

 ss->delay = d;

 return 0;
}

int streams_set_mixer    (int id) {
 struct roar_stream_server * ss;
 struct roar_stream_server * pmss;
 int i;
 int subsys;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( streams_get_flag(id, ROAR_FLAG_PASSMIXER) == 1 ) {
  if ( (subsys = streams_get_subsys(id)) == -1 )
   return -1;

  for (i = 0; i < ROAR_STREAMS_MAX; i++) {
   if ( (pmss = g_streams[i]) != NULL ) {
    if ( streams_get_flag(i, ROAR_FLAG_PASSMIXER) == 1 ) {
     if ( streams_get_subsys(i) == subsys ) {
      memcpy(&(pmss->mixer), &(ss->mixer), sizeof(struct roar_mixer_settings));

      // update hwmixers and the like but do not set mixer value recrusivly.
      streams_reset_flag(i, ROAR_FLAG_PASSMIXER);
      streams_set_mixer(i);
      streams_set_flag(i, ROAR_FLAG_PASSMIXER);
     }
    }
   }
  }
 }

 if ( !streams_get_flag(id, ROAR_FLAG_HWMIXER) )
  return 0;

 if ( ss->driver_id == -1 )
  return 0;

 return driver_set_volume(id, &(ss->mixer));
}

int streams_set_map      (int id, char * map, size_t len) {
 struct roar_stream_server * ss;
 int ssdir;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( map != NULL ) {
  if ( ROAR_STREAM(ss)->info.channels != len )
   return -1;
 }

 ssdir = streams_get_ssdir(id);

 switch (ssdir) {
  case STREAM_DIR_IN:
  case STREAM_DIR_NONE:
    memset(ss->chanmap.in, 0, sizeof(ss->chanmap.in));

    if ( map != NULL )
     memcpy(ss->chanmap.in, map, len);

    roardsp_chanmap_calc(&(ss->chanmap), ROARDSP_CHANMAP_MAP, 0);
   break;
  case STREAM_DIR_OUT:
    memset(ss->chanmap.out, 0, sizeof(ss->chanmap.out));

    if ( map != NULL )
     memcpy(ss->chanmap.out, map, len);

    roardsp_chanmap_calc(&(ss->chanmap), ROARDSP_CHANMAP_MAP, 0);
   break;
  default:
    return -1;
 }

 return 0;
}

int streams_ctl          (int id, int_least32_t cmd, void * data) {
 struct roar_stream_server * ss;
 int_least32_t comp;

 _CHECK_SID(id);

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
 _CHECK_SID(id);

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

 _CHECK_SID(id);

 if ( (s = ROAR_STREAM(ss = g_streams[id])) == NULL )
  return -1;

 // set up stream_info
 stream_info = &(s->info);

 // calc todo_in
 inlen = ROAR_OUTPUT_CALC_OUTBUFSIZE(stream_info);

 buflen = ROAR_OUTPUT_CALC_OUTBUFSIZE_MAX(info, stream_info);

 ROAR_DBG("streams_fill_mixbuffer2(id=%i, info=%p{...}): inlen=%lu, buflen=%lu", id, info, (unsigned long)inlen, (unsigned long)buflen);

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

 ROAR_DBG("streams_fill_mixbuffer2(id=%i, info=...): inlen_got=%u", id, inlen_got);

 if ( ss->is_new ) {
  ss->state = ROAR_STREAMSTATE_OLD;
  ROAR_INFO("streams_fill_mixbuffer2(id=%i, info=...): stream state: new->old", ROAR_DBG_INFO_VERBOSE, id);
 }

 ss->is_new = 0;

 // check channel map:
#if 0
 if ( roardsp_chanmap_mappcm(indata, indata, inlen, stream_info->channels, &(ss->chanmap), stream_info->bits) == -1 ) {
  if ( bufbuf != NULL )
   roar_buffer_free(bufbuf);
  return -1;
 }
#endif

 // check codec, bits, channels, rate...
 if ( is_the_same ) {
  if ( indata != outdata )
   memcpy(outdata, indata, inlen);

  if ( inlen < outlen )
   memset(outdata+inlen, 0, outlen-inlen);
 } else {
//  if ( roar_conv(outdata, indata, (8*inlen_got*info->rate)/(stream_info->rate * stream_info->bits), stream_info, info) == -1 ) {
  ROAR_DBG("streams_fill_mixbuffer2(*): CALL roar_conv2(*)...");
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

 if ( !streams_get_flag(id, ROAR_FLAG_HWMIXER) && !streams_get_flag(id, ROAR_FLAG_PASSMIXER) ) {
  ROAR_DBG("streams_fill_mixbuffer2(*): CALL roar_amp_pcm(*)...");
  if ( roar_amp_pcm(outdata, info->bits, outdata, 8*outlen / info->bits, info->channels, &(ss->mixer)) == -1 )
   return -1;
 }

 if ( streams_get_flag(id, ROAR_FLAG_ANTIECHO) ) {
  ROAR_DBG("streams_fill_mixbuffer2(*): Calcing antiecho...");
  // we can ignore errors here:
  if ( stream_outputbuffer_request(id, &bufbuf, buflen) == 0 ) {
   if ( roar_buffer_get_data(bufbuf, &bufdata) != -1 )
    memcpy(bufdata, outdata, outlen);
  }
 }

 s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(info, outlen)*info->channels);

 ROAR_DBG("streams_fill_mixbuffer2(*) = 0");
 return 0;
}


int streams_get_mixbuffers (void *** bufferlist, struct roar_audio_info * info, unsigned int pos) {
 static void * bufs[ROAR_STREAMS_MAX+1];
 int i;
 int have = 0;
 int dir;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   dir = streams_get_dir(i);

   switch (dir) {
    case ROAR_DIR_PLAY:
    case ROAR_DIR_BIDIR:
     break;
    case ROAR_DIR_BRIDGE:
      if ( g_streams[i]->buffer == NULL )
       continue;
     break;
    default:
      continue;
   }

   if ( streams_get_flag(i, ROAR_FLAG_PAUSE) )
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

 _CHECK_SID(id);

 if ( g_streams[id]->buffer == NULL ) {
  g_streams[id]->buffer = buf;
  ROAR_DBG("stream_add_buffer(id=%i, buf=%p) = 0", id, buf);
  return 0;
 }

 ROAR_DBG("stream_add_buffer(id=%i, buf=%p) = ?", id, buf);
 return roar_buffer_add(g_streams[id]->buffer, buf);
}

int stream_shift_out_buffer   (int id, void * data, size_t * len) {
 _CHECK_SID(id);

 if ( g_streams[id]->buffer == NULL )
  return -1;

 return roar_buffer_shift_out(&(g_streams[id]->buffer), data, len);
}

int stream_shift_buffer   (int id, struct roar_buffer ** buf) {
 struct roar_buffer * next;

 _CHECK_SID(id);

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
 _CHECK_SID(id);

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

 _CHECK_SID(id);

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

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( ss->outputbuffer != NULL ) {
  ret = roar_buffer_free(ss->outputbuffer);
  ss->outputbuffer = NULL;
  return ret;
 }

 return 0;
}

int stream_prethru_add(int id, struct roar_buffer * buf) {
 register struct roar_stream_server *  ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( ss->prethru == NULL ) {
  ss->prethru = buf;
  return 0;
 }

 if ( roar_buffer_add(ss->prethru, buf) == -1 ) {
  return -1;
 }

 return 0;
}

int stream_prethru_add_data(int id, void ** buf, size_t len) {
 struct roar_buffer * buffer;

 _CHECK_SID(id);

 if ( roar_buffer_new(&buffer, len) == -1 )
  return -1;

 if ( roar_buffer_get_data(buffer, buf) == -1 ) {
  roar_buffer_free(buffer);
  return -1;
 }

 if ( stream_prethru_add(id, buffer) == -1 ) {
  roar_buffer_free(buffer);
  return -1;
 }

 return 0;
}

int stream_prethru_destroy(int id) {
 int ret;
 register struct roar_stream_server *  ss;

 _CHECK_SID(id);

 if ( (ss = g_streams[id]) == NULL )
  return -1;

 if ( ss->prethru != NULL ) {
  ret = roar_buffer_free(ss->prethru);
  ss->prethru = NULL;
  return ret;
 }

 return 0;
}

int stream_prethru_send(int dst, int src) {
 struct roar_stream_server *  dst_ss, * src_ss;
 struct roar_buffer * bufbuf;
 void * bufdata;
 size_t buflen;

 ROAR_DBG("stream_prethru_send(dst=%i, src=%i) = ?", dst, src);

 _CHECK_SID(dst);
 _CHECK_SID(src);

 if ( (dst_ss = g_streams[dst]) == NULL )
  return -1;

 if ( (src_ss = g_streams[src]) == NULL )
  return -1;

 bufbuf = src_ss->prethru;

 ROAR_DBG("stream_prethru_send(dst=%i, src=%i): prethru buffer at %p", dst, src, bufbuf);

 while (bufbuf != NULL) {
  ROAR_DBG("stream_prethru_send(dst=%i, src=%i): looping with buffer at %p", dst, src, bufbuf);

  if ( roar_buffer_get_data(bufbuf, &bufdata) == -1 )
   return -1;

  if ( roar_buffer_get_len(bufbuf, &buflen) == -1 )
   return -1;

  if ( stream_vio_s_write(dst_ss, bufdata, buflen) != buflen )
   return -1;

  if ( roar_buffer_get_next(bufbuf, &bufbuf) == -1 )
   return -1;
 }

 ROAR_DBG("stream_prethru_send(dst=%i, src=%i) = 0", dst, src);
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

 _CHECK_SID(id);

 ROAR_DBG("streams_check(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 if ( (fh = s->fh) == -1 )
  return 0;

 if ( streams_get_flag(id, ROAR_FLAG_PAUSE) )
  return 0;

 switch (s->dir) {
  case ROAR_DIR_LIGHT_IN:
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
    return light_check_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
   break;
  case ROAR_DIR_MIDI_IN:
#ifndef ROAR_WITHOUT_DCOMP_MIDI
    return midi_check_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
   break;
  case ROAR_DIR_RAW_IN:
#ifndef ROAR_WITHOUT_DCOMP_RAW
    return raw_check_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
   break;
  case ROAR_DIR_RDTCS_IN:
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
    return rdtcs_check_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
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

/*
 ROAR_DBG("streams_check(id=%i): ROAR_OUTPUT_BUFFER_SAMPLES=%i, s->info.channels=%i, s->info.bits=%i, s->info.rat=%i, g_sa->rate=%i", id, ROAR_OUTPUT_BUFFER_SAMPLES, s->info.channels, s->info.bits, s->info.rate, g_sa->rate);
*/

 req  = ROAR_OUTPUT_CALC_OUTBUFSIZE(&(s->info)); // optimal size
// req  = ROAR_OUTPUT_BUFFER_SAMPLES * s->info.channels * (s->info.bits / 8) * ((float)s->info.rate/g_sa->rate);
 req += ss->need_extra; // bytes left we sould get....

 ROAR_DBG("streams_check(id=%i): asking for %i bytes", id, req);

 if ( roar_buffer_new(&b, req) == -1 ) {
  ROAR_ERR("streams_check(*): Can not alloc buffer space!");
  ROAR_DBG("streams_check(*) = -1");
  return -1;
 }

 roar_buffer_get_data(b, (void **)&buf);

 ROAR_DBG("streams_check(id=%i): buffer is up and ready ;)", id);
 ROAR_DBG("streams_check(id=%i): asking for %i bytes", id, req);

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

 _CHECK_SID(id);

 ROAR_DBG("streams_send_mon(id=%i) = ?", id);

 s = ROAR_STREAM((ss = g_streams[id]));

/*
 if ( (fh = s->fh) == -1 )
  return 0;
*/

 if ( !ss->ready )
  return 0;

 if ( g_config->jumbo_mtu )
  roar_vio_sync(ss->viop);

 if ( streams_get_flag(id, ROAR_FLAG_PAUSE) )
  return 0;

 switch (s->dir) {
  case ROAR_DIR_LIGHT_OUT:
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
    return light_send_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
   break;
  case ROAR_DIR_MIDI_OUT:
#ifndef ROAR_WITHOUT_DCOMP_MIDI
    return midi_send_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
   break;
  case ROAR_DIR_RDTCS_OUT:
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
    return rdtcs_send_stream(id);
#else
    streams_delete(id);
    return -1;
#endif
   break;

  case ROAR_DIR_COMPLEX_OUT:
    // send a tick:
    if ( ss->codecfilter != -1 ) {
     if ( codecfilter_write(ss->codecfilter_inst, ss->codecfilter, NULL, 0) == 0 )
      ss->state = ROAR_STREAMSTATE_OLD;
    }
    return 0;
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
  if ( stream_outputbuffer_request(id, &bufbuf, ROAR_OUTPUT_CALC_OUTBUFSIZE_MAX(&(s->info), g_sa)) == -1 )
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
  ROAR_DBG("streams_send_mon(id=%i): antiecho=%i", id, antiecho);
  if ( roar_remove_init(&removalstate) == -1 ) {
   _return(-1);
  }

  ROAR_DBG("streams_send_mon(id=%i): antiecho=%i", id, antiecho);
  if ( roar_remove_so(obuf, ip, ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels, g_sa->bits, &removalstate) == -1 ) {
   ROAR_DBG("streams_send_mon(id=%i): anti echo failed", id);
   _return(-1);
  }
  ROAR_DBG("streams_send_mon(id=%i): antiecho=%i", id, antiecho);
 }

 if ( !is_vol_eq ) {
  if ( roar_amp_pcm(obuf, g_sa->bits, ip, ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels, g_sa->channels, &(ss->mixer)) == -1 ) {
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
   ROAR_DBG("streams_send_mon(id=%i) = 0", id);
   _return(0);
  }

  ROAR_DBG("streams_send_mon(id=%i) = ?", id);

  if ( (ret = stream_vio_s_write(ss, obuf, olen)) == olen ) {
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), olen)*s->info.channels);
   ss->state = ROAR_STREAMSTATE_OLD;
   ROAR_DBG("streams_send_mon(id=%i) = 0", id);
   _return(0);
  }

  ROAR_DBG("streams_send_mon(id=%i) = ?", id);

  if ( ret > 0 && errno == 0 ) {
   ROAR_WARN("streams_send_mon(id=%i): Overrun in stream: wrote %i of %i bytes, %i bytes missing", id, (int)ret, olen, olen-(int)ret);
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), ret)*s->info.channels);
   ss->state = ROAR_STREAMSTATE_OLD;
   _return(0);
  }
 } else {
  errno = 0;
  if ( codecfilter_write(ss->codecfilter_inst, ss->codecfilter, obuf, olen)
            == olen ) {
   s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, ROAR_OUTPUT_CALC_OUTBUFSAMP(&(s->info), olen)*s->info.channels);
   ss->state = ROAR_STREAMSTATE_OLD;
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
   ss->state = ROAR_STREAMSTATE_OLD;
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

 _CHECK_SID(id);

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
 _CHECK_SID(stream);

 return stream_vio_s_read(g_streams[stream], buf, count);
}

ssize_t stream_vio_write(int stream, void *buf, size_t count) {
 _CHECK_SID(stream);

 return stream_vio_s_write(g_streams[stream], buf, count);
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

 if ( streams_thru_num ) {
  for (i = 0; i < ROAR_STREAMS_MAX; i++) {
   if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == ROAR_STREAM(stream)->id ) {
    if ( ROAR_STREAM(g_streams[i])->dir == ROAR_DIR_THRU ) {
     if ( g_streams[i]->ready ) {
      if ( stream_vio_write(i, orig_buf, len) != len )
       streams_delete(i);

      if ( g_streams[i] != NULL )
       g_streams[i]->state = ROAR_STREAMSTATE_OLD;
     }
    }
   }
  }
 }

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

 if ( streams_thru_num ) {
  for (i = 0; i < ROAR_STREAMS_MAX; i++) {
   if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == ROAR_STREAM(stream)->id ) {
    if ( ROAR_STREAM(g_streams[i])->dir == ROAR_DIR_THRU ) {
     if ( g_streams[i]->ready ) {
      if ( g_streams[i]->state == ROAR_STREAMSTATE_NEW ) {
       if ( streams_get_flag(i, ROAR_FLAG_PRETHRU) == 1 ) {
        if ( stream_prethru_send(i, ROAR_STREAM(stream)->id) == -1 ) {
         streams_delete(i);
        }
       }
      }

      if ( stream_vio_write(i, buf, count) != count ) {
       streams_delete(i);
      }

      if ( g_streams[i] != NULL )
       g_streams[i]->state = ROAR_STREAMSTATE_OLD;
     }
    }
   }
  }
 }

 if ( g_config->jumbo_mtu ) {
  if ( stream->viop != &(stream->jumbo) ) {
   if ( roar_vio_open_jumbo(&(stream->jumbo), &(stream->vio), g_config->jumbo_mtu) != -1 ) {
    // if that works we continue using the jumbo vio,
    // in case it didn't we dont, just use normal VIO.
    stream->viop = &(stream->jumbo);
   }
  }
 }

 return roar_vio_write(stream->viop, buf, count);
}

//ll
