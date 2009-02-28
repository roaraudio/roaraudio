//vio.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008, 2009
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

// CTLs:
/*
 * 0xAAAABBBB
 * AAAA:
 *   0x0001 -> Basic stream calls
 *   0x0002 -> Driver calls
 *
 * BBBB:
 *   0x0XXX -> Client
 *   0x1XXX -> Server 
 */
// stream:
#define ROAR_VIO_CTL_SET_STREAM    0x00010001 /* normal streams */
#define ROAR_VIO_CTL_GET_STREAM    0x00010002

#define ROAR_VIO_CTL_SET_SSTREAM   0x00011001 /* server streams */
#define ROAR_VIO_CTL_GET_SSTREAM   0x00011002

#define ROAR_VIO_CTL_GET_DELAY     0x00010011 /* return in bytes as the vio interface */
                                              /* does not know anything about streams */

#define ROAR_VIO_CTL_GET_DBLOCKS   0x00020001 /* get Driver Blocks */
#define ROAR_VIO_CTL_SET_DBLOCKS   0x00020002 /* set Driver Blocks */
#define ROAR_VIO_CTL_GET_DBLKSIZE  0x00020003 /* get Driver Blocks size (in byte) */

// sys io:

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
 int     (*ctl     )(struct roar_vio_calls * vio, int cmd, void * data);
 int     (*close   )(struct roar_vio_calls * vio);
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
int     roar_vio_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     roar_vio_close   (struct roar_vio_calls * vio);

// converters:
int     roar_vio_open_file     (struct roar_vio_calls * calls, char * filename, int flags, mode_t mode);
int     roar_vio_open_fh       (struct roar_vio_calls * calls, int fh);

int     roar_vio_simple_stream (struct roar_vio_calls * calls, int rate, int channels, int bits, int codec,
                                                               char * server, int dir, char * name);

int     roar_vio_open_stdio    (struct roar_vio_calls * calls, FILE * dst);

FILE *  roar_vio_to_stdio      (struct roar_vio_calls * calls, int flags);


// possible VIOs:

// basic
ssize_t roar_vio_basic_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_basic_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_basic_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_basic_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_basic_sync    (struct roar_vio_calls * vio);
int     roar_vio_basic_close   (struct roar_vio_calls * vio);

// null
// this is a read and write in one!
ssize_t roar_vio_null_rw    (struct roar_vio_calls * vio, void *buf, size_t count);

// pass

int     roar_vio_open_pass    (struct roar_vio_calls * calls, struct roar_vio_calls * dst);
ssize_t roar_vio_pass_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_pass_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_pass_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_pass_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_pass_sync    (struct roar_vio_calls * vio);
int     roar_vio_pass_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     roar_vio_pass_close   (struct roar_vio_calls * vio);

// re-read/write

int     roar_vio_open_re (struct roar_vio_calls * calls, struct roar_vio_calls * dst);
ssize_t roar_vio_re_read (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_re_write(struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_re_lseek(struct roar_vio_calls * vio, off_t offset, int whence);

// stdio
ssize_t roar_vio_stdio_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_stdio_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_stdio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_stdio_sync    (struct roar_vio_calls * vio);
int     roar_vio_stdio_close   (struct roar_vio_calls * vio);

#endif

//ll
