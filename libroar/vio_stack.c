//vio_stack.c:

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

int     roar_vio_open_stack    (struct roar_vio_calls * calls) {
 struct roar_vio_stack * self;

 if ( calls == NULL )
  return -1;

 if ( (self = roar_mm_malloc(sizeof(struct roar_vio_stack))) == NULL )
  return -1;

 memset(self,  0, sizeof(struct roar_vio_stack));
 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst     = self;
 calls->close    = roar_vio_stack_close;
 calls->ctl      = roar_vio_stack_ctl;
 calls->read     = roar_vio_stack_read;
 calls->write    = roar_vio_stack_write;
 calls->lseek    = roar_vio_stack_lseek;
 calls->nonblock = roar_vio_stack_nonblock;
 calls->sync     = roar_vio_stack_sync;

 return 0;
}

int     roar_vio_stack_add     (struct roar_vio_calls * calls, struct roar_vio_calls * vio) {
 struct roar_vio_stack * self;

 if ( calls == NULL || vio == NULL )
  return -1;

 if ( (self = calls->inst) == NULL )
  return -1;

 if ( self->next == ROAR_VIO_STACK_MAX )
  return -1;

 self->cur = self->calls[self->next++] = vio;

 return 0;
}

int     roar_vio_stack_close   (struct roar_vio_calls * vio) {
 struct roar_vio_stack * self;
 int i;

 if ( vio == NULL )
  return -1;

 if ( (self = vio->inst) == NULL )
  return -1;

 if ( self->cur != NULL ) {
  if ( roar_vio_close(self->cur) == -1 )
   return -1;

  for (i = 0; i < self->next; i++)
   free(self->calls[i]);
 }

 roar_mm_free(self);

 return 0;
}

int     roar_vio_stack_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 if (vio == NULL || cmd == -1)
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_WRITE_FH:
  case ROAR_VIO_CTL_SELECT:
    return roar_vio_ctl(((struct roar_vio_stack*)(vio->inst))->cur, cmd, data);
   break;
 }

 return roar_vio_ctl(((struct roar_vio_stack*)(vio->inst))->cur, cmd, data);
}


ssize_t roar_vio_stack_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 if ( ((struct roar_vio_stack*)(vio->inst))->cur == NULL )
  return -1;

 return roar_vio_read(((struct roar_vio_stack*)(vio->inst))->cur, buf, count);
}

ssize_t roar_vio_stack_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 if ( ((struct roar_vio_stack*)(vio->inst))->cur == NULL )
  return -1;

 return roar_vio_write(((struct roar_vio_stack*)(vio->inst))->cur, buf, count);
}

off_t   roar_vio_stack_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 if ( ((struct roar_vio_stack*)(vio->inst))->cur == NULL )
  return -1;

 return roar_vio_lseek(((struct roar_vio_stack*)(vio->inst))->cur, offset, whence);
}

int     roar_vio_stack_nonblock(struct roar_vio_calls * vio, int state) {
 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 if ( ((struct roar_vio_stack*)(vio->inst))->cur == NULL )
  return -1;

 return roar_vio_nonblock(((struct roar_vio_stack*)(vio->inst))->cur, state);
}

int     roar_vio_stack_sync    (struct roar_vio_calls * vio) {
 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 if ( ((struct roar_vio_stack*)(vio->inst))->cur == NULL )
  return -1;

 return roar_vio_sync(((struct roar_vio_stack*)(vio->inst))->cur);
}

//ll
