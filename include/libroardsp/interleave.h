//interleave.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifndef _LIBROARDSP_INTERLEAVE_H_
#define _LIBROARDSP_INTERLEAVE_H_

#include "libroardsp.h"

#define ROAR_INTERLEAVE_CTL_GET                   0x00
#define ROAR_INTERLEAVE_CTL_SET                   0x01

#define ROAR_INTERLEAVE_CTL_SET_MAX_BUFFER        (ROAR_INTERLEAVE_CTL_SET|0x10)

#define ROAR_INTERLEAVE_MAX_CHANNELS              16

struct roar_interleave {
 size_t channels;
 size_t bits;
};

int roar_interl_init  (struct roar_interleave * state, size_t channels, size_t bits);
int roar_interl_uninit(struct roar_interleave * state);

int roar_interl_ctl   (struct roar_interleave * state, int cmd, void * data);

int roar_interl_decode_ext(struct roar_interleave * state, void * in, void ** out, size_t len);

#endif

//ll
