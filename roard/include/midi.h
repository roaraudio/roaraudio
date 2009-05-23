//midi.h:

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

#ifndef _MIDI_H_
#define _MIDI_H_

#include <roaraudio.h>

#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/kd.h>
#endif

#define MIDI_CB_NOOVERRIDE 0
#define MIDI_CB_OVERRIDE   1

int g_console;

uint32_t g_midi_cb_stoptime;
int g_midi_cb_playing;

int midi_init (void);
int midi_free (void);

int midi_update(void);

int midi_check_stream  (int id);
int midi_send_stream   (int id);

// cb = console beep
int midi_cb_play(float t, float freq, int override);
int midi_cb_update (void);
int midi_cb_start(float freq);
int midi_cb_stop (void);

#endif

//ll
