//libroar.c:

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

#include "libroar.h"

#ifndef roar_mm_mlock
int roar_mm_mlock(const void *addr, size_t len) {
#if defined(ROAR_TARGET_WIN32)
 return GlobalLock(addr) == addr ? 0 : -1;
#elif defined(ROAR_TARGET_MICROCONTROLLER)
 return 0;
#else
 long sz = sysconf(_SC_PAGESIZE);
 unsigned long int pos = (unsigned long int) addr;

 len += sz - (len % sz);

 pos -= pos % sz;

 return mlock((void*)pos, len);
#endif
}
#endif

#ifndef roar_mm_munlock
int roar_mm_munlock(const void *addr, size_t len) {
#if defined(ROAR_TARGET_WIN32)
 // TODO: find out what do do here. GlobalUnLock()? does such a function exist?
// return GlobalLock(addr) == addr ? 0 : -1;
 return -1;
#elif defined(ROAR_TARGET_MICROCONTROLLER)
 return 0;
#else
 long sz = sysconf(_SC_PAGESIZE);
 unsigned long int pos = (unsigned long int) addr;

 len += sz - (len % sz);

 pos -= pos % sz;

 return munlock((void*)pos, len);
#endif
}
#endif

// for compatibility with old versions:
int _ROAR_MLOCK(const void *addr, size_t len) {
 roar_debug_warn_obsolete("_ROAR_MLOCK", "roar_mm_mlock", NULL);
 return roar_mm_mlock(addr, len);
}

//ll
