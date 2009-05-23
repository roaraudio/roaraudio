//midi.c:

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

#if defined(ROAR_HAVE_IO_POSIX) && !defined(ROAR_TARGET_WIN32)
#define _HAVE_CONSOLE
#endif

int midi_init (void) {

 if ( midi_cb_init() == -1 ) {
  ROAR_WARN("Can not initialize MIDI subsystem component CB");
 }

 if ( midi_clock_init() == -1 ) {
  ROAR_WARN("Can not initialize MIDI subsystem component clock");
 }

 g_midi_mess.buf = NULL;

 return 0;
}

int midi_free (void) {
 return midi_cb_free();
}

int midi_update(void) {

 if ( g_midi_clock.stream != -1 )
  midi_check_bridge(g_midi_clock.stream);

 return midi_cb_update();
}

int midi_reinit(void) {

 if ( g_midi_mess.buf != NULL )
  roar_buffer_free(g_midi_mess.buf);

 g_midi_mess.buf = NULL;

 return 0;
}

// STREAMS:

int midi_check_stream  (int id) {
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_buffer        *   b;
 char                      * buf;
 ssize_t                     len;
 int                           i;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("midi_check_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 if ( s->dir == ROAR_DIR_BRIDGE )
  return midi_check_bridge(id);

 switch (s->info.codec) {
  case ROAR_CODEC_MIDI:
   break;
  default:
    streams_delete(id);
    return -1;
 }

 if ( roar_buffer_new(&b, MIDI_READ_SIZE) == -1 ) {
  ROAR_ERR("midi_check_stream(*): Can not alloc buffer space!");
  ROAR_DBG("midi_check_stream(*) = -1");
  return -1;
 }

 roar_buffer_get_data(b, (void **)&buf);

 if ( (len = stream_vio_s_read(ss, buf, MIDI_READ_SIZE)) < 1 ) {
  streams_delete(id);
  roar_buffer_free(b);
  return -1;
 }

 roar_buffer_set_len(b, len);

 if ( stream_add_buffer(id, b) == -1 ) {
  roar_buffer_free(b);
  streams_delete(id);
  return -1;
 }

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL && ROAR_STREAM(g_streams[i])->pos_rel_id == id ) {
   if ( ROAR_STREAM(g_streams[i])->dir == ROAR_DIR_THRU ) {
    if ( stream_vio_write(i, buf, len) != len ) {
     streams_delete(i);
    }
   }
  }
 }

 switch (s->info.codec) {
  case ROAR_CODEC_MIDI:
    return midi_conv_midi2mes(id);
   break;
  default:
    streams_delete(id);
    return -1;
 }

 return 0;
}

int midi_send_stream   (int id) {
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("midi_send_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 switch (s->info.codec) {
  default:
    streams_delete(id);
    return -1;
 }

 return 0;
}

int midi_conv_midi2mes (int id) {
 unsigned char * data;
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_buffer        * buf = NULL;
 struct midi_message       * mes = NULL;
 size_t need = 0, have = 0;
 int alive = 1;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("midi_conv_midi2mes(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 while (ss->buffer != NULL && alive) {
  if ( roar_buffer_get_data(ss->buffer, (void**)&data) == -1 ) {
   alive = 0;
   continue;
  }

  if ( roar_buffer_get_len(ss->buffer, &have) == -1 ) {
   alive = 0;
   continue;
  }

  while (have && alive) {
   printf("%.2x=", *data);
   if ( *data & 0x80 ) {
    if ( buf != NULL ) {
     midi_add_buf(id, buf);
     buf = NULL;
    }

    need = 0;
    printf("S\n");
    if ( midi_new_bufmes(&buf, &mes) == -1 ) {
     alive = 0;
     continue;
    }

    if (*data == MIDI_TYPE_CLOCK_TICK || *data == MIDI_TYPE_CLOCK_START || *data == MIDI_TYPE_CLOCK_STOP ) {
     mes->type = *data;
    } else {
     mes->type    = *data & 0xF0;
     mes->channel = *data & 0x0F;
     switch (*data & 0xF0) {
      case MIDI_TYPE_NOTE_ON:
      case MIDI_TYPE_NOTE_OFF:
        need = 2;
       break;
      case MIDI_TYPE_PA:
        need = 2;
       break;
      case MIDI_TYPE_CONTROLER:
        need = 2;
       break;
      case MIDI_TYPE_PROGRAM:
        need = 1;
       break;
      case MIDI_TYPE_MA:
        need = 1;
       break;
      case MIDI_TYPE_PB:
      case MIDI_TYPE_SYSEX:
        need = 1;
       break;
     }
    }

   } else {
    printf("D\n");

    if ( mes == NULL ) {
     ROAR_WARN("midi_conv_midi2mes(id=%i): Lost sync.", id);
     data++;
     have--;
     continue;
    }

    switch (mes->type) {
      case MIDI_TYPE_NOTE_ON:
      case MIDI_TYPE_NOTE_OFF:
      case MIDI_TYPE_PA:
      case MIDI_TYPE_CONTROLER:
        if ( need == 2 ) {
         mes->kk = *data;
        } else {
         mes->vv = *data;
        }
       break;
      case MIDI_TYPE_PROGRAM:
      case MIDI_TYPE_MA:
        mes->vv = *data;
       break;
      case MIDI_TYPE_PB:
      case MIDI_TYPE_SYSEX:
        ROAR_WARN("midi_conv_midi2mes(id=%i): Message of Type 0x%.2X (PB or SysEx) not yet supported", id, mes->type);
       break;
    }

    if ( need )
     need--;
   }
   data++;
   have--;
  }

  if ( need ) {
   ROAR_ERR("midi_conv_midi2mes(*): FIXME: BUG!!! Need to restore buffer here with corrected length");
  } else if ( !have ) {
   roar_buffer_next(&(ss->buffer));
  }

  if ( need && buf != NULL ) {
   roar_buffer_free(buf);
   buf = NULL;
  } else if ( buf != NULL ) {
   midi_add_buf(id, buf);
   buf = NULL;
  }
 }

 return 0;
}

