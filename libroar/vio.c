//vio.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

int roar_vio_init_calls (struct roar_vio_calls * calls) {
 if ( calls == NULL )
  return -1;

 memset((void*)calls, 0, sizeof(struct roar_vio_calls));

/*
 calls->read  = (ssize_t (*)(int fd, void *buf, size_t count,      void * inst))read;
 calls->write = (ssize_t (*)(int fd, void *buf, size_t count,      void * inst))write;
 calls->lseek = (off_t   (*)(int fildes, off_t offset, int whence, void * inst))lseek;
*/

 calls->read     = roar_vio_basic_read;
 calls->write    = roar_vio_basic_write;
 calls->lseek    = roar_vio_basic_lseek;
 calls->nonblock = roar_vio_basic_nonblock;
 calls->sync     = roar_vio_basic_sync;
 calls->close    = roar_vio_basic_close;

 return 0;
}

int roar_vio_set_inst (struct roar_vio_calls * vio, void * inst) {
 if ( vio == NULL )
  return -1;

 vio->inst = inst;

 return 0;
}

int roar_vio_set_fh   (struct roar_vio_calls * vio, int fh) {
 return roar_vio_set_inst(vio, (void*)(ROAR_INSTINT)(fh + 1));
}

int roar_vio_get_fh   (struct roar_vio_calls * vio) {
 if ( vio == NULL )
  return -1;

 return ((int)(ROAR_INSTINT)vio->inst) - 1;
}


ssize_t roar_vio_read (struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( vio == NULL )
  return -1;

 if ( vio->read == NULL )
  return -1;

 return vio->read(vio, buf, count);
}

ssize_t roar_vio_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( vio == NULL )
  return -1;

 if ( vio->write == NULL )
  return -1;

 return vio->write(vio, buf, count);
}

off_t   roar_vio_lseek(struct roar_vio_calls * vio, off_t offset, int whence) {
 if ( vio == NULL )
  return -1;

 if ( vio->lseek == NULL )
  return -1;

 return vio->lseek(vio, offset, whence);
}

int     roar_vio_nonblock(struct roar_vio_calls * vio, int state) {
 if ( vio == NULL )
  return -1;

 if ( vio->nonblock == NULL )
  return -1;

 return vio->nonblock(vio, state);
}

int     roar_vio_sync    (struct roar_vio_calls * vio) {
 if ( vio == NULL )
  return -1;

 if ( vio->sync == NULL )
  return -1;

 return vio->sync(vio);
}

int     roar_vio_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 if ( vio == NULL )
  return -1;

 if ( vio->ctl == NULL )
  return -1;

 return vio->ctl(vio, cmd, data);
}

int     roar_vio_close    (struct roar_vio_calls * vio) {
 if ( vio == NULL )
  return -1;

 if ( vio->sync == NULL )
  return -1;

 return vio->close(vio);
}

// VIOs:

// basic
ssize_t roar_vio_basic_read (struct roar_vio_calls * vio, void *buf, size_t count) {
 return read(roar_vio_get_fh(vio), buf, count);
}

ssize_t roar_vio_basic_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 return write(roar_vio_get_fh(vio), buf, count);
}

off_t   roar_vio_basic_lseek(struct roar_vio_calls * vio, off_t offset, int whence) {
 return lseek(roar_vio_get_fh(vio), offset, whence);
}

int     roar_vio_basic_nonblock(struct roar_vio_calls * vio, int state) {
 if ( roar_socket_nonblock(roar_vio_get_fh(vio), state) == -1 )
  return -1;

 if ( state == ROAR_SOCKET_NONBLOCK )
  return 0;

 roar_vio_sync(vio);

 return 0;
}

int     roar_vio_basic_sync    (struct roar_vio_calls * vio) {
 return ROAR_FDATASYNC(roar_vio_get_fh(vio));
}

int     roar_vio_basic_close    (struct roar_vio_calls * vio) {
 return close(roar_vio_get_fh(vio));
}

// null
ssize_t roar_vio_null_rw    (struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( vio == NULL || buf == NULL )
  return -1;

 return 0;
}

// pass
int     roar_vio_open_pass    (struct roar_vio_calls * calls, struct roar_vio_calls * dst) {
 if ( calls == NULL )
  return -1;

 memset((void*)calls, 0, sizeof(struct roar_vio_calls));

 calls->read     = roar_vio_pass_read;
 calls->write    = roar_vio_pass_write;
 calls->lseek    = roar_vio_pass_lseek;
 calls->nonblock = roar_vio_pass_nonblock;
 calls->sync     = roar_vio_pass_sync;
 calls->close    = roar_vio_pass_close;

 calls->inst     = dst;

 return 0;
}

ssize_t roar_vio_pass_read (struct roar_vio_calls * vio, void *buf, size_t count) {
 return roar_vio_read((struct roar_vio_calls *) vio->inst, buf, count);
}

ssize_t roar_vio_pass_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 return roar_vio_write((struct roar_vio_calls *) vio->inst, buf, count);
}

off_t   roar_vio_pass_lseek(struct roar_vio_calls * vio, off_t offset, int whence) {
 return roar_vio_lseek((struct roar_vio_calls *) vio->inst, offset, whence);
}

int     roar_vio_pass_nonblock(struct roar_vio_calls * vio, int state) {
 return roar_vio_nonblock((struct roar_vio_calls *) vio->inst, state);
}

int     roar_vio_pass_sync    (struct roar_vio_calls * vio) {
 return roar_vio_sync((struct roar_vio_calls *) vio->inst);
}

int     roar_vio_pass_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 return roar_vio_ctl((struct roar_vio_calls *) vio->inst, cmd, data);
}

int     roar_vio_pass_close   (struct roar_vio_calls * vio) {
 return roar_vio_close((struct roar_vio_calls *) vio->inst);
}


// re
int     roar_vio_open_re (struct roar_vio_calls * calls, struct roar_vio_calls * dst) {
 if ( roar_vio_open_pass(calls, dst) == -1 )
  return -1;

 calls->read  = roar_vio_re_read;
 calls->write = roar_vio_re_write;
 calls->lseek = roar_vio_re_lseek;

 return 0;
}
ssize_t roar_vio_re_read (struct roar_vio_calls * vio, void *buf, size_t count) {
  size_t len =  0;
 ssize_t r   = -1;

 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 errno = 0;

 while ( (r = roar_vio_read((struct roar_vio_calls *) vio->inst, buf, count)) > 0 ) {
  len   += r;
  buf   += r;
  count -= r;
  if ( count == 0 )
   break;
 }

 if ( len == 0 && r == -1 )
  return -1;

 return len;
}

ssize_t roar_vio_re_write(struct roar_vio_calls * vio, void *buf, size_t count) {
  size_t len =  0;
 ssize_t r   = -1;

 if ( vio == NULL )
  return -1;

 if ( vio->inst == NULL )
  return -1;

 errno = 0;

 while ( (r = roar_vio_write((struct roar_vio_calls *) vio->inst, buf, count)) > 0 ) {
  len   += r;
  buf   += r;
  count -= r;
  if ( count == 0 )
   break;
 }

 if ( len == 0 && r == -1 )
  return -1;

 return len;
}

// TODO: we should do a some more intelgent thing here.
off_t   roar_vio_re_lseek(struct roar_vio_calls * vio, off_t offset, int whence) {
 return roar_vio_lseek((struct roar_vio_calls *) vio->inst, offset, whence);
}


//ll
