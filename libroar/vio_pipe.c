//vio_pipe.c:

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

#include "libroar.h"

int roar_vio_open_pipe (struct roar_vio_calls * s0, struct roar_vio_calls * s1, int type, int flags) {
#ifndef ROAR_WITHOUT_VIO_PIPE
 struct roar_vio_pipe * self;
 int                    rw = flags & (O_RDONLY|O_WRONLY|O_RDWR);

 if ( s0 == NULL || s1 == NULL )
  return -1;

 if ( (self = roar_mm_malloc(sizeof(struct roar_vio_pipe))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct roar_vio_pipe));

 self->refcount = 2;
 self->flags    = flags;

 if ( type == ROAR_VIO_PIPE_TYPE_AUTO ) {
#ifdef ROAR_TARGET_WIN32
  type = ROAR_VIO_PIPE_TYPE_BUFFER;
#else
  type = ROAR_VIO_PIPE_TYPE_SOCKET;
#endif
 }

 self->type     = type;

 switch (type) {
  case ROAR_VIO_PIPE_TYPE_BUFFER:
    // no buffers need to be set up here,
    // we handle the NULL pointer in the reader and writer func
    roar_mm_free(self);
    return -1;
   break;
  case ROAR_VIO_PIPE_TYPE_PIPE:
    self->b.p[0] = self->b.p[1] = self->b.p[2] = self->b.p[3] = -1;

    if ( rw == O_RDWR || rw == O_RDONLY )
     if ( pipe(self->b.p) == -1 ) {
      roar_mm_free(self);
      return -1;
     }
    if ( rw == O_RDWR || rw == O_WRONLY )
     if ( pipe((self->b.p) + 2) == -1 ) {
      close(self->b.p[0]);
      close(self->b.p[1]);
      roar_mm_free(self);
      return -1;
     }
   break;
#ifdef ROAR_HAVE_UNIX
  case ROAR_VIO_PIPE_TYPE_SOCKET:
    if ( socketpair(AF_UNIX, SOCK_STREAM, 0, self->b.p) == -1 ) {
     roar_mm_free(self);
     return -1;
    }

    if ( rw == O_RDONLY ) {
     ROAR_SHUTDOWN(self->b.p[0], SHUT_WR);
     ROAR_SHUTDOWN(self->b.p[1], SHUT_RD);
    } else if ( rw == O_WRONLY ) {
     ROAR_SHUTDOWN(self->b.p[0], SHUT_RD);
     ROAR_SHUTDOWN(self->b.p[1], SHUT_WR);
    }
   break;
#endif
  default:
    roar_mm_free(self);
    return -1;
 }

 roar_vio_pipe_init(s0, self, flags);
 roar_vio_pipe_init(s1, self, flags);

 self->s0 = s0;

 return 0;
#else
 return -1;
#endif
}

#ifndef ROAR_WITHOUT_VIO_PIPE
int roar_vio_pipe_init (struct roar_vio_calls * s,  struct roar_vio_pipe * self, int flags) {
 if ( s == NULL || self == NULL )
  return -1;

 memset(s, 0, sizeof(struct roar_vio_calls));

 s->close    = roar_vio_pipe_close;
 s->read     = roar_vio_pipe_read;
 s->write    = roar_vio_pipe_write;
 s->nonblock = roar_vio_pipe_nonblock;
 s->sync     = roar_vio_pipe_sync;

 s->inst = (void*) self;

 if ( flags & O_NONBLOCK ) {
  roar_vio_pipe_nonblock(s, ROAR_SOCKET_NONBLOCK);
 }

 return 0;
}

int     roar_vio_pipe_close   (struct roar_vio_calls * vio) {
 struct roar_vio_pipe * self;
 int                    idx;

 if ( vio == NULL )
  return -1;

 if ( (self = (struct roar_vio_pipe *)vio->inst) == NULL )
  return -1;

 self->refcount--;

 switch (self->type) {
  case ROAR_VIO_PIPE_TYPE_BUFFER:
    // this will be a bit more complex as we need to change the flags, too.
   break;
  case ROAR_VIO_PIPE_TYPE_PIPE:
   switch (ROAR_VIO_PIPE_S(self, vio)) {
    case 0:
      close(self->b.p[0]);
      close(self->b.p[3]);
      self->b.p[0] = -1;
      self->b.p[3] = -1;
     break;
    case 1:
      close(self->b.p[1]);
      close(self->b.p[2]);
      self->b.p[1] = -1;
      self->b.p[2] = -1;
     break;
   }
   break;
#ifdef ROAR_HAVE_UNIX
  case ROAR_VIO_PIPE_TYPE_SOCKET:
    close(self->b.p[idx = ROAR_VIO_PIPE_S(self, vio)]);
    self->b.p[idx] = -1;
   break;
#endif
 }

 if ( ! self->refcount ) {
  roar_mm_free(self);
 }

 vio->inst = NULL;
 return 0;
}

