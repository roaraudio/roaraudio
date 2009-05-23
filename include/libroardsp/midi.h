//midi.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - August 2008
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _LIBROARMIDI_H_
#define _LIBROARMIDI_H_

#include "libroardsp.h"

#ifdef ROAR_HAVE_LIBM
#include <math.h>
#endif

#define ROAR_MIDI_NOTE_NONE -1

#define ROAR_MIDI_NOTE_C  26162
#define ROAR_MIDI_NOTE_Cs 27718
#define ROAR_MIDI_NOTE_D  29366
#define ROAR_MIDI_NOTE_Ds 31112
#define ROAR_MIDI_NOTE_E  32962
#define ROAR_MIDI_NOTE_F  34922
#define ROAR_MIDI_NOTE_Fs 36999
#define ROAR_MIDI_NOTE_G  39199
#define ROAR_MIDI_NOTE_Gs 41530
#define ROAR_MIDI_NOTE_A  44000
#define ROAR_MIDI_NOTE_As 46616
#define ROAR_MIDI_NOTE_B  49388

#define ROAR_MIDI_MAX_NOTENAME_LEN (1+8)

#define ROAR_MIDI_TYPE_SINE 1

struct roar_midi_len {
 int mul;
 int div;
};

struct roar_note_octave {
 uint16_t       note;
 char           name[ROAR_MIDI_MAX_NOTENAME_LEN+1];
 int            octave;
 float          freq;
 struct roar_midi_len len;
};


struct roar_midi_basic_state {
 struct roar_midi_len    len;
 struct roar_note_octave note;
};

char         * roar_midi_note2name   (uint16_t note);
uint16_t       roar_midi_name2note   (char * note);
uint16_t       roar_midi_midi2note   (unsigned char midiid);
float          roar_midi_note2freq   (uint16_t note);

//int            roar_midi_free_no     (struct roar_note_octave * note);

int            roar_midi_find_octave (char * note);
int            roar_midi_add_octave  (struct roar_note_octave * note);
int            roar_midi_notefill    (struct roar_note_octave * note);

int            roar_midi_gen_tone    (struct roar_note_octave * note, int16_t * samples, float t, int rate, int channels, int type, void * opts);


int roar_midi_play_note  (struct roar_stream * stream, struct roar_note_octave * note, float len);

int roar_midi_basic_init (struct roar_midi_basic_state * state);
int roar_midi_basic_play (struct roar_stream * stream, struct roar_midi_basic_state * state, char * notes);

#endif

//ll
