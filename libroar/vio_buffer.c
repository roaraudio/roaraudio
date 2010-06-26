//vio_buffer.c:

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

int     roar_vio_open_buffer    (struct roar_vio_calls * calls, struct roar_vio_calls * dst, ssize_t minsize, int use_re) {
 struct roar_vio_buffer * self;;

 if ( calls == NULL || dst == NULL )
  return -1;

 if ( (self = roar_mm_malloc(sizeof(struct roar_vio_buffer))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct roar_vio_buffer));

 self->backend     = dst;
 self->min_bufsize = minsize;
 self->use_re      = use_re;

 if ( use_re ) {
  if ( roar_vio_open_re(&(self->re_vio), dst) == -1 ) {
   roar_mm_free(self);
   return -1;
  }
 }

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst     = self;
 calls->close    = roar_vio_buffer_close;
 calls->write    = roar_vio_buffer_write;
 calls->nonblock = roar_vio_buffer_nonblock;
 calls->ctl      = roar_vio_buffer_ctl;
 calls->sync     = roar_vio_buffer_sync;
 calls->lseek    = roar_vio_buffer_lseek;

 return 0;
}

int     roar_vio_buffer_close   (struct roar_vio_calls * vio) {
 struct roar_vio_buffer * self = vio->inst;
 int ret;

 if ( self->buf_old != NULL )
  roar_buffer_free(self->buf_old);

 if ( self->buf_cur != NULL )
  roar_buffer_free(self->buf_cur);

 if ( self->use_re ) {
  ret = roar_vio_close(&(self->re_vio));
 } else {
  ret = roar_vio_close(self->backend);
 }

 roar_mm_free(self);

 return ret;
}

ssize_t roar_vio_buffer_read    (struct roar_vio_calls * vio, void *buf, size_t count);

int     roar_vio_buffer_sync    (struct roar_vio_calls * vio) {
 struct roar_vio_buffer * self = vio->inst;
 struct roar_vio_calls  * backend;
 off_t dst = 0;

 if ( self->use_re ) {
  backend = &(self->re_vio);
 } else {
  backend = self->backend;
 }

 // bring backend in sync state, if this fails we do not need to continue...
 if ( self->use_re ) {
  if ( roar_vio_sync(backend) == -1 )
   return -1;
 } else {
  if ( roar_vio_sync(backend) == -1 )
   return -1;
 }

 if ( self->buf_old == NULL && self->buf_cur == NULL )
  return 0; // we are in sync in case no buffers are currently used.

 if (self->buf_old != NULL && self->buf_cur == NULL ) {
  // we just finished the segment
  roar_buffer_free(self->buf_old);
  return 0;
 }

 // calc seek pos:
 if ( self->offset.is_old ) {
  dst = (self->len_cur + self->len_old) - self->offset.offset;
 } else {
  dst = self->len_cur - self->offset.offset;
 }

 dst = -dst;

 // do the seek:
 if ( roar_vio_lseek(backend, dst, SEEK_CUR) == (off_t)-1 )
  return -1;

 // free all internal buffers:
 if ( self->buf_old != NULL )
  roar_buffer_free(self->buf_old);

 if ( self->buf_cur != NULL )
  roar_buffer_free(self->buf_cur);

 self->offset.is_old = 0;
 self->offset.offset = 0;

 // funally bring the backend in sync state again, to be sure all changes are done.
 if ( self->use_re ) {
  if ( roar_vio_sync(backend) == -1 )
   return -1;
 } else {
  if ( roar_vio_sync(backend) == -1 )
   return -1;
 }

 return 0;
}

off_t   roar_vio_buffer_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 struct roar_vio_buffer * self = vio->inst;
 size_t newoff;
 off_t  ret;
 off_t  invoff;

 // in-memory seeking is only supported for SEEK_CUR:
 if ( whence == SEEK_CUR ) {
  if ( offset == 0 ) {
   return 0;
  } else if ( offset > 0 ) {
   newoff = self->offset.offset + offset;

   if ( self->offset.is_old ) {
    if ( newoff == self->len_old ) {
     self->offset.is_old = 0;
     self->offset.offset = 0;
     self->abspos += offset;
     return self->abspos;
    } else if ( newoff < self->len_old ) {
     self->offset.offset = newoff;
     self->abspos += offset;
     return self->abspos;
    } else if ( newoff > self->len_old ) {
     if ( newoff < (self->len_old + self->len_cur ) ) {
      self->offset.is_old = 0;
      self->offset.offset = offset + self->offset.offset - self->len_old;
      self->abspos += offset;
      return self->abspos;
     }
    }
   } else {
    if ( newoff == self->len_cur ) {
     roar_buffer_free(self->buf_old);
     self->buf_old = self->buf_cur;
     self->buf_cur = NULL;
     self->offset.offset = 0;
     self->abspos += offset;
     return self->abspos;
    } else if ( newoff < self->len_cur ) {
     self->offset.offset = newoff;
     self->abspos += offset;
     return self->abspos;
    }
   }
  } else {
   invoff = -offset;

   if ( invoff <= self->offset.offset ) {
    self->offset.offset = 0;
    self->abspos -= invoff;
    return self->abspos;
   }

   if ( !self->offset.is_old ) {
    if ( invoff > self->offset.offset ) {
     if ( invoff <= (self->len_old + self->offset.offset) ) {
      self->offset.is_old = 1;
      self->offset.offset = (self->len_old + self->offset.offset) - invoff;
      self->abspos -= invoff;
      return self->abspos;
     }
    }
   }
  }
 }

 // we need to do a physical seek;
 // get in sync with current possition, flush all buffers,...
 if ( roar_vio_buffer_sync(vio) == -1 )
  return -1;

 // do the seek:
 if ( self->use_re ) {
  ret = roar_vio_lseek(&(self->re_vio), offset, whence);
 } else {
  ret = roar_vio_lseek(self->backend, offset, whence);
 }

 if (ret != (off_t)-1)
  self->abspos = ret;

 return ret;
}

int     roar_vio_buffer_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 struct roar_vio_buffer * self;

 if (vio == NULL || cmd == -1)
  return -1;

 ROAR_DBG("roar_vio_buffer_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 self = vio->inst;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "buffer";
    return 0;
   break;
  case ROAR_VIO_CTL_GET_NEXT:
    if ( self->use_re ) {
     *(struct roar_vio_calls **)data = &(self->re_vio);
    } else {
     *(struct roar_vio_calls **)data = self->backend;
    }
    return 0;
   break;
  case ROAR_VIO_CTL_SET_NEXT:
    if ( self->use_re ) {
     return roar_vio_ctl(&(self->re_vio), ROAR_VIO_CTL_SET_NEXT, data);
    } else {
     self->backend = *(struct roar_vio_calls **)data;
    }
    return 0;
   break;
 }

 return roar_vio_ctl((struct roar_vio_calls *) vio->inst, cmd, data);
}

ssize_t roar_vio_buffer_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_buffer * self = vio->inst;

 if ( self->use_re ) {
  return roar_vio_write(&(self->re_vio), buf, count);
 } else {
  return roar_vio_write(self->backend, buf, count);
 }
}

int     roar_vio_buffer_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_vio_buffer * self = vio->inst;

 if ( self->use_re ) {
  return roar_vio_nonblock(&(self->re_vio), state);
 } else {
  return roar_vio_nonblock(self->backend, state);
 }
}

//ll
