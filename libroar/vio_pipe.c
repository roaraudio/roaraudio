//vio_pipe.c:

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

int roar_vio_open_pipe (struct roar_vio_calls * s1, struct roar_vio_calls * s2, int type, int flags) {
 struct roar_vio_pipe * self;
 int                    rw = flags & (O_RDONLY|O_WRONLY|O_RDWR);

 if ( s1 == NULL || s2 == NULL )
  return -1;

 if ( (self = malloc(sizeof(struct roar_vio_pipe))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct roar_vio_pipe));

 self->refcount = 2;
 self->type     = type;
 self->flags    = flags;

 if ( type == ROAR_VIO_PIPE_TYPE_AUTO ) {
#ifdef ROAR_TARGET_WIN32
  type = ROAR_VIO_PIPE_TYPE_BUFFER;
#else
  type = ROAR_VIO_PIPE_TYPE_SOCKET;
#endif
 }

 switch (type) {
  case ROAR_VIO_PIPE_TYPE_BUFFER:
    // no buffers need to be set up here,
    // we handle the NULL pointer in the reader and writer func
   break;
  case ROAR_VIO_PIPE_TYPE_PIPE:
    if ( rw == O_RDWR || rw == O_RDONLY )
     if ( pipe(self->b.p) == -1 ) {
      free(self);
      return -1;
     }
    if ( rw == O_RDWR || rw == O_WRONLY )
     if ( pipe((self->b.p) + 2) == -1 ) {
      close(self->b.p[0]);
      close(self->b.p[1]);
      free(self);
      return -1;
     }
   break;
  case ROAR_VIO_PIPE_TYPE_SOCKET:
    if ( socketpair(AF_UNIX, SOCK_STREAM, 0, self->b.p) == -1 ) {
     free(self);
     return -1;
    }

    if ( rw == O_RDONLY ) {
     shutdown(self->b.p[0], SHUT_WR);
     shutdown(self->b.p[1], SHUT_RD);
    } else if ( rw == O_WRONLY ) {
     shutdown(self->b.p[0], SHUT_RD);
     shutdown(self->b.p[1], SHUT_WR);
    }
   break;
  default:
    free(self);
    return -1;
 }

 return 0;
}

//ll
