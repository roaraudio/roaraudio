//vio_bio.c:

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

int     roar_vio_open_bio      (struct roar_vio_calls * calls, BIO * bio) {
#ifdef ROAR_HAVE_LIBSSL
 if ( calls == NULL || bio == NULL )
  return -1;

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->read     = roar_vio_bio_read;
 calls->write    = roar_vio_bio_write;
 calls->lseek    = roar_vio_bio_lseek;
 calls->nonblock = roar_vio_bio_nonblock;
 calls->sync     = roar_vio_bio_sync;
 calls->close    = roar_vio_bio_close;

 calls->inst     = (void*) bio;

 return 0;
#else
 return -1;
#endif
}

BIO *   roar_vio_to_bio        (struct roar_vio_calls * calls) {
 return NULL;
}

#ifdef ROAR_HAVE_LIBSSL
ssize_t roar_vio_bio_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 BIO * bio = (BIO*)(vio->inst);
 int r;

 if ( (r = BIO_read(bio, buf, count)) == -2 )
  return -1;

 return r;
}
ssize_t roar_vio_bio_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 BIO * bio = (BIO*)(vio->inst);
 int r;

 if ( (r = BIO_write(bio, buf, count)) == -2 )
  return -1;

 return r;
}

off_t   roar_vio_bio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 return -1;
}

int     roar_vio_bio_nonblock(struct roar_vio_calls * vio, int state) {
 return -1;
}
int     roar_vio_bio_sync    (struct roar_vio_calls * vio) {
 return -1;
}

int     roar_vio_bio_close   (struct roar_vio_calls * vio) {
 BIO * bio = (BIO*)(vio->inst);

 // TODO: check the return values

 BIO_flush(bio);
 BIO_free_all(bio);

 return 0;
}
#endif

//ll
