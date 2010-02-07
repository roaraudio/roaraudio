//memmgr.h:

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

#ifndef _LIBROARMEMMGR_H_
#define _LIBROARMEMMGR_H_

#include "libroar.h"

#ifdef ROAR_USE_MEMMGR
// those functions are currently not implemeted:
void * roar_mm_calloc(size_t nmemb, size_t size);
void * roar_mm_malloc(size_t size);
void   roar_mm_free(void *ptr);
void * roar_mm_realloc(void *ptr, size_t size);
char * roar_mm_strdup(const char *s);

/*
 * TODO: we need some functions to control the pre-alloc
 *       of memory.
 */

#else
#define roar_mm_calloc(nmemb, size) calloc((nmemb), (size))
#define roar_mm_malloc(size)        malloc((size))
#define roar_mm_free(ptr)           free((ptr))
#define roar_mm_realloc(ptr, size)  realloc((ptr), (size))
#define roar_mm_strdup(str)         strdup((str))
#endif

#endif

//ll
