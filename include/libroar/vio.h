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

#define ROAR_VIO_CTL_GET                  0x1
#define ROAR_VIO_CTL_SET                  0x2
#define ROAR_VIO_CTL_CLIENT            0x0000
#define ROAR_VIO_CTL_SERVER            0x1000
#define ROAR_VIO_CTL_GENERIC     (0x0000<<16)
#define ROAR_VIO_CTL_STREAM      (0x0001<<16)
#define ROAR_VIO_CTL_DRIVER      (0x0002<<16)

// basic calls:
#define ROAR_VIO_CTL_GET_NEXT          (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_GET|0x0100)
#define ROAR_VIO_CTL_SET_NEXT          (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_SET|0x0100)
#define ROAR_VIO_CTL_GET_FH            (ROAR_VIO_CTL_GENERIC|0x0110)
#define ROAR_VIO_CTL_GET_READ_FH       (ROAR_VIO_CTL_GENERIC|0x0111)
#define ROAR_VIO_CTL_GET_WRITE_FH      (ROAR_VIO_CTL_GENERIC|0x0112)
#define ROAR_VIO_CTL_SELECT            (ROAR_VIO_CTL_GENERIC|0x0120)

// stream:
#define ROAR_VIO_CTL_SET_STREAM    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_SET) /* normal streams */
#define ROAR_VIO_CTL_GET_STREAM    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_GET)

#define ROAR_VIO_CTL_SET_SSTREAM   (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_SET) /* server streams */
#define ROAR_VIO_CTL_GET_SSTREAM   (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_GET)
#define ROAR_VIO_CTL_SET_SSTREAMID (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_SET|0x10) /* server streams */
#define ROAR_VIO_CTL_GET_SSTREAMID (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_GET|0x10)

#define ROAR_VIO_CTL_SET_AUINFO    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SET|0x2000) /* set a struct roar_audio_info */
#define ROAR_VIO_CTL_GET_AUINFO    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_GET|0x2000) /* get a struct roar_audio_info */

#define ROAR_VIO_CTL_GET_DELAY     (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_GET|0x010) /* return in bytes as the vio interface */
                                              /* does not know anything about streams */

#define ROAR_VIO_CTL_GET_DBLOCKS   (ROAR_VIO_CTL_DRIVER|0x0001) /* get Driver Blocks */
#define ROAR_VIO_CTL_SET_DBLOCKS   (ROAR_VIO_CTL_DRIVER|0x0002) /* set Driver Blocks */
#define ROAR_VIO_CTL_GET_DBLKSIZE  (ROAR_VIO_CTL_DRIVER|0x0003) /* get Driver Blocks size (in byte) */
#define ROAR_VIO_CTL_SET_DBLKSIZE  (ROAR_VIO_CTL_DRIVER|0x0004) /* set Driver Blocks size (in byte) */

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

// some alias functions:
#define roar_vio_puts(vio,s) roar_vio_write((vio), (s), strlen((s)))
//#define roar_vio_putc(vio,c) roar_vio_write((vio), &(c), 1)
int     roar_vio_putc    (struct roar_vio_calls * vio, char c);
int     roar_vio_getc    (struct roar_vio_calls * vio);

int     roar_vio_printf  (struct roar_vio_calls * vio, const char *format, ...);

// converters:
int     roar_vio_open_file     (struct roar_vio_calls * calls, char * filename, int flags, mode_t mode);
int     roar_vio_open_fh       (struct roar_vio_calls * calls, int fh);
int     roar_vio_open_fh_socket(struct roar_vio_calls * calls, int fh);

int     roar_vio_open_socket   (struct roar_vio_calls * calls, char * host, int port);
int     roar_vio_open_socket_listen(struct roar_vio_calls * calls, int type, char * host, int port);

int     roar_vio_simple_stream (struct roar_vio_calls * calls, int rate, int channels, int bits, int codec,
                                                               char * server, int dir, char * name);

int     roar_vio_open_stdio    (struct roar_vio_calls * calls, FILE * dst);

FILE *  roar_vio_to_stdio      (struct roar_vio_calls * calls, int flags);
#if defined(ROAR_HAVE_FOPENCOOKIE) || defined(ROAR_HAVE_FUNOPEN)
int roar_vio_to_stdio_close (void *__cookie);
#endif
#if defined(ROAR_HAVE_FOPENCOOKIE)
__ssize_t roar_vio_to_stdio_read (void *__cookie, char *__buf, size_t __nbytes);
__ssize_t roar_vio_to_stdio_write (void *__cookie, __const char *__buf, size_t __n);
int roar_vio_to_stdio_lseek (void *__cookie, _IO_off64_t *__pos, int __w);
#elif defined(ROAR_HAVE_FUNOPEN)
int roar_vio_to_stdio_read(void *__cookie, char *__buf, int __nbytes);
int roar_vio_to_stdio_write(void *__cookie, const char *__buf, int __n);
fpos_t roar_vio_to_stdio_lseek(void *__cookie, fpos_t __pos, int __w);
#endif


// possible VIOs:

// basic
ssize_t roar_vio_basic_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_basic_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_basic_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_basic_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_basic_sync    (struct roar_vio_calls * vio);
int     roar_vio_basic_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
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
int     roar_vio_stdio_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     roar_vio_stdio_close   (struct roar_vio_calls * vio);

#endif

//ll
