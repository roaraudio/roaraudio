//fader.c:

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

#define _CHECK_BASIC()   if ( state == NULL ) return -1
#define _CHECK_CALCPCM() _CHECK_BASIC(); if ( frames == 0 ) return 0; if ( data == NULL ) return -1;

int roar_fader_init         (struct roar_fader_state * state, float * poly, int coeff) {
 _CHECK_BASIC();

 if ( poly == NULL )
  return -1;

 if ( coeff < 2 )
  return -1;

 if ( coeff > ROAR_FADER_MAX_COEFF )
  return -1;

 memset(state, 0, sizeof(struct roar_fader_state));

 state->rate  = -1;
 state->coeff = coeff;

 state->start = -1;
 state->stop  = -1;

 memcpy(&(state->poly), poly, sizeof(float)*coeff);

 return 0;
}

int roar_fader_set_rate     (struct roar_fader_state * state, int rate) {
 _CHECK_BASIC();

 state->rate = rate;

 return 0;
}

int roar_fader_set_startstop(struct roar_fader_state * state, ssize_t start, ssize_t stop) {
 _CHECK_BASIC();

 if ( start >= 0 )
  state->start = start;

 if ( stop  >= 0 )
  state->stop  = stop;

 return 0;
}

int roar_fader_calcpcm_i16n(struct roar_fader_state * state, int16_t * data, size_t frames, int channels) {
 _CHECK_CALCPCM();

 switch (channels) {
  case 1: return roar_fader_calcpcm_i161(state, data, frames);
 }

 return -1;
}

int roar_fader_calcpcm_i161(struct roar_fader_state * state, int16_t * data, size_t frames) {
 return -1;
}

//ll
