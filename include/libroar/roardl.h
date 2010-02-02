//roardl.h:

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

#ifndef _LIBROARROARDL_H_
#define _LIBROARROARDL_H_

#include "libroar.h"

#define ROAR_DL_HANDLE_DEFAULT          ((struct roar_dl_lhandle*)(void*)0)
#define ROAR_DL_HANDLE_NEXT             ((struct roar_dl_lhandle*)(void*)1)

struct roar_dl_lhandle {
#if defined(ROAR_HAVE_LIBDL)
 void * handle;
#else
 char dummy[8];
#endif
};

struct roar_dl_librarypara {
 int version;
 size_t len;
};

struct roar_dl_libraryinst {
 int version;
 size_t len;
};

struct roar_dl_lhandle * roar_dl_open(const char * filename, int flags, int ra_init);
int                      roar_dl_close(struct roar_dl_lhandle * lhandle);

void                   * roar_dl_getsym(struct roar_dl_lhandle * lhandle, const char * sym, int type);

int                      roar_dl_ra_init(struct roar_dl_lhandle * lhandle, const char * prefix);

#endif

//ll
