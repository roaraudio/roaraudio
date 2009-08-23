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

#if !defined(ROAR_HAVE_LIBM) && !defined(M_PI)
#define M_PI 3.141592
#endif

#define _CHECK_BASIC() if ( state == NULL ) return -1
#define _CHECK_PCMOUT() _CHECK_BASIC(); if ( frames == 0 ) return 0; if ( out == NULL ) return -1

int roar_synth_init(struct roar_synth_state * state, struct roar_note_octave * note, int rate) {
 _CHECK_BASIC();

 if ( rate <= 0 )
  return -1;

 memset(state, 0, sizeof(struct roar_synth_state));

 state->note   = note; // NULL is valid here!
 state->rate   = rate;
 state->volume = 1;

 state->func = ROAR_SYNTH_SYNF_DEFAULT;

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

int roar_synth_set_volume(struct roar_synth_state * state, float volume) {
 _CHECK_BASIC();

 state->volume = volume;

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
 float freq;
#ifdef DEBUG
 float cv0, cv1;
#endif
 int i;

 _CHECK_PCMOUT();

 t_step = 1.0/state->rate;

 ROAR_DBG("roar_synth_pcmout_i161(*): state->pcmoffset=%lu", (unsigned long)state->pcmoffset);

 t_cur  = (float)state->pcmoffset/(float)state->rate;

 freq   = state->note->freq;

 ROAR_DBG("roar_synth_pcmout_i161(*): t_cur=%f, freq=%f", t_cur, freq);

#ifdef DEBUG
 cv0 = 2.0*M_PI*freq*t_cur;
#endif
 for (i = 0; i < frames; i++, t_cur += t_step) {
  out[i] = 32767.0*state->volume*state->func(2.0*M_PI*freq*t_cur, state);
 }
#ifdef DEBUG
 cv1 = 2.0*M_PI*freq*t_cur;
#endif

#ifdef DEBUG
 ROAR_DBG("roar_synth_pcmout_i161(*): cv0=%f, cv1=%f, cv1-cv0=%f", cv0, cv1, cv1-cv0);
#endif

 state->pcmoffset += frames;

 ROAR_DBG("roar_synth_pcmout_i161(*): state->pcmoffset=%lu", (unsigned long)state->pcmoffset);

 return 0;
}

// basic SINFs:
float roar_synth_synf_rect (float t, struct roar_synth_state * state) {
 t /= 2*M_PI;
 t -= (int)t;

 if ( t < 0.5 )
  return  1;
 else
  return -1;
}

float roar_synth_synf_saw  (float t, struct roar_synth_state * state) {
 t /= 2*M_PI;
 t -= (int)t;

 return 2*t - 1;
}

float roar_synth_synf_tri  (float t, struct roar_synth_state * state) {
 t /= 2*M_PI;
 t -= (int)t;

 if ( t < 0.5 )
  return   4* t      - 1;
 else
  return  -4*(t-0.5) + 1;
}

float roar_synth_synf_trap (float t, struct roar_synth_state * state) {
 t /= 2*M_PI;
 t -= (int)t;

 if ( t < 0.125 || t > 0.875 ) {
  return -1;
 } else if ( t < 0.625 && t > 0.375 ) {
  return  1;
 } else if ( t < 0.5 ) {
  return  8*(t-0.375) + 1;
 } else {
  return -8*(t-0.625) + 1;
 }
}

#ifdef ROAR_HAVE_LIBM
float roar_synth_synf_s2s  (float t, struct roar_synth_state * state) {
 float sin2 = sinf(t/1.2);

 return sin2*sin2 * sin(t*1.2);
}
#endif

//ll
