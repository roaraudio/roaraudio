//vio.c:

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

#include "libroar.h"

#ifdef ROAR_HAVE_IO_POSIX
#define _CAN_OPERATE
#endif

int roar_vio_init_calls (struct roar_vio_calls * calls) {
#ifdef _CAN_OPERATE
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
 calls->ctl      = roar_vio_basic_ctl;
 calls->close    = roar_vio_basic_close;

 return 0;
#else
 return -1;
#endif
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
 ROAR_DBG("roar_vio_read(vio=%p, buf=%p, count=%u) = ?", vio, buf, (unsigned int)count);

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

 ROAR_DBG("roar_vio_ctl(vio=%p, cmd=0x%.8x, data=%p): vio->ctl=%p", vio, cmd, data, vio->ctl);

 if ( vio->ctl == NULL )
  return -1;

 return vio->ctl(vio, cmd, data);
}

int     roar_vio_close    (struct roar_vio_calls * vio) {
 if ( vio == NULL )
  return -1;

 if ( vio->close == NULL )
  return -1;

 return vio->close(vio);
}

int     roar_vio_putc    (struct roar_vio_calls * vio, char c) {
 return roar_vio_write(vio, &c, 1);
}

int     roar_vio_getc    (struct roar_vio_calls * vio) {
 unsigned char c;

 if ( roar_vio_read(vio, &c, 1) != 1 )
  return EOF;

 return c;
}

int     roar_vio_printf(struct roar_vio_calls * vio, const char *format, ...) {
 va_list ap;
 int ret;
 char buf[8192];

 va_start(ap, format);
 ret = vsnprintf(buf, 8192, format, ap);
 va_end(ap);

 return roar_vio_write(vio, buf, ret);
}

// converters:
int     roar_vio_open_file     (struct roar_vio_calls * calls, char * filename, int flags, mode_t mode) {
#ifdef _CAN_OPERATE
 int fh;

 if ( calls == NULL || filename == NULL )
  return -1;

 if ( (fh = open(filename, flags, mode)) == -1 )
  return -1;

 if ( roar_vio_open_fh(calls, fh) == -1 ) {
  close(fh);
  return -1;
 }

 return 0;
#else
 return -1;
#endif
}

int     roar_vio_open_fh       (struct roar_vio_calls * calls, int fh) {
 if ( calls == NULL )
  return -1;

 if ( roar_vio_init_calls(calls) == -1 )
  return -1;

 return roar_vio_set_fh(calls, fh);
}

int     roar_vio_open_fh_socket(struct roar_vio_calls * calls, int fh) {
 if ( calls == NULL )
  return -1;

 return roar_vio_open_fh(calls, fh);
}

int     roar_vio_open_socket   (struct roar_vio_calls * calls, char * host, int port) {
 int fh;

 if ( calls == NULL )
  return -1;

 if ( (fh = roar_socket_connect(host, port)) == -1 )
  return -1;

 return roar_vio_open_fh_socket(calls, fh);
}

int     roar_vio_open_socket_listen(struct roar_vio_calls * calls, int type, char * host, int port) {
 int fh;

 if ( calls == NULL )
  return -1;

 if ( (fh = roar_socket_listen(type, host, port)) == -1 )
  return -1;

 return roar_vio_open_fh_socket(calls, fh);
}

int     roar_vio_simple_stream (struct roar_vio_calls * calls, int rate, int channels, int bits, int codec,
                                                               char * server, int dir, char * name) {
 int fh;

 if ( calls == NULL )
  return -1;

 if ( (fh = roar_simple_stream(rate, channels, bits, codec, server, dir, name)) == -1 )
  return -1;

 return roar_vio_open_fh_socket(calls, fh);
}


int     roar_vio_open_stdio    (struct roar_vio_calls * calls, FILE * dst) {
#ifndef ROAR_WITHOUT_VIO_STDIO
 if ( calls == NULL || dst == NULL )
  return -1;

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->read  = roar_vio_stdio_read;
 calls->write = roar_vio_stdio_write;
 calls->lseek = roar_vio_stdio_lseek;
 calls->sync  = roar_vio_stdio_sync;
 calls->close = roar_vio_stdio_close;

 calls->inst  = dst;

 return 0;
#else
 return -1;
#endif
}

FILE *  roar_vio_to_stdio      (struct roar_vio_calls * calls, int flags) {
#ifdef ROAR_HAVE_FOPENCOOKIE
 cookie_io_functions_t foc_funcs;
#endif

 if ( calls == NULL )
  return NULL;

#if defined(ROAR_HAVE_FOPENCOOKIE)
 memset(&foc_funcs, 0, sizeof(cookie_io_functions_t));

 foc_funcs.close = roar_vio_to_stdio_close;
 foc_funcs.read  = roar_vio_to_stdio_read;
 foc_funcs.write = roar_vio_to_stdio_write;

 return fopencookie((void*) calls, "rw", foc_funcs);
#elif defined(ROAR_HAVE_FUNOPEN)
 return funopen((void*) calls, roar_vio_to_stdio_read,  roar_vio_to_stdio_write,
                               roar_vio_to_stdio_lseek, roar_vio_to_stdio_close);
#else
 return NULL;
#endif
}

