//filterchain.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#ifndef _LIBROARDSP_FILTERCHAIN_H_
#define _LIBROARDSP_FILTERCHAIN_H_

#include "libroardsp.h"

#define ROARDSP_MAX_FILTERS_PER_CHAIN 8

// types:
struct roardsp_filterchain {
 int filters;
 struct roardsp_filter * filter[ROARDSP_MAX_FILTERS_PER_CHAIN];
};

// funcs:
int roardsp_fchain_init  (struct roardsp_filterchain * chain);
int roardsp_fchain_uninit(struct roardsp_filterchain * chain);
int roardsp_fchain_add   (struct roardsp_filterchain * chain, struct roardsp_filter * filter);
int roardsp_fchain_calc  (struct roardsp_filterchain * chain, void * data, size_t len);
int roardsp_fchain_reset (struct roardsp_filterchain * chain, int what);
int roardsp_fchain_num   (struct roardsp_filterchain * chain);

#endif

//ll
