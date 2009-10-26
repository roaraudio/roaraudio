//vio_jumbo.c:

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

int     roar_vio_open_jumbo    (struct roar_vio_calls * calls, struct roar_vio_calls * vio, size_t buffersize) {
 struct roar_vio_jumbo * self;

 if ( (self = roar_mm_malloc(sizeof(struct roar_vio_jumbo))) == NULL ) {
  return -1;
 }

 memset(self, 0, sizeof(struct roar_vio_jumbo));

 self->backend = vio;

 if ( roar_buffer_new(&(self->buffer), buffersize) == -1 ) {
  roar_mm_free(self);
  return -1;
 }

 memset(calls, 0, sizeof(struct roar_vio_calls));
 calls->inst     = self;
 calls->close    = roar_vio_jumbo_close;
 calls->read     = roar_vio_jumbo_read;
 calls->write    = roar_vio_jumbo_write;
 calls->lseek    = roar_vio_jumbo_lseek;
 calls->nonblock = roar_vio_jumbo_nonblock;
 calls->sync     = roar_vio_jumbo_sync;
 calls->ctl      = roar_vio_jumbo_ctl;

 return 0;
}

int     roar_vio_jumbo_close   (struct roar_vio_calls * vio) {
 struct roar_vio_jumbo * self = vio->inst;

 if ( roar_vio_jumbo_sync(vio) == -1 )
  return -1;

 roar_buffer_free(self->buffer);
 roar_mm_free(self);

 return 0;
}

ssize_t roar_vio_jumbo_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_jumbo * self = vio->inst;

 return roar_vio_read(self->backend, buf, count);
}

ssize_t roar_vio_jumbo_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_jumbo * self = vio->inst;
 size_t   buflen;
 void   * data;

 ROAR_DBG("roar_vio_jumbo_write(vio=%p, buf=%p, count=%lu) = ?", vio, buf, (unsigned long) count);

 if ( roar_buffer_get_len(self->buffer, &buflen) == -1 )
  return -1;

 ROAR_DBG("roar_vio_jumbo_write(vio=%p, buf=%p, count=%lu) = ?", vio, buf, (unsigned long) count);

 if ( roar_buffer_get_data(self->buffer, &data) == -1 )
  return -1;

 ROAR_DBG("roar_vio_jumbo_write(vio=%p, buf=%p, count=%lu) = ?", vio, buf, (unsigned long) count);

 if ( (self->pos + count) > buflen ) {
  if ( roar_vio_jumbo_sync(vio) == -1 )
   return -1;

  // in case we write something that is longer than the buffer
  if ( self->pos > buflen ) {
   return roar_vio_write(self->backend, data, count);
  }

  memcpy(data, buf, count);
  self->pos = count;
 } else {
  memcpy(data + self->pos, buf, count);
  self->pos += count;
 }

 ROAR_DBG("roar_vio_jumbo_write(vio=%p, buf=%p, count=%lu) = ?", vio, buf, (unsigned long) count);

 return count;
}

off_t   roar_vio_jumbo_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 struct roar_vio_jumbo * self = vio->inst;

 if ( roar_vio_jumbo_sync(vio) == -1 )
  return (off_t) -1;

 return roar_vio_lseek(self->backend, offset, whence);
}

int     roar_vio_jumbo_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_vio_jumbo * self = vio->inst;

 return roar_vio_nonblock(self->backend, state);
}

int     roar_vio_jumbo_sync    (struct roar_vio_calls * vio) {
 struct roar_vio_jumbo * self = vio->inst;
 void                  * data;

 if ( self->pos == 0 )
  return 0;

 if ( roar_buffer_get_data(self->buffer, &data) == -1 )
  return -1;

 // TODO: do this a bit more intelergent (RE?)
 if ( roar_vio_write(self->backend, data, self->pos) != self->pos )
  return -1;

 self->pos = 0;

 return 0;
}

int     roar_vio_jumbo_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 return -1;
}

//ll
