//remove.c:

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

int roar_remove_init (struct roar_remove_state * state) {
 if ( state == NULL )
  return -1;

 memset(state, 0, sizeof(struct roar_remove_state));

 state->old = 65535;

 return 0;
}

int roar_remove      (void * inout, void * subs, int samples, int bits, struct roar_remove_state * state) {
 if ( inout == NULL || subs == NULL || samples < 0 )
  return -1;

 switch (bits) {
  case 8:  return roar_remove_8 (inout, subs, samples, state); break;
  case 16: return roar_remove_16(inout, subs, samples, state); break;
  case 32: return roar_remove_32(inout, subs, samples, state); break;
 }

 return -1;
}

int roar_remove_8    (int8_t  * inout, int8_t  * subs, int samples, struct roar_remove_state * state) {
 int i;
 register int_least16_t s;
 register int_least16_t peak;

 if ( state == NULL ) {
  for (i = 0; i < samples; i++) {
   s  = inout[i];
   s -= subs[i];
   inout[i] = s;
  }
 } else {
  peak = 127;
  for (i = 0; i < samples; i++) {
   s  = inout[i];
   s -= subs[i];
   s  = s < 0 ? -s : s; // we true 32 bit, not int operation here
   if ( s > peak )
    peak = s;
  }

  for (i = 0; i < samples; i++) {
   s  = -subs[i];
   s *=  127;
   s /=  peak;
   s +=  inout[i];
   inout[i] = s;
  }
 }

 return 0;
}

int roar_remove_16   (int16_t * inout, int16_t * subs, int samples, struct roar_remove_state * state) {
 int i;
 register int32_t s;
 register int32_t peak;

 if ( state == NULL ) {
  for (i = 0; i < samples; i++) {
   s  = inout[i];
   s -= subs[i];
   inout[i] = s;
  }
 } else {
  peak = 65535;
  for (i = 0; i < samples; i++) {
   s  = inout[i];
   s -= subs[i];
   s  = s < 0 ? -s : s; // we true 32 bit, not int operation here
   if ( s > peak )
    peak = s;
  }

  for (i = 0; i < samples; i++) {
   s  = -subs[i];
   s *=  65535;
   s /=  peak;
   s +=  inout[i];
   inout[i] = s;
  }
 }

 return 0;
}

int roar_remove_32   (int32_t * inout, int32_t * subs, int samples, struct roar_remove_state * state) {
 int i;
 register int64_t s;
 register int64_t peak;

 if ( state == NULL ) {
  for (i = 0; i < samples; i++) {
   s  = inout[i];
   s -= subs[i];
   inout[i] = s;
  }
 } else {
  peak = 4294967295UL;
  for (i = 0; i < samples; i++) {
   s  = inout[i];
   s -= subs[i];
   s  = s < 0 ? -s : s; // we true 32 bit, not int operation here
   if ( s > peak )
    peak = s;
  }

  for (i = 0; i < samples; i++) {
   s  = -subs[i];
   s *=  4294967295UL;
   s /=  peak;
   s +=  inout[i];
   inout[i] = s;
  }
 }

 return 0;
}

int roar_remove_so   (void    * subout, void   * in, int samples, int bits, struct roar_remove_state * state) {
 if ( subout == NULL || in == NULL || samples < 0 )
  return -1;

 switch (bits) {
  case 16: return roar_remove_so16(subout, in, samples, state); break;
 }

 return -1;
}

int roar_remove_so8  (int8_t  * subout, int8_t  * in, int samples, struct roar_remove_state * state) {
 int i;
 register int_least16_t s;
 register int_least16_t peak;

 if ( state == NULL ) {
  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s +=  in[i];
   subout[i] = s;
  }
 } else {
  peak = 127;
  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s +=  in[i];
   s  = s < 0 ? -s : s; // we true 32 bit, not int operation here
   if ( s > peak )
    peak = s;
  }

  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s *=  127;
   s /=  peak;
   s +=  in[i];
   subout[i] = s;
  }
 }

 return 0;
}

int roar_remove_so16 (int16_t * subout, int16_t * in, int samples, struct roar_remove_state * state) {
 int i;
 register int32_t s;
 register int32_t peak;

 if ( state == NULL ) {
  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s +=  in[i];
   subout[i] = s;
  }
 } else {
  peak = 65535;
  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s +=  in[i];
   s  = s < 0 ? -s : s; // we true 32 bit, not int operation here
   if ( s > peak )
    peak = s;
  }

  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s *=  65535;
   s /=  peak;
   s +=  in[i];
   subout[i] = s;
  }
 }

 return 0;
}

int roar_remove_so32 (int32_t * subout, int32_t * in, int samples, struct roar_remove_state * state) {
 int i;
 register int64_t s;
 register int64_t peak;

 if ( state == NULL ) {
  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s +=  in[i];
   subout[i] = s;
  }
 } else {
  peak = 4294967295UL;
  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s +=  in[i];
   s  = s < 0 ? -s : s; // we true 32 bit, not int operation here
   if ( s > peak )
    peak = s;
  }

  for (i = 0; i < samples; i++) {
   s  = -subout[i];
   s *=  4294967295UL;
   s /=  peak;
   s +=  in[i];
   subout[i] = s;
  }
 }

 return 0;
}

//ll