int midi_conv_mes2midi (int id) {
 return -1;
}

int midi_new_bufmes    (struct roar_buffer ** buf, struct midi_message ** mes) {
 if ( buf == NULL || mes == NULL )
  return -1;

 *buf = (void*)(*mes = NULL);

 if ( roar_buffer_new(buf, sizeof(struct midi_message)) == -1 )
  return -1;

 if ( roar_buffer_get_data(*buf, (void**)mes) == -1 ) {
  roar_buffer_free(*buf);
  *buf = (void*)(*mes = NULL);
  return -1;
 }

 memset((void*)*mes, 0, sizeof(struct midi_message));

 (*mes)->type = MIDI_TYPE_NONE;

 return 0;
}

int midi_add_buf       (int id, struct roar_buffer * buf) {
 struct midi_message * mes;

 if ( id == -1 || buf == NULL )
  return -1;

 if ( roar_buffer_get_data(buf, (void**)&mes) == -1 )
  return -1;

 ROAR_WARN("midi_add_buf(id=%i, buf=%p) = ?", id, buf);
 ROAR_WARN("midi_add_buf(*): MIDI Message of Type 0x%.2X", mes->type);
 ROAR_WARN("midi_add_buf(*): Channel: %i", mes->channel);
 ROAR_WARN("midi_add_buf(*): flags=0x%.2X", mes->flags);
 ROAR_WARN("midi_add_buf(*): kk=0x%.2X, vv=0x%.2X", mes->kk, mes->vv);

 if ( g_midi_mess.buf == NULL ) {
  g_midi_mess.buf = buf;
 } else {
  roar_buffer_add(g_midi_mess.buf, buf);
 }

 return 0;
}

// bridges:
int midi_check_bridge  (int id) {

 ROAR_DBG("midi_check_bridge(id=%i) = ?", id);

 if ( id == g_midi_clock.stream ) {
  midi_clock_tick();

  return 0;
 }

 return -1;
}

// clock:

int midi_clock_init (void) {
 struct roar_stream * s;
 struct roar_stream_server * ss;

 if ( (g_midi_clock.stream = streams_new()) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component clock");
  return -1;
 }

 midi_vio_set_dummy(g_midi_clock.stream);

 streams_get(g_midi_clock.stream, &ss);
 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->pos_rel_id    =  g_midi_clock.stream;

 s->info.codec    =  ROAR_CODEC_MIDI;
 ss->codec_orgi   =  ROAR_CODEC_MIDI;

 s->info.channels =  0;
 s->info.rate     = MIDI_RATE;
 s->info.bits     =  8;

 if ( streams_set_dir(g_midi_clock.stream, ROAR_DIR_BRIDGE, 1) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component clock");
  return -1;
 }

 streams_set_name(g_midi_clock.stream, "MIDI Clock");

 streams_set_flag(g_midi_clock.stream, ROAR_FLAG_PRIMARY);
 streams_set_flag(g_midi_clock.stream, ROAR_FLAG_SYNC);

 midi_clock_set_bph(3600); // one tick per sec

 return 0;
}

int midi_clock_set_bph (uint_least32_t bph) {
 uint_least32_t sph = g_sa->rate/2 * 75 * g_sa->channels; // samples per houre

 g_midi_clock.bph  = bph;

 g_midi_clock.spt  = sph/bph;

 g_midi_clock.nt   = ROAR_MATH_OVERFLOW_ADD(g_pos, g_midi_clock.spt);

 return 0;
}