int     roar_vio_pipe_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_vio_pipe * self;

 if ( vio == NULL )
  return -1;

 if ( (self = (struct roar_vio_pipe *)vio->inst) == NULL )
  return -1;

 switch (self->type) {
  case ROAR_VIO_PIPE_TYPE_PIPE:
    if ( roar_socket_nonblock(self->b.p[ROAR_VIO_PIPE_S(self,vio)*2], state) == -1 )
     return -1;
    return roar_socket_nonblock(self->b.p[(ROAR_VIO_PIPE_SR(self,vio)*2)+1], state);
   break;
#ifdef ROAR_HAVE_UNIX
  case ROAR_VIO_PIPE_TYPE_SOCKET:
    return roar_socket_nonblock(self->b.p[ROAR_VIO_PIPE_S(self,vio)], state);
   break;
#endif
 }

 return -1;
}

int     roar_vio_pipe_sync    (struct roar_vio_calls * vio) {
 // we may add fdatasync() calls here depending on the type
 // but in general they should not be needed on pipes.
 return 0;
}

int     roar_vio_pipe_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 struct roar_vio_pipe * self;

 if (vio == NULL || cmd == -1)
  return -1;

 if ( (self = (struct roar_vio_pipe *)vio->inst) == NULL )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "pipe";
    return 0;
   break;
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_SELECT_FH:
#ifdef ROAR_HAVE_UNIX
    if ( self->type == ROAR_VIO_PIPE_TYPE_SOCKET ) {
     *(int*)data = self->b.p[ROAR_VIO_PIPE_S(self,vio)];
     return 0;
    } else {
     return -1;
    }
#else
    return -1;
#endif
   break;
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_SELECT_READ_FH:
    switch (self->type) {
#ifdef ROAR_HAVE_UNIX
     case ROAR_VIO_PIPE_TYPE_SOCKET:
       *(int*)data = self->b.p[ROAR_VIO_PIPE_S(self,vio)];
       return 0;
      break;
#endif
     case ROAR_VIO_PIPE_TYPE_PIPE:
       *(int*)data = self->b.p[ROAR_VIO_PIPE_S(self,vio)*2];
       return 0;
      break;
    }
  case ROAR_VIO_CTL_GET_WRITE_FH:
  case ROAR_VIO_CTL_GET_SELECT_WRITE_FH:
    switch (self->type) {
#ifdef ROAR_HAVE_UNIX
     case ROAR_VIO_PIPE_TYPE_SOCKET:
       *(int*)data = self->b.p[ROAR_VIO_PIPE_S(self,vio)];
       return 0;
      break;
#endif
     case ROAR_VIO_PIPE_TYPE_PIPE:
       *(int*)data = self->b.p[(ROAR_VIO_PIPE_SR(self,vio)*2)+1];
       return 0;
      break;
    }
   break;
 }

 return -1;
}


ssize_t roar_vio_pipe_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_pipe * self;
 int                    idx;

 if ( vio == NULL )
  return -1;

 if ( (self = (struct roar_vio_pipe *)vio->inst) == NULL )
  return -1;

 switch (self->type) {
  case ROAR_VIO_PIPE_TYPE_BUFFER:
    idx = ROAR_VIO_PIPE_S(self,vio);

    if ( (idx == 0 ? O_WRONLY : O_RDONLY) == (self->flags & (O_RDONLY|O_WRONLY|O_RDWR)) ) {
     raise(SIGPIPE);
     return -1;
    }

    if ( self->b.b[idx] == NULL )
     return 0;

    if ( roar_buffer_shift_out(&(self->b.b[idx]), buf, &count) == -1 )
     return -1;

    return count;
   break;
  case ROAR_VIO_PIPE_TYPE_PIPE:
    return read(self->b.p[ROAR_VIO_PIPE_S(self,vio)*2], buf, count);
   break;
#ifdef ROAR_HAVE_UNIX
  case ROAR_VIO_PIPE_TYPE_SOCKET:
    return read(self->b.p[ROAR_VIO_PIPE_S(self,vio)], buf, count);
   break;
#endif
 }

 return -1;
}

ssize_t roar_vio_pipe_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_pipe * self;
 struct roar_buffer   * next;
 void                 * data;
 int                    idx;

 if ( vio == NULL )
  return -1;

 if ( (self = (struct roar_vio_pipe *)vio->inst) == NULL )
  return -1;

 switch (self->type) {
  case ROAR_VIO_PIPE_TYPE_BUFFER:
    if ( self->refcount < 2 ) {
     raise(SIGPIPE);
     return -1;
    }

    idx = ROAR_VIO_PIPE_SR(self,vio);

    if ( (idx == 0 ? O_WRONLY : O_RDONLY) == (self->flags & (O_RDONLY|O_WRONLY|O_RDWR)) ) {
     raise(SIGPIPE);
     return -1;
    }

    if ( roar_buffer_new_data(&next, count, &data) == -1 )
     return -1;

    memcpy(data, buf, count);

    if ( self->b.b[idx] == NULL ) {
     self->b.b[idx] = next;
    } else {
     if ( roar_buffer_add(self->b.b[idx], next) == -1 ) {
      roar_buffer_free(next);
      return -1;
     }
    }

    return count;
   break;
  case ROAR_VIO_PIPE_TYPE_PIPE:
    return write(self->b.p[(ROAR_VIO_PIPE_SR(self,vio)*2)+1], buf, count);
   break;
#ifdef ROAR_HAVE_UNIX
  case ROAR_VIO_PIPE_TYPE_SOCKET:
    return write(self->b.p[ROAR_VIO_PIPE_S(self,vio)], buf, count);
   break;
#endif
 }

 return -1;
}
#endif

//ll
