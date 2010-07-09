//meta.c:

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

#ifdef ROAR_SUPPORT_META

int stream_meta_set   (int id, int type, char * name, char * val) {
 int i;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++)
  if ( s->meta[i].type == type ) {
   s->meta[i].type = ROAR_META_TYPE_NONE;
   if ( s->meta[i].value )
    free(s->meta[i].value);
   s->meta[i].value = NULL;
  }

 return stream_meta_add(id, type, name, val);
}

int stream_meta_add   (int id, int type, char * name, char * val) {
 int i;
 char * c;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  if ( s->meta[i].type == ROAR_META_TYPE_NONE ) {
   s->meta[i].type = type;

   if ( name == NULL ) {
    s->meta[i].key[0] = 0;
   } else {
    strncpy(s->meta[i].key, name, ROAR_META_MAX_NAMELEN);
   }

   if ( (c = strdup(val)) == NULL ) {
    s->meta[i].type = ROAR_META_TYPE_NONE;
    s->meta[i].key[0] = 0;
    return -1;
   }

   s->meta[i].value = c;

   ROAR_DBG("stream_meta_add(id=%i, type=%i, name='%s', val='%s') = 0", id, type, name, val);

   return 0;
  }
 }

 return -1;
}

int stream_meta_get   (int id, int type, char * name, char * val, size_t len) {
 int i, vallen;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  if ( s->meta[i].type == type ) {
   if ( name != NULL )
    if ( strncmp(s->meta[i].key, name, ROAR_META_MAX_NAMELEN) != 0 )
     continue;

   if ( s->meta[i].value == NULL )
    return -1;

   if ( (vallen = strlen(s->meta[i].value)) > (len - 1) ) {
    ROAR_DBG("stream_meta_get(*): val too small: need %i have %i", vallen, len);
    return -1;
   }

   strncpy(val, s->meta[i].value, vallen);
   val[vallen] = 0;

   return 0;
  }
 }

 return -1;
}

int stream_meta_list  (int id, int * types, size_t len) {
 int i, j;
 int have = 0;
 int found;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 if ( len < ROAR_META_MAX_PER_STREAM ) // TODO: find a way we do not need this
  return -1;

 types[0] = -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  found = 0;
  for (j = 0; j < have; j++)
   if ( types[j] == s->meta[i].type ) {
    found = 1;
    break;
   }

  if ( !found )
   types[have++] = s->meta[i].type;
 }

 return have;
}

int stream_meta_clear (int id) {
 int i;
 struct roar_stream_server * s = NULL;

 if ( id < 0 || id > ROAR_STREAMS_MAX ) {
  ROAR_ERR("stream_meta_clear(id=%i): Can not clear meta data on stream: invalid stream ID", id);
  return -1;
 }

 s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  s->meta[i].type   = ROAR_META_TYPE_NONE;
  if ( s->meta[i].value )
   free(s->meta[i].value);
  s->meta[i].value  = NULL;
  s->meta[i].key[0] = 0;
 }

 return 0;
}

int stream_meta_finalize(int id) {
 register int dir;
 register int co, ci, i;
 struct roar_stream_server * s;

 if ( streams_get_flag(id, ROAR_FLAG_META) != 1 ) // ignore non meta streams
  return 0;

 dir = ROAR_STREAM(g_streams[id])->dir;

 if ( dir != ROAR_DIR_PLAY   && dir != ROAR_DIR_META &&  // ignore on non input streams
      dir != ROAR_DIR_FILTER && dir != ROAR_DIR_BIDIR )
  return 0;

 ROAR_DBG("stream_meta_finalize(id=%i) = ?", id);

 for (co = 0; co < ROAR_STREAMS_MAX; co++) {
  if ( g_streams[co] == NULL )
   continue;

  dir = ROAR_STREAM(g_streams[co])->dir;

  if ( dir != ROAR_DIR_MONITOR && dir != ROAR_DIR_FILTER &&
       dir != ROAR_DIR_META    && dir != ROAR_DIR_BIDIR  &&
       dir != ROAR_DIR_OUTPUT                             )
   continue;

  if ( streams_get_flag(co, ROAR_FLAG_META) != 1 )
   continue;

  ROAR_DBG("stream_meta_finalize(id=%i): found output stream: id=%i", id, co);
  stream_meta_clear(co);

  for (ci = 0; ci < ROAR_STREAMS_MAX; ci++) {
   if ( g_streams[ci] == NULL )
    continue;

   dir = ROAR_STREAM(g_streams[ci])->dir;

   if ( dir != ROAR_DIR_PLAY   && dir != ROAR_DIR_META &&
        dir != ROAR_DIR_FILTER && dir != ROAR_DIR_BIDIR )
    continue;

   if ( streams_get_flag(ci, ROAR_FLAG_META) != 1 )
    continue;

   ROAR_DBG("stream_meta_finalize(id=%i): found input stream: id=%i", id, ci);

   // ok, next we copy the date of ci to co:
   s = g_streams[ci];

   for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
    if ( s->meta[i].type == ROAR_META_TYPE_NONE )
     continue;

    ROAR_DBG("stream_meta_finalize(id=%i): found meta data, copy: %i->%i", id, ci, co);
    stream_meta_add(co, s->meta[i].type, s->meta[i].key, s->meta[i].value); // ignore errors
   }
  }

  // ask the codec filter to update meta data:
  ROAR_DBG("stream_meta_finalize(id=%i): Asking stream %i to update meta data", id, co);
  streams_ctl(co, ROAR_CODECFILTER_CTL_META_UPDATE, NULL); // ignore errors...
 }

 return 0;
}

#endif

//ll
