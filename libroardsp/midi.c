//midi.c:

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

#include "libroardsp.h"

struct {
 uint16_t id;
 float freq;
 char * name;
} _libroar_notes[] = {
 {ROAR_MIDI_NOTE_C , 261.625565, "C"},
 {ROAR_MIDI_NOTE_Cs, 277.182631, "C#"},
 {ROAR_MIDI_NOTE_D , 293.664768, "D"},
 {ROAR_MIDI_NOTE_Ds, 311.126984, "D#"},
 {ROAR_MIDI_NOTE_E , 329.627557, "E"},
 {ROAR_MIDI_NOTE_F , 349.228231, "F"},
 {ROAR_MIDI_NOTE_Fs, 369.994423, "F#"},
 {ROAR_MIDI_NOTE_G , 391.995436, "G"},
 {ROAR_MIDI_NOTE_Gs, 415.304698, "G#"},
 {ROAR_MIDI_NOTE_A , 440.000000, "A"},
 {ROAR_MIDI_NOTE_As, 466.163762, "A#"},
 {ROAR_MIDI_NOTE_B , 493.883301, "B"},
 {0, 0, NULL}
};

char         * roar_midi_note2name   (uint16_t note) {
 int i;

 for (i = 0; _libroar_notes[i].name != NULL; i++)
  if ( _libroar_notes[i].id == note )
   return _libroar_notes[i].name;

 return NULL;
}

uint16_t   roar_midi_name2note   (char * note) {
 int i;

 for (i = 0; _libroar_notes[i].name != NULL; i++)
  if ( strcasecmp(_libroar_notes[i].name, note) == 0 )
   return _libroar_notes[i].id;

 return ROAR_MIDI_NOTE_NONE;
}

float          roar_midi_note2freq   (uint16_t note) {
 int i;

 for (i = 0; _libroar_notes[i].name != NULL; i++)
  if ( _libroar_notes[i].id == note )
   return _libroar_notes[i].freq;

 return -1;
}

int            roar_midi_find_octave (char * note);
int            roar_midi_add_octave  (struct roar_note_octave * note) {
 note->name[0] = 0;
 return -1;
}

int            roar_midi_notefill    (struct roar_note_octave * note) {
 int oct;

 if ( !note )
  return -1;

 if ( note->note && note->name[0] == 0 ) {
  roar_midi_add_octave(note);
 } else if ( !note->note && note->name[0] != 0 ) {
  return -1;
 }

 oct = 1 << abs(note->octave);

 note->freq = roar_midi_note2freq(note->note);

 if ( note->octave < 0 ) {
  note->freq /= oct;
 } else {
  note->freq *= oct;
 }

 return 0;
}

int roar_midi_gen_tone (struct roar_note_octave * note, int16_t * samples, float t, int rate, int channels, int type, void * opts) {
#ifdef ROAR_HAVE_LIBM
 int i, c;
 float ct;
 float step = M_PI*2*note->freq/rate;
 int16_t s;
 float (*op)(float x) = NULL;

/*
 rate: 1/s
 t   : 1s
 freq: 1/s
 rew : 1s
*/

 if ( type == ROAR_MIDI_TYPE_SINE ) {
  op = sinf;
 } else {
  return -1;
 }

 if ( op == NULL )
  return -1;

 for (ct = 0, i = 0; ct <= t; ct += step, i += channels) {
  s = 32767*op(ct);

  for (c = 0; c < channels; c++)
   samples[i+c] = s;
 }

 return 0;
#else
 return -1;
#endif
}


int roar_midi_play_note  (struct roar_stream * stream, struct roar_note_octave * note, float len) {
 return -1;
}

int roar_midi_basic_init (struct roar_midi_basic_state * state) {
 if (!state)
  return -1;

 state->len.mul = 1;
 state->len.div = 60;

 state->note.note   = ROAR_MIDI_NOTE_NONE;
 state->note.octave = 0;

 return 0;
}

int roar_midi_basic_play (struct roar_stream * stream, struct roar_midi_basic_state * state, char * notes) {
 struct roar_midi_basic_state is;
 char   cn[ROAR_MIDI_MAX_NOTENAME_LEN+1] = {0};
 int i;
 int have = 0;
 struct roar_note_octave * n;

 if ( !notes )
  return -1;

 if ( state == NULL ) {
  state = &is;
  roar_midi_basic_init(state);
 }

 n = &(state->note);

 for (; *notes != 0; notes++) {
  if ( *notes == '>' ) {
   n->octave++;
  } else if ( *notes == '<' ) {
   n->octave--;
  } else if ( *notes != ' ' ) {
   have++;
  }

  if (have) {
   roar_midi_play_note(stream, n, 60 * state->len.mul / state->len.div);
   have = 0;
  }
 }

 return 0;
}

//ll
