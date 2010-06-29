//vio_buffer_store.c:

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

int     roar_vio_open_buffer_store    (struct roar_vio_calls * calls, struct roar_vio_buffer_store ** inst) {
 struct roar_vio_buffer_store * self;

 if ( calls == NULL )
  return -1;

 self = roar_mm_malloc(sizeof(struct roar_vio_buffer_store));

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roar_vio_buffer_store));

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst     = self;
 calls->close    = roar_vio_buffer_store_close;
 calls->nonblock = roar_vio_buffer_store_nonblock;
 calls->sync     = roar_vio_buffer_store_sync;
 calls->ctl      = roar_vio_buffer_store_ctl;
 calls->write    = roar_vio_buffer_store_write;
 calls->read     = roar_vio_buffer_store_read;
 calls->lseek    = roar_vio_buffer_store_lseek;

 return 0;
}

int     roar_vio_buffer_store_close   (struct roar_vio_calls * vio) {
 struct roar_vio_buffer_store * self = vio->inst;
 int ret = 0;

 if ( self->in != NULL )
  if ( roar_buffer_free(self->in) != 0 )
   ret = -1;

 if ( self->out != NULL )
  if ( roar_buffer_free(self->out) != 0 )
   ret = -1;

 return ret;
}

ssize_t roar_vio_buffer_store_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_buffer_store * self = vio->inst;

 if ( count == 0 )
  return 0;

 if ( buf == NULL )
  return -1;

 if ( self->out == NULL )
  return 0;

 if ( roar_buffer_shift_out(&(self->out), buf, &count) == -1 )
  return -1;

 return count;
}

ssize_t roar_vio_buffer_store_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_buffer_store * self = vio->inst;
 struct roar_buffer * nbuf;
 void * data;

 if ( count == 0 )
  return 0;

 if ( buf == NULL )
  return -1;

 if ( roar_buffer_new_data(&nbuf, count, &data) == -1 )
  return -1;

 memcpy(data, buf, count);

 if ( self->in == NULL ) {
  self->in = nbuf;
 } else {
  if ( roar_buffer_add(self->in, nbuf) == -1 ) {
   roar_buffer_free(nbuf);
   return -1;
  }
 }

 return count;
}

off_t   roar_vio_buffer_store_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 // TODO: implement support to seek forward as wide as the buffer allows us to seek.
 return (off_t)-1;
}

int     roar_vio_buffer_store_nonblock(struct roar_vio_calls * vio, int state) {
 return 0; // if we are in nonblock or not is the same for us.
}

int     roar_vio_buffer_store_sync    (struct roar_vio_calls * vio) {
 return 0; // we are always sync.
}

int     roar_vio_buffer_store_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 if (vio == NULL || cmd == -1)
  return -1;

 ROAR_DBG("roar_vio_buffer_store_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "buffer_store";
    return 0;
   break;
 }

 return -1;
}

//ll
