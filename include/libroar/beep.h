//beep.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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

#ifndef _LIBROARBEEP_H_
#define _LIBROARBEEP_H_

#include "libroar.h"

#define ROAR_BEEP_MAX_VOL  65535
#define ROAR_BEEP_MAX_TIME 65535 /* ms */
#define ROAR_BEEP_MAX_FREQ 65535 /* Hz */
#define ROAR_BEEP_MAX_POS  32767

#define ROAR_BEEP_DEFAULT_VOL  (ROAR_BEEP_MAX_VOL/4)
#define ROAR_BEEP_DEFAULT_TIME 256 /* ms */
#define ROAR_BEEP_DEFAULT_FREQ 440 /* Hz */
#define ROAR_BEEP_DEFAULT_TYPE ROAR_BEEP_TYPE_DEFAULT

#define ROAR_BEEP_TYPE_DEFAULT            0
#define ROAR_BEEP_TYPE_CBELL              1
#define ROAR_BEEP_TYPE_XBELL              2
#define ROAR_BEEP_TYPE_ERROR              3

struct roar_beep {
 uint16_t vol;
 uint16_t time;
 uint16_t freq;
 uint16_t type;
 int16_t  x, y, z;
};

int roar_beep(struct roar_connection * con, const struct roar_beep * beep);

#endif

//ll
