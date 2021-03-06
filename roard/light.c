//light.c:

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

#ifndef ROAR_WITHOUT_DCOMP_LIGHT

int light_init  (unsigned int channels) {
 struct roar_stream_server * ss;
 int i;

 g_light_state.channels = 0;

 if ( channels == 0 || channels > (512*512) ) /* unrealstic values */
  return -1;

 if ( (g_light_state.state = malloc(channels)) == NULL ) {
  return -1;
 }

 if ( (g_light_state.changes = malloc(channels)) == NULL ) {
  free(g_light_state.state);
  return -1;
 }

 g_light_state.channels = channels;

 if ( (g_light_mixer.stream = add_mixer(ROAR_SUBSYS_LIGHT, _MIXER_NAME("Light Control"), &ss)) == -1 ) {
  free(g_light_state.state);
  return -1;
 }

 ROAR_STREAM(ss)->info.codec = ROAR_CODEC_DMX512;
 ROAR_STREAM(ss)->info.bits  = ROAR_LIGHT_BITS;
 ROAR_STREAM(ss)->info.rate  = ROAR_OUTPUT_CFREQ;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   if ( streams_get_subsys(i) == ROAR_SUBSYS_LIGHT ) {
    streams_set_mixer_stream(i, g_light_mixer.stream);
   }
  }
 }

 return light_reset();
}

int light_free  (void) {
 if ( g_light_state.state != NULL ) {
  free(g_light_state.state);
 }

 if ( g_light_state.changes != NULL ) {
  free(g_light_state.changes);
 }

 g_light_state.channels = 0;

 return 0;
}

int light_reset (void) {
 if ( g_light_state.channels == 0 )
  return 0;

 if ( g_light_state.state == NULL )
  return -1;

 if ( g_light_state.changes == NULL )
  return -1;

 memset(g_light_state.state,   0, g_light_state.channels);
 memset(g_light_state.changes, 0, g_light_state.channels);

 return 0;
}

int light_reinit(void) {
 if ( g_light_state.changes == NULL )
  return -1;

 memset(g_light_state.changes, 0, g_light_state.channels);

 return 0;
}

int light_update(void) {
 return 0;
}

int light_check_stream  (int id) {
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_roardmx_message  mes;
 char buf[512];
 int i, c;
 uint16_t      channel;
 unsigned char value;

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

    for (i = 0; i < (g_light_state.channels < 512 ? g_light_state.channels : 512); i++) {
     g_light_state.changes[i] |= g_light_state.state[i] ^ buf[i];
     g_light_state.state[i]    =                          buf[i];
    }
//    memcpy(g_light_state.state, buf, g_light_state.channels < 512 ? g_light_state.channels : 512);

    s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, 1);

    return 0;
   break;
  case ROAR_CODEC_ROARDMX:
    ROAR_DBG("light_check_stream(id=%i): Codec: RoarDMX", id);
    if ( roar_roardmx_message_recv(&mes, &(ss->vio)) == -1 ) {
     streams_delete(id); // because we don't know at the moment...
     return -1;
    }

    // we ignore errors here at the moment as 0 not < -1
    c = roar_roardmx_message_numchannels(&mes);
    ROAR_DBG("light_check_stream(id=%i): Number of subframes: %u", id, c);

    for (i = 0; i < c; i++) {
     if ( roar_roardmx_message_get_chanval(&mes, &channel, &value, i) == -1 )
      return -1;

     if ( g_light_state.channels < channel ) {
      ROAR_WARN("light_check_stream(id=%i): Writing on non extisting DMX channel %u", id, channel);
      continue;
     } else {
      g_light_state.state[channel]   = value;
      g_light_state.changes[channel] = 0xFF; // the channel changed
     }
    }
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
 unsigned char buf[512];
 register unsigned char * bufptr;
 struct roar_roardmx_message  mes;
 int i;
 int have_message = 0;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("light_send_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 switch (s->info.codec) {
  case ROAR_CODEC_DMX512:
    chans = g_light_state.channels;

    if ( chans > 512 )
     chans = 512;

    if ( chans == 512 ) {
     bufptr = g_light_state.state;
    } else {
     memset(buf, 0, 512);
     memcpy(buf, g_light_state.state, chans);
     bufptr = buf;
    }

    if ( stream_vio_s_write(ss, bufptr, 512) != 512 ) {
     streams_delete(id);
     return -1;
    }

    s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, 1);

    return 0;
   break;
  case ROAR_CODEC_ROARDMX:
    for (i = 0; i < g_light_state.channels; i++) {
     if ( g_light_state.changes[i] ) {
      if ( !have_message )
       if ( roar_roardmx_message_new_sset(&mes) == -1 )
        return -1;

      have_message = 2;

      if ( roar_roardmx_message_add_chanval(&mes, i, g_light_state.state[i]) == -1 ) {
       if ( roar_roardmx_message_send(&mes, &(ss->vio)) == -1 )
        return -1;

       if ( roar_roardmx_message_new_sset(&mes) == -1 )
        return -1;

       have_message = 1;
      }
     }
    }

    if ( have_message == 2 ) {
     if ( roar_roardmx_message_send(&mes, &(ss->vio)) == -1 )
      return -1;
    }
    return 0;
   break;
  default:
    streams_delete(id);
    return -1;
 }

 return 0;
}

#endif

//ll
