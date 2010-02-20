//midi.c:

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

#ifndef ROAR_WITHOUT_DCOMP_MIDI

#if defined(ROAR_HAVE_IO_POSIX) && !defined(ROAR_TARGET_WIN32)
#define _HAVE_CONSOLE
#endif

int midi_init_config(void) {
 midi_config.init        = 1;
 midi_config.inited      = 0;

#ifndef ROAR_WITHOUT_DCOMP_CB
 midi_config.init_cb     = 0;
 midi_config.console_dev = NULL;
#endif

 return 0;
}

int midi_init (void) {
 struct roar_stream_server * ss;
 int i;

 midi_config.inited = 0;

 if ( midi_config.init ) {
#ifndef ROAR_WITHOUT_DCOMP_CB
  if ( midi_config.init_cb ) {
   if ( midi_cb_init() == -1 ) {
    ROAR_WARN("Can not initialize MIDI subsystem component CB");
   }
  }
#endif

  if ( midi_clock_init() == -1 ) {
   ROAR_WARN("Can not initialize MIDI subsystem component clock");
  }

 }

 g_midi_mess.buf = NULL;

 if ( (g_midi_mixer.stream = add_mixer(ROAR_SUBSYS_MIDI, _MIXER_NAME("MIDI"), &ss)) == -1 ) {
  ROAR_WARN("Can not create MIDI mixer");
 }

 roardsp_chanlist_init(ss->chanmap.in,   16, ROARDSP_CHANLIST_MAP_MIDI);
 memcpy(ss->chanmap.out, ss->chanmap.in, sizeof(ss->chanmap.out));
 streams_set_map(g_midi_mixer.stream, NULL, 0);

 ss->state = ROAR_STREAMSTATE_OLD;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   if ( streams_get_subsys(i) == ROAR_SUBSYS_MIDI ) {
    streams_set_mixer_stream(i, g_midi_mixer.stream);
   }
  }
 }

 midi_config.inited |= MIDI_INITED_MAIN;

 return 0;
}

int midi_free (void) {
 if ( midi_reinit() == -1 )
  return -1;

#ifndef ROAR_WITHOUT_DCOMP_CB
 if ( midi_cb_free() == -1 )
  return -1;
#endif

 return 0;
}

