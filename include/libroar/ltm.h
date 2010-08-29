//ltm.h:

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

#ifndef _LIBROARLTM_H_
#define _LIBROARLTM_H_

#include "libroar.h"

struct roar_ltm_result;

int roar_ltm_register(struct roar_connection * con, int mt, int window, int * streams, size_t slen);
int roar_ltm_unregister(struct roar_connection * con, int mt, int window, int * streams, size_t slen);

struct roar_ltm_result * roar_ltm_get(struct roar_connection * con, int mt, int window, int * streams, size_t slen, struct roar_ltm_result * oldresult);

#define roar_ltm_freeres(x) roar_mm_free((x))

int roar_ltm_get_numstreams(struct roar_ltm_result * res);
int roar_ltm_get_mt(struct roar_ltm_result * res);
int roar_ltm_get_window(struct roar_ltm_result * res);

int roar_ltm_get_numchans(struct roar_ltm_result * res, int streamidx);
int64_t roar_ltm_extract(struct roar_ltm_result * res, int mt, int streamidx, int channel);

#endif

//ll
