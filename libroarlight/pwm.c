//pwm.c:

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

#include "libroarlight.h"

uint16_t _g_roar_lpwm16[] = {
/*
    0x0000, 0x0001, 0x0101, 0x0111,  0x1111, 0x1115, 0x1515, 0x1555,
    0x5555, 0x5557, 0x5757, 0x5777,  0x7777, 0x777F, 0x7F7F, 0x7FFF,
    0xFFFF
*/
    0xFFFF, 0x7F7F, 0x777F, 0x7777,  0x5777, 0x5757, 0x5557, 0x5555,
    0x1555, 0x1515, 0x1115, 0x1111,  0x0111, 0x0101, 0x0001, 0x0000
                            };

int roar_light_pwm_new (struct roar_lpwm_state * state, int bits ) {
 if ( state == NULL )
  return -1;

 if ( bits < 1 || bits > 32 )
  return -1;

 state->bits = bits;

 return roar_light_pwm_set(state, 0);
}

int roar_light_pwm_set (struct roar_lpwm_state * state, int value) {
 if ( state == NULL )
  return -1;

 if ( value < 0 || value > state->bits )
  return -1;

 state->value = value;

 return 0;
}

int roar_light_pwm_send(struct roar_lpwm_state * state, struct roar_vio_calls * vio, size_t len) {
 char          * buf;
 int16_t       * buf16;
 size_t          todo = len;
 uint64_t        s;

 ROAR_DBG("roar_light_pwm_send(state=%p, vio=%p, len=%u) = ?", state, vio, len);

 if ( state == NULL )
  return -1;

 if ( vio == NULL )
  return -1;

 if ( state->bits != 16 )
  return -1;

 if ( len == 0 )
  return 0;

 if ( (buf = malloc(len)) == NULL )
  return -1;

 buf16 = (int16_t *) buf;

 while (todo > 1) {
  ROAR_DBG("roar_light_pwm_send(*): loop: todo=%u, fill=%i", todo, state->fill);

  if ( state->fill < 16 ) {
   s             = _g_roar_lpwm16[state->value];
   s           <<= state->fill;
   state->s     |= s;
   state->fill  += 16;
  }

  *buf16 = state->s & 0xFFFF;
  state->s    >>= 16;
  state->fill  -= 16;

  buf16++;
  todo -= 2;
 }

 if ( todo ) {
  if ( state->fill < 8 ) {
   s             = _g_roar_lpwm16[state->value];
   s           <<= state->fill;
   state->s     |= s;
   state->fill  += 16;
  }

  buf[len-1]    = state->s & 0xFF;
  state->s    >>= 8;
  state->fill  -= 8;
 }

 if ( roar_vio_write(vio, buf, len) != len ) {
  free(buf);
  return -1;
 }

 free(buf);

 return 0;
}

//ll
