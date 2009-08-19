//synth.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#define _CHECK_BASIC() if ( state == NULL ) return -1
#define _CHECK_PCMOUT() _CHECK_BASIC(); if ( frames == 0 ) return 0; if ( out == NULL ) return -1

int roar_synth_init(struct roar_synth_state * state, struct roar_note_octave * note, int rate) {
 _CHECK_BASIC();

 if ( rate <= 0 )
  return -1;

 memset(state, 0, sizeof(struct roar_synth_state));

 state->note = note; // NULL is valid here!
 state->rate = rate;

 state->func = ROAR_SYNTH_SYNF_SINE;

 return 0;
}

int roar_synth_set_offset(struct roar_synth_state * state, size_t offset) {
 _CHECK_BASIC();

 state->pcmoffset = offset;

 return 0;
}

int roar_synth_set_func  (struct roar_synth_state * state, ROAR_SYNTH_FUNC_TYPE(func)) {
 _CHECK_BASIC();

 if ( func == NULL )
  return -1;

 state->func = func;

 return 0;
}

int roar_synth_pcmout_i16n(struct roar_synth_state * state, int16_t * out, size_t frames, int channels) {
 _CHECK_PCMOUT();

 switch (channels) {
  case 1: return roar_synth_pcmout_i161(state, out, frames);
  default:
    return -1;
 }

 return 0;
}

int roar_synth_pcmout_i161(struct roar_synth_state * state, int16_t * out, size_t frames) {
 float t_step;
 float t_cur;
 int i;

 _CHECK_PCMOUT();

 t_step = 1.0/state->rate;

 t_cur  = t_step * state->pcmoffset;

 for (i = 0; i < frames; i++, t_cur += t_step) {
  out[i] = 32767.0*state->func(t_cur, state);
 }

 state->pcmoffset += frames;

 return -1;
}


//ll