#if defined(ROAR_HAVE_FOPENCOOKIE) || defined(ROAR_HAVE_FUNOPEN)
int roar_vio_to_stdio_close (void *__cookie) {
 return roar_vio_close((struct roar_vio_calls *) __cookie);
}

#if defined(ROAR_HAVE_FOPENCOOKIE)
__ssize_t roar_vio_to_stdio_read (void *__cookie, char *__buf, size_t __nbytes) {
#elif defined(ROAR_HAVE_FUNOPEN)
int roar_vio_to_stdio_read(void *__cookie, char *__buf, int __nbytes) {
#endif
 return roar_vio_read((struct roar_vio_calls *) __cookie, __buf, __nbytes);
}

#if defined(ROAR_HAVE_FOPENCOOKIE)
__ssize_t roar_vio_to_stdio_write (void *__cookie, __const char *__buf, size_t __n) {
#elif defined(ROAR_HAVE_FUNOPEN)
int roar_vio_to_stdio_write(void *__cookie, const char *__buf, int __n) {
#endif
 return roar_vio_write((struct roar_vio_calls *) __cookie, (char *) __buf, __n);
}

#if defined(ROAR_HAVE_FOPENCOOKIE)
int roar_vio_to_stdio_lseek (void *__cookie, _IO_off64_t *__pos, int __w);
#elif defined(ROAR_HAVE_FUNOPEN)
fpos_t roar_vio_to_stdio_lseek(void *__cookie, fpos_t __pos, int __w) {
 return roar_vio_lseek((struct roar_vio_calls *) __cookie, __pos, __w);
}
#endif
#endif

// VIOs:

// basic
ssize_t roar_vio_basic_read (struct roar_vio_calls * vio, void *buf, size_t count) {
#ifdef _CAN_OPERATE
 return read(roar_vio_get_fh(vio), buf, count);
#else
 return -1;
#endif
}

ssize_t roar_vio_basic_write(struct roar_vio_calls * vio, void *buf, size_t count) {
#ifdef _CAN_OPERATE
 return write(roar_vio_get_fh(vio), buf, count);
#else
 return -1;
#endif
}

off_t   roar_vio_basic_lseek(struct roar_vio_calls * vio, off_t offset, int whence) {
#ifdef _CAN_OPERATE
 return lseek(roar_vio_get_fh(vio), offset, whence);
#else
 return -1;
#endif
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
#ifdef ROAR_FDATASYNC
 return ROAR_FDATASYNC(roar_vio_get_fh(vio));
#else
 return 0;
#endif
}

int     roar_vio_basic_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {

 if ( vio == NULL || cmd == -1 )
  return -1;

 ROAR_DBG("roar_vio_basic_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_WRITE_FH:
    ROAR_DBG("roar_vio_basic_ctl(vio=%p, cmd=ROAR_VIO_CTL_GET_*FH(0x%.8x), data=%p) = 0 // fh=%i", vio, cmd, data, roar_vio_get_fh(vio));
    *(int*)data = roar_vio_get_fh(vio);
    return 0;
   break;
 }

 return -1;
}

int     roar_vio_basic_close    (struct roar_vio_calls * vio) {
#ifdef _CAN_OPERATE
 if ( roar_vio_get_fh(vio) != -1 )
  return close(roar_vio_get_fh(vio));

 return 0;
#else
 return -1;
#endif
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
 calls->ctl      = roar_vio_pass_ctl;
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
 if (vio == NULL || cmd == -1)
  return -1;

 ROAR_DBG("roar_vio_pass_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NEXT:
    *(struct roar_vio_calls **)data = vio->inst;
    return 0;
   break;
  case ROAR_VIO_CTL_SET_NEXT:
    vio->inst = *(struct roar_vio_calls **)data;
    return 0;
   break;
 }

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

// stdio:
#ifndef ROAR_WITHOUT_VIO_STDIO
ssize_t roar_vio_stdio_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 return fread(buf, 1, count, (FILE*)(vio->inst));
}

ssize_t roar_vio_stdio_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 return fwrite(buf, 1, count, (FILE*)(vio->inst));
}

off_t   roar_vio_stdio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
 if ( fseek((FILE*)(vio->inst), offset, whence) == -1 )
  return -1;

 return ftell((FILE*)(vio->inst));
}

int     roar_vio_stdio_sync    (struct roar_vio_calls * vio) {
 return fflush((FILE*)(vio->inst));
}

int     roar_vio_stdio_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {

 if ( vio == NULL || cmd == -1 )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_WRITE_FH:
   *(int*)data = fileno((FILE*)(vio->inst));
    return 0;
   break;
 }

 return -1;
}

int     roar_vio_stdio_close   (struct roar_vio_calls * vio) {
 return fclose((FILE*)(vio->inst));
}
#endif

//ll
