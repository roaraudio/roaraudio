//light.c:

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

int light_init  (unsigned int channels) {

 g_light_state.channels = 0;

 if ( (g_light_state.state = malloc(channels)) == NULL ) {
  return -1;
 }

 g_light_state.channels = channels;

 return light_reset();
}

int light_free  (void) {
 if ( g_light_state.state != NULL ) {
  free(g_light_state.state);
 }

 g_light_state.channels = 0;

 return 0;
}

int light_reset (void) {
 if ( g_light_state.channels == 0 )
  return 0;

 if ( g_light_state.state == NULL )
  return -1;

 memset(g_light_state.state, 0, g_light_state.channels);

 return 0;
}

int light_update(void) {
 return 0;
}

int light_check_stream  (int id) {
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 char buf[512];
 int i;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("light_check_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 switch (s->info.codec) {
  case ROAR_CODEC_DMX512:
    if ( stream_vio_s_read(ss, buf, 512) != 512 ) {
     streams_delete(id);
     return -1;
    }

    memcpy(g_light_state.state, buf, g_light_state.channels < 512 ? g_light_state.channels : 512);

    for (i = 0; i < ROAR_STREAMS_MAX; i++) {
     if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == id ) {
      if ( ROAR_STREAM(g_streams[i])->dir == ROAR_DIR_THRU ) {
       if ( stream_vio_write(i, buf, 512) != 512 ) {
        streams_delete(i);
       }
      }
     }
    }

    return 0;
   break;
  default:
    streams_delete(id);
    return -1;
 }

 return 0;
}

int light_send_stream   (int id) {
 int chans;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 char buf[512];

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("light_send_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 switch (s->info.codec) {
  case ROAR_CODEC_DMX512:
    chans = g_light_state.channels;

    if ( chans > 512 )
     chans = 512;

    if ( stream_vio_s_write(ss, g_light_state.state, chans) != chans ) {
     streams_delete(id);
     return -1;
    }

    if ( chans < 512 ) {
     chans = 512 - chans;
     memset(buf, 0, chans);
     if ( stream_vio_s_write(ss, g_light_state.state, chans) != chans ) {
      streams_delete(id);
      return -1;
     }
    }

    return 0;
   break;
  default:
    streams_delete(id);
    return -1;
 }

 return 0;
}

//ll
