//auth.h:

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

#ifndef _LIBROARVIO_H_
#define _LIBROARVIO_H_

#include "libroar.h"

struct roar_vio_calls {
 void * inst;
/*
 ssize_t (*read) (int fd, void *buf, size_t count, void * inst);
 ssize_t (*write)(int fd, void *buf, size_t count, void * inst);
 off_t   (*lseek)(int fildes, off_t offset, int whence, void * inst);
*/
 ssize_t (*read    )(struct roar_vio_calls * vio, void *buf, size_t count);
 ssize_t (*write   )(struct roar_vio_calls * vio, void *buf, size_t count);
 off_t   (*lseek   )(struct roar_vio_calls * vio, off_t offset, int whence);
 int     (*nonblock)(struct roar_vio_calls * vio, int state);
 int     (*sync    )(struct roar_vio_calls * vio);
};

int roar_vio_init_calls (struct roar_vio_calls * calls);

int roar_vio_set_inst (struct roar_vio_calls * vio, void * inst);
int roar_vio_set_fh   (struct roar_vio_calls * vio, int fh);

int roar_vio_get_fh   (struct roar_vio_calls * vio);

ssize_t roar_vio_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_sync    (struct roar_vio_calls * vio);


// possible VIOs:

// basic
ssize_t roar_vio_basic_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_basic_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_basic_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_basic_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_basic_sync    (struct roar_vio_calls * vio);

// null
// this is read an write in one!
ssize_t roar_vio_null_rw    (struct roar_vio_calls * vio, void *buf, size_t count);

// pass

ssize_t roar_vio_pass_read (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_pass_write(struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_pass_lseek(struct roar_vio_calls * vio, off_t offset, int whence);

// re-read/write

ssize_t roar_vio_re_read (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_re_write(struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_re_lseek(struct roar_vio_calls * vio, off_t offset, int whence);



#endif

//ll
