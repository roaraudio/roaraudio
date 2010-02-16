

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

int     roar_vio_open_magic    (struct roar_vio_calls * calls, struct roar_vio_calls * dst, int * codec);
int     roar_vio_magic_close   (struct roar_vio_calls * vio);

ssize_t roar_vio_magic_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_magic * self = (struct roar_vio_magic *)(vio->inst);
 struct roar_buffer    * inp;
 void * calls;
 size_t len;

 if (self == NULL)
  return -1;

 if ( roar_stack_get_cur(&(self->vios), &calls) == -1 )
  return -1;

 inp = &(self->inp);

 if ( roar_buffer_get_len(inp, &len) == -1 )
  return -1;

 if ( len ) {
  len = len > count ? count : len;
  if ( roar_buffer_shift_out(&inp, buf, &len) == -1 )
   return -1;
 }

 count -= len;
 buf += len;

 if ( count ) {
  if ( (count = roar_vio_read((struct roar_vio_calls*) calls, buf, count)) == -1 )
   return len;

  return len+count;
 }

 return len;
}

ssize_t roar_vio_magic_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_magic * self = (struct roar_vio_magic *)(vio->inst);
 void * calls;

 if (self == NULL)
  return -1;

 if ( roar_stack_get_cur(&(self->vios), &calls) == -1 )
  return -1;

 return roar_vio_write((struct roar_vio_calls*)calls, buf, count);
}

off_t   roar_vio_magic_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 struct roar_vio_magic * self = (struct roar_vio_magic *)(vio->inst);
 void * calls;

 if (self == NULL)
  return -1;

 if ( roar_stack_get_cur(&(self->vios), &calls) == -1 )
  return -1;

 return roar_vio_lseek((struct roar_vio_calls*)calls, offset, whence);
}

int     roar_vio_magic_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_vio_magic * self = (struct roar_vio_magic *)(vio->inst);
 void * calls;

 if (self == NULL)
  return -1;

 if ( roar_stack_get_cur(&(self->vios), &calls) == -1 )
  return -1;

 return roar_vio_nonblock((struct roar_vio_calls*)calls, state);
}

int     roar_vio_magic_sync    (struct roar_vio_calls * vio) {
 struct roar_vio_magic * self = (struct roar_vio_magic *)(vio->inst);
 void * calls;

 if (self == NULL)
  return -1;

 if ( roar_stack_get_cur(&(self->vios), &calls) == -1 )
  return -1;

 return roar_vio_sync((struct roar_vio_calls*)calls);
}

//ll