int midi_clock_tick (void) {
 unsigned int diff;

 while ( g_pos >= g_midi_clock.nt ) {
  diff = g_pos - g_midi_clock.nt;
  ROAR_DBG("midi_clock_tick(void): g_pos is %u samples (%5.2f%%) ahead of nt.", diff, (float)diff/g_midi_clock.spt);

  g_midi_clock.nt   = ROAR_MATH_OVERFLOW_ADD(g_midi_clock.nt, g_midi_clock.spt);

  if ( streams_get_flag(g_midi_clock.stream, ROAR_FLAG_SYNC) ) {
   ROAR_DBG("midi_clock_tick(void): TICK! (nt=%lu)", g_midi_clock.nt);
  } else {
   ROAR_DBG("midi_clock_tick(void): silent tick. (nt=%lu)", g_midi_clock.nt);
  }
 }

 return 0;
}

// CB:

int midi_cb_init (void) {
#ifdef _HAVE_CONSOLE
 struct roar_stream * s;
 struct roar_stream_server * ss;
 int i;
 char * files[] = {
                   "/dev/console",
#ifdef __linux__
                   "/dev/tty0",
                   "/dev/vc/0",
#endif
                   NULL
                  };

 g_midi_cb.console  = -1;
 g_midi_cb.stream   = -1;
 g_midi_cb.stoptime =  0;
 g_midi_cb.playing  =  0;

 for (i = 0; files[i] != NULL; i++) {
  if ( (g_midi_cb.console = open(files[i], O_WRONLY|O_NOCTTY, 0)) != -1 )
   break;
 }

 if ( g_midi_cb.console == -1 )
  return -1;

 if ( (g_midi_cb.stream = streams_new()) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component CB");
  midi_cb_free();
  return -1;
 }

 midi_vio_set_dummy(g_midi_cb.stream);

 streams_get(g_midi_cb.stream, &ss);
 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->pos_rel_id    = -1;

 s->info.codec    =  0;
 ss->codec_orgi   =  0;

 s->info.channels =  1;
 s->info.rate     = 1193180;
 s->info.bits     =  8;

 if ( streams_set_dir(g_midi_cb.stream, ROAR_DIR_BRIDGE, 1) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component CB");
  midi_cb_free();
  return -1;
 }

 streams_set_name(g_midi_cb.stream, "Console speaker bridge");

 streams_set_flag(g_midi_cb.stream, ROAR_FLAG_OUTPUT);
 streams_set_flag(g_midi_cb.stream, ROAR_FLAG_PRIMARY);
 streams_set_flag(g_midi_cb.stream, ROAR_FLAG_HWMIXER);

 return 0;
#else
 g_midi_cb.console  = -1;
 g_midi_cb.stream   = -1;

 return -1;
#endif
}

int midi_cb_free (void) {
#ifdef _HAVE_CONSOLE

 midi_cb_stop();

 if ( g_midi_cb.stream != -1 )
  streams_delete(g_midi_cb.stream);

 if ( g_midi_cb.console != -1 )
  close(g_midi_cb.console);

 return 0;
#else
 return -1;
#endif
}

int midi_cb_play(float t, float freq, int override) {
 float samples_per_sec /* S/s */ = g_sa->rate * g_sa->channels;

/*
#define MIDI_CB_NOOVERRIDE 0
#define MIDI_CB_OVERRIDE   1
*/
 if ( g_midi_cb.playing && override != MIDI_CB_OVERRIDE )
  return -1;

 g_midi_cb.stoptime = ROAR_MATH_OVERFLOW_ADD(g_pos, samples_per_sec*t);
 midi_cb_start(freq);
 g_midi_cb.playing = 1;

 return 0;
}

int midi_cb_update (void) {
 if ( !g_midi_cb.playing )
  return 0;

 if ( g_midi_cb.stoptime <= g_pos )
  midi_cb_stop();

 return 0;
}

int midi_cb_start(float freq) {
// On linux this uses ioctl KIOCSOUND
#ifdef __linux__
 if ( g_midi_cb.console == -1 )
  return -1;

 if ( ioctl(g_midi_cb.console, KIOCSOUND, freq == 0 ? 0 : (int)(1193180.0/freq)) == -1 )
  return -1;

 return 0;
#else
 return -1;
#endif
}

int midi_cb_stop (void) {
#ifdef __linux__
 g_midi_cb.playing = 0;
 return midi_cb_start(0);
#else
 return -1;
#endif
}

// VIO:

int     midi_vio_set_dummy(int stream) {
 struct roar_stream_server * ss;

 if ( streams_get(stream, &ss) == -1 )
  return -1;

 ss->vio.read     = NULL;
 ss->vio.write    = NULL;
 ss->vio.lseek    = NULL;
 ss->vio.nonblock = (int (*)(struct roar_vio_calls * vio, int state))midi_vio_ok;
 ss->vio.sync     = (int (*)(struct roar_vio_calls * vio))midi_vio_ok;
 ss->vio.ctl      = NULL;
 ss->vio.close    = (int (*)(struct roar_vio_calls * vio))midi_vio_ok;

 return 0;
}

int     midi_vio_ok(struct roar_vio_calls * vio, ...) {
 return 0;
}

//ll
