//interleave.c:

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

int roar_interl_init  (struct roar_interleave * state, size_t channels, size_t bits) {
 if ( state == NULL )
  return -1;

 // we can currently only hanle full bytes:
 if ( bits % 8 )
  return -1;

 // currently we have a channel limit
 if ( channels > ROAR_INTERLEAVE_MAX_CHANNELS )
  return -1;

 memset(state, 0, sizeof(struct roar_interleave));

 state->channels = channels;
 state->bits     = bits;

 return 0;
}

int roar_interl_uninit(struct roar_interleave * state) {
 if ( state == NULL )
  return -1;

 memset(state, 0, sizeof(struct roar_interleave));

 return 0;
}

int roar_interl_ctl   (struct roar_interleave * state, int cmd, void * data) {
 if ( state == NULL )
  return -1;

 return -1;
}

int roar_interl_encode_ext(struct roar_interleave * state, void ** in, void  * out, size_t len) {
 if ( state == NULL )
  return -1;

 return -1;
}

int roar_interl_decode_ext(struct roar_interleave * state, void * in, void ** out, size_t len) {
 size_t chan = 0;
 size_t ic;       // input counter
 size_t bc   = 0; // bit counter
 char   * op[ROAR_INTERLEAVE_MAX_CHANNELS]; // output pointer

 if ( state == NULL )
  return -1;

 if ( in == NULL || out == NULL )
  return -1;

 if ( len == 0 )
  return 0;

 memcpy(op, out, sizeof(void*)*state->channels);

 for (ic = 0; ic < len; ic++) {
  // get char and copy it
  *(op[chan]) = ((char*)in)[ic];
  op[chan]++;

  bc += 8;

  if ( bc == state->bits ) {
   bc = 0;
   chan++;
   if ( chan == state->channels )
    chan = 0;
  }
 }

 return -1;
}

//ll
