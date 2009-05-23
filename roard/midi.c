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

 return 0;
}
int midi_free (void) {
 return midi_cb_free();
}

int midi_update(void) {
 return midi_cb_update();
}

// STREAMS:

int midi_check_stream  (int id) {
 return -1;
}
int midi_send_stream   (int id) {
 return -1;
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

 g_console          = -1;
 g_midi_cb_stream   = -1;
 g_midi_cb_stoptime =  0;
 g_midi_cb_playing  =  0;

 for (i = 0; files[i] != NULL; i++) {
  if ( (g_console = open(files[i], O_WRONLY|O_NOCTTY, 0)) != -1 )
   break;
 }

 if ( g_console == -1 )
  return -1;

 if ( (g_midi_cb_stream = streams_new()) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component CB");
  midi_cb_free();
  return -1;
 }

 streams_get(g_midi_cb_stream, &ss);
 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->pos_rel_id    = -1;

 s->info.codec    =  0;
 ss->codec_orgi   =  0;

 s->info.channels =  1;
 s->info.rate     = 1193180;
 s->info.bits     =  8;

 if ( streams_set_dir(g_midi_cb_stream, ROAR_DIR_BRIDGE, 1) == -1 ) {
  ROAR_WARN("Error while initializing MIDI subsystem component CB");
  midi_cb_free();
  return -1;
 }

 streams_set_name(g_midi_cb_stream, "Console speaker bridge");

 streams_set_flag(g_midi_cb_stream, ROAR_FLAG_OUTPUT);
 streams_set_flag(g_midi_cb_stream, ROAR_FLAG_PRIMARY);

 return 0;
#else
 g_console          = -1;
 g_midi_cb_stream   = -1;

 return -1;
#endif
}

int midi_cb_free (void) {
#ifdef _HAVE_CONSOLE

 midi_cb_stop();

 if ( g_midi_cb_stream != -1 )
  streams_delete(g_midi_cb_stream);

 if ( g_console != -1 )
  close(g_console);

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
 if ( g_midi_cb_playing && override != MIDI_CB_OVERRIDE )
  return -1;

 g_midi_cb_stoptime = ROAR_MATH_OVERFLOW_ADD(g_pos, samples_per_sec*t);
 midi_cb_start(freq);
 g_midi_cb_playing = 1;

 return 0;
}

int midi_cb_update (void) {
 if ( !g_midi_cb_playing )
  return 0;

 if ( g_midi_cb_stoptime <= g_pos )
  midi_cb_stop();

 return 0;
}

int midi_cb_start(float freq) {
// On linux this uses ioctl KIOCSOUND
#ifdef __linux__
 if ( g_console == -1 )
  return -1;

 if ( ioctl(g_console, KIOCSOUND, freq == 0 ? 0 : (int)(1193180.0/freq)) == -1 )
  return -1;

 return 0;
#else
 return -1;
#endif
}

int midi_cb_stop (void) {
#ifdef __linux__
 g_midi_cb_playing = 0;
 return midi_cb_start(0);
#else
 return -1;
#endif
}

//ll
