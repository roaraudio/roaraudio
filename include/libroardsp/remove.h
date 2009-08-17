//remove.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARDSP_REMOVE_H_
#define _LIBROARDSP_REMOVE_H_

#include "libroardsp.h"

struct roar_remove_state {
 uint16_t a, b;
 uint16_t  old;
};

int roar_remove_init (struct roar_remove_state * state);
int roar_remove      (void    * inout, void    * subs, int samples, int bits, struct roar_remove_state * state);
int roar_remove_8    (int8_t  * inout, int8_t  * subs, int samples, struct roar_remove_state * state);
int roar_remove_16   (int16_t * inout, int16_t * subs, int samples, struct roar_remove_state * state);
int roar_remove_32   (int32_t * inout, int32_t * subs, int samples, struct roar_remove_state * state);

int roar_remove_so   (void    * subout, void    * in, int samples, int bits, struct roar_remove_state * state);
int roar_remove_so8  (int16_t * subout, int16_t * in, int samples, struct roar_remove_state * state);
int roar_remove_so16 (int16_t * subout, int16_t * in, int samples, struct roar_remove_state * state);
int roar_remove_so32 (int16_t * subout, int16_t * in, int samples, struct roar_remove_state * state);

#endif

//ll
