//vio_ops.c:

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

#define ROAR_VIO_COPY_BUFSIZE 1024

ssize_t roar_vio_copy_data   (struct roar_vio_calls * out, struct roar_vio_calls * in) {
 char    buf[ROAR_VIO_COPY_BUFSIZE];
 ssize_t len;
 ssize_t done = 0;

 ROAR_DBG("roar_vio_copy_data(out=%p, in=%p) = ?", out, in);

 if ( out == NULL || in == NULL )
  return -1;

 while ((len = roar_vio_read(in, buf, ROAR_VIO_COPY_BUFSIZE)) > 0) {
  if ( roar_vio_write(out, buf, len) != len )
   return -1;

  done += len;
  ROAR_DBG("roar_vio_copy_data(out=%p, in=%p): len=%li, done=%li", out, in, (long int)len, (long int)done);
 }

 ROAR_DBG("roar_vio_copy_data(out=%p, in=%p): len=%li, done=%li", out, in, (long int)len, (long int)done);

 ROAR_DBG("roar_vio_copy_data(out=%p, in=%p) = %li", out, in, (long int)done);
 return done;
}

//ll
