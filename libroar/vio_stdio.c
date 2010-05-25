//vio_stdio.c:

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

// stdio:
#ifndef ROAR_WITHOUT_VIO_STDIO
ssize_t roar_vio_stdio_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 return fread(buf, 1, count, (FILE*)(vio->inst));
}

ssize_t roar_vio_stdio_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 return fwrite(buf, 1, count, (FILE*)(vio->inst));
}

off_t   roar_vio_stdio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence) {
#if defined(ROAR_HAVE_FSEEK) && defined(ROAR_HAVE_FTELL)
 if ( fseek((FILE*)(vio->inst), offset, whence) == -1 )
  return -1;

 return ftell((FILE*)(vio->inst));
#else
 return (off_t)-1;
#endif
}

int     roar_vio_stdio_sync    (struct roar_vio_calls * vio) {
 return fflush((FILE*)(vio->inst));
}

int     roar_vio_stdio_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {

 if ( vio == NULL || cmd == -1 )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "stdio";
    return 0;
   break;
#ifdef ROAR_HAVE_FILENO
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_WRITE_FH:
  case ROAR_VIO_CTL_GET_SELECT_FH:
  case ROAR_VIO_CTL_GET_SELECT_READ_FH:
  case ROAR_VIO_CTL_GET_SELECT_WRITE_FH:
   *(int*)data = fileno((FILE*)(vio->inst));
    return 0;
   break;
#endif
 }

 return -1;
}

int     roar_vio_stdio_close   (struct roar_vio_calls * vio) {
 return fclose((FILE*)(vio->inst));
}

#endif

//ll
