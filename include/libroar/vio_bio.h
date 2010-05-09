//vio_bio.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

#ifndef _LIBROARVIO_BIO_H_
#define _LIBROARVIO_BIO_H_

#include "libroar.h"

#ifndef ROAR_HAVE_LIBSSL
typedef void BIO;
#endif

int     roar_vio_open_bio      (struct roar_vio_calls * calls, BIO * bio);
BIO *   roar_vio_to_bio        (struct roar_vio_calls * calls);

#ifdef ROAR_HAVE_LIBSSL
ssize_t roar_vio_bio_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_bio_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_bio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_bio_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_bio_sync    (struct roar_vio_calls * vio);
int     roar_vio_bio_close   (struct roar_vio_calls * vio);
#endif

#endif

//ll
