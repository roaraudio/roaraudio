//vio_string.c:

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

#include "libroar.h"

int     roar_vio_putc    (struct roar_vio_calls * vio, char c) {
 return roar_vio_write(vio, &c, 1);
}

int     roar_vio_getc    (struct roar_vio_calls * vio) {
 unsigned char c;

 if ( roar_vio_read(vio, &c, 1) != 1 )
  return EOF;

 return c;
}

int     roar_vio_printf(struct roar_vio_calls * vio, const char *format, ...) {
 va_list ap;
 int ret;
 char buf[8192];

 va_start(ap, format);
 ret = vsnprintf(buf, 8192, format, ap);
 va_end(ap);

 return roar_vio_write(vio, buf, ret);
}


//ll