int midi_update(void) {

 if ( g_midi_clock.stream != -1 )
  midi_check_bridge(g_midi_clock.stream);

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 midi_conv_mes2ssynth();
#endif

#ifndef ROAR_WITHOUT_DCOMP_CB
 return midi_cb_update();
#else
 return 0;
#endif
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
  case ROAR_CODEC_MIDI:
    return midi_conv_mes2midi(id);
   break;
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
 size_t need = 0, have = 0, last_have = 0, old_have = 0;
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
  old_have = have;

  while (have && alive) {
   if ( *data & 0x80 ) {
    if ( buf != NULL ) {
     midi_add_buf(id, buf);
     buf = NULL;
    }

    last_have = have;

    need = 0;
    if ( midi_new_bufmes(&buf, &mes) == -1 ) {
     alive = 0;
     continue;
    }

    if (*data == MIDI_TYPE_CLOCK_TICK || *data == MIDI_TYPE_CLOCK_START || *data == MIDI_TYPE_CLOCK_STOP ) {
     mes->type = *data;

     if ( *data == MIDI_TYPE_CLOCK_TICK ) {
      s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, 1);
     }
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

    if ( mes == NULL ) {
     ROAR_WARN("midi_conv_midi2mes(id=%i): Lost sync.", id);
     data++;
     have--;
     continue;
    }

    switch (mes->type) {
      case MIDI_TYPE_NOTE_ON:
      case MIDI_TYPE_NOTE_OFF:
       if ( need == 2 ) {
        roar_midi_note_from_midiid(&(mes->d.note), *data);

        // if velocity is zero we have a NOTE OFF event
        if ( data[1] == 0 )
         mes->type = MIDI_TYPE_NOTE_OFF;
       }
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

    if ( !need ) {
     switch (mes->type) {
      case MIDI_TYPE_CONTROLER:
       switch (mes->kk) {
        case MIDI_CCE_MAIN_VOL:
         if ( 516 * mes->vv > 65100 ) { // max volume
          ss->mixer.mixer[mes->channel] = 65535;
         } else {
          ss->mixer.mixer[mes->channel] = 516 * mes->vv;
         }
         break;
       }
      break;
     }
    }

   }
   data++;
   have--;
  }

  if ( need ) {
   if ( roar_buffer_set_offset(ss->buffer, old_have - last_have) == -1 ) {
    ROAR_ERR("midi_conv_midi2mes(*): FIXME: BUG!!! Need to restore buffer here with corrected length");
   }
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

#define _nb  len++; *(d++)
int midi_conv_mes2midi (int id) {
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;
 struct roar_buffer        * buf = g_midi_mess.buf;
 struct midi_message       * mes = NULL;
 unsigned char               data[3];
 unsigned char             * d;
 int                         len;
 int                         send_clock;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("midi_conv_mes2midi(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 send_clock = streams_get_flag(id, ROAR_FLAG_SYNC);

 while (buf != NULL) {
  if ( roar_buffer_get_data(buf, (void**)&mes) == -1 ) {
   return -1;
  }

  if (mes->type == MIDI_TYPE_CLOCK_TICK || mes->type == MIDI_TYPE_CLOCK_START || mes->type == MIDI_TYPE_CLOCK_STOP ) {
   if ( send_clock ) {
    if ( stream_vio_s_write(ss, &(mes->type), 1) != 1 ) {
     streams_delete(id);
     return -1;
    }
   }

   if ( mes->type == MIDI_TYPE_CLOCK_TICK ) {
    s->pos = ROAR_MATH_OVERFLOW_ADD(s->pos, 1);
   }
  } else {
   len = 0;
   d   = data;

   _nb = (mes->type & 0xF0) | (mes->channel & 0x0F);

   switch (mes->type) {
    case MIDI_TYPE_CONTROLER:
      switch (mes->kk) {
       case MIDI_CCE_MAIN_VOL:
         if ( 516 * mes->vv > 65100 ) { // max volume
          ss->mixer.mixer[mes->channel] = 65535;
         } else {
          ss->mixer.mixer[mes->channel] = 516 * mes->vv;
         }
        break;
      }
    case MIDI_TYPE_NOTE_ON:
    case MIDI_TYPE_NOTE_OFF:
    case MIDI_TYPE_PA:
      _nb = mes->kk;
      _nb = mes->vv;
     break;
    case MIDI_TYPE_PROGRAM:
    case MIDI_TYPE_MA:
      _nb = mes->vv;
     break;
    case MIDI_TYPE_PB:
    case MIDI_TYPE_SYSEX:
     break;
   }

   if ( stream_vio_s_write(ss, data, len) != len ) {
    streams_delete(id);
    return -1;
   }
  }

  if ( roar_buffer_get_next(buf, &buf) == -1 )
   buf = NULL;
 }

 return 0;
}
#undef _nb

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
int midi_conv_mes2ssynth(void) {
 struct roar_buffer        * buf = g_midi_mess.buf;
 struct midi_message       * mes = NULL;

 while (buf != NULL) {
  if ( roar_buffer_get_data(buf, (void**)&mes) == -1 ) {
   return -1;
  }

  if ( ssynth_eval_message(mes) == -1 )
   return -1;

  if ( roar_buffer_get_next(buf, &buf) == -1 )
   buf = NULL;
 }

 return 0;
}
#endif

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

 ROAR_DBG("midi_add_buf(id=%i, buf=%p) = ?", id, buf);
 ROAR_DBG("midi_add_buf(*): MIDI Message of Type 0x%.2X", mes->type);
 ROAR_DBG("midi_add_buf(*): Channel: %i", mes->channel);
 ROAR_DBG("midi_add_buf(*): flags=0x%.2X", mes->flags);
 ROAR_DBG("midi_add_buf(*): kk=0x%.2X, vv=0x%.2X", mes->kk, mes->vv);

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

 client_stream_add(g_self_client, g_midi_clock.stream);

 midi_vio_set_dummy(g_midi_clock.stream);

 streams_get(g_midi_clock.stream, &ss);
 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->pos_rel_id    =  g_midi_clock.stream;

 s->info.codec    =  ROAR_CODEC_MIDI;
 ss->codec_orgi   =  ROAR_CODEC_MIDI;

 s->info.channels = 1; // we have only one channel, ticking on channel 0
 s->info.rate     = ROAR_MIDI_TICKS_PER_BEAT; // one beat per sec
 s->info.bits     = ROAR_MIDI_BITS;

 if ( streams_set_dir(g_midi_clock.stream, ROAR_DIR_BRIDGE, 1) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component clock");
  return -1;
 }

 streams_set_name(g_midi_clock.stream, "MIDI Clock");

 streams_set_flag(g_midi_clock.stream, ROAR_FLAG_PRIMARY);
 streams_set_flag(g_midi_clock.stream, ROAR_FLAG_SYNC);

 midi_clock_set_bph(3600); // one beat per sec

 ss->state = ROAR_STREAMSTATE_NEW;

 midi_config.inited |= MIDI_INITED_CLOCK;

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
 struct roar_buffer  * buf;
 struct midi_message * mes;
 struct roar_stream_server * ss;
 unsigned int diff;

 if ( streams_get(g_midi_clock.stream, &ss) == -1 ) {
  ROAR_ERR("midi_clock_tick(void): Something very BAD happend: can not get stream object of my own stream!");
  g_midi_clock.stream = -1;
  ROAR_WARN("midi_clock_tick(void): Disabled MIDI Clock");
  return -1;
 }

 while ( g_pos >= g_midi_clock.nt ) {
  diff = g_pos - g_midi_clock.nt;
  ROAR_DBG("midi_clock_tick(void): g_pos is %u samples (%5.2f%%) after of nt.", diff, (float)diff/g_midi_clock.spt);

  g_midi_clock.nt   = ROAR_MATH_OVERFLOW_ADD(g_midi_clock.nt, g_midi_clock.spt);

  if ( streams_get_flag(g_midi_clock.stream, ROAR_FLAG_SYNC) ) {
   ROAR_DBG("midi_clock_tick(void): TICK! (nt=%lu)", g_midi_clock.nt);
   if ( midi_new_bufmes(&buf, &mes) == -1 ) {
    ROAR_ERR("midi_clock_tick(void): Can not create Clock-Tick-Message");
   }

   mes->type = MIDI_TYPE_CLOCK_TICK;

   if ( midi_add_buf(g_midi_clock.stream, buf) == -1 ) {
    ROAR_ERR("midi_clock_tick(void): Can not add Clock-Tick-Message");
    roar_buffer_free(buf);
    return -1;
   }

   ROAR_STREAM(ss)->pos = ROAR_MATH_OVERFLOW_ADD(ROAR_STREAM(ss)->pos, 1);
   ss->state = ROAR_STREAMSTATE_OLD;
  } else {
   ROAR_DBG("midi_clock_tick(void): silent tick. (nt=%lu)", g_midi_clock.nt);
  }
 }

 return 0;
}

// CB:

#ifndef ROAR_WITHOUT_DCOMP_CB
int midi_cb_init (void) {
#ifdef _HAVE_CONSOLE
 struct roar_stream * s;
 struct roar_stream_server * ss;
 int i;
 char * files[] = {
                   "/NX",
                   "/dev/roarconsole",
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

 if ( midi_config.console_dev != NULL ) {
  files[0] = midi_config.console_dev;
 }

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

 client_stream_add(g_self_client, g_midi_clock.stream);

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
 streams_set_flag(g_midi_cb.stream, ROAR_FLAG_MUTE);

 midi_config.inited |= MIDI_INITED_CB;

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

 return 0;
}

int midi_cb_update (void) {

 if ( !streams_get_flag(g_midi_cb.stream, ROAR_FLAG_MUTE) ) {
  if ( midi_cb_readbuf() == -1 )
   return -1;
 } else if ( g_midi_cb.playing ) {
  midi_cb_stop();
 }

 if ( !g_midi_cb.playing )
  return 0;

/*
 if ( g_midi_cb.stoptime <= g_pos )
  midi_cb_stop();
*/

 ROAR_DBG("midi_cb_update(void) = ?");

 return 0;
}

int midi_cb_start(float freq) {
// On linux this uses ioctl KIOCSOUND
#ifdef __linux__
 if ( g_midi_cb.console == -1 )
  return -1;

 if ( ioctl(g_midi_cb.console, KIOCSOUND, freq == 0 ? 0 : (int)(1193180.0/freq)) == -1 )
  return -1;

 g_midi_cb.playing = 1;

 return 0;
#else
 return -1;
#endif
}

int midi_cb_stop (void) {
#ifdef __linux__
 int ret;

 ret = midi_cb_start(0);
 g_midi_cb.playing = 0;

 return ret;
#else
 return -1;
#endif
}

int midi_cb_readbuf(void) {
 struct roar_buffer        * buf = g_midi_mess.buf;
 struct midi_message       * mes = NULL;

 ROAR_DBG("midi_cb_readbuf(void) = ?");

 while (buf != NULL) {
  ROAR_DBG("midi_cb_readbuf(void): buf=%p", buf);

  if ( roar_buffer_get_data(buf, (void**)&mes) == -1 ) {
   return -1;
  }

  switch (mes->type) {
   case MIDI_TYPE_NOTE_ON:
     midi_cb_start(mes->d.note.freq);
    break;
   case MIDI_TYPE_NOTE_OFF:
     midi_cb_stop();
    break;
   case MIDI_TYPE_CONTROLER:
     if ( mes->kk == MIDI_CCE_ALL_NOTE_OFF ) /* all note off */
      midi_cb_stop();
    break;
  }

  if ( roar_buffer_get_next(buf, &buf) == -1 )
   buf = NULL;
 }

 return 0;
}
#endif

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

#endif

//ll
