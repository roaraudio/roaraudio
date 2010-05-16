//vio.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
#include <sys/ioctl.h>

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
 ROAR_DBG("roar_vio_write(vio=%p, buf=%p, count=%u) = ?", vio, buf, (unsigned int)count);

 if ( vio == NULL )
  return -1;

 if ( vio->write == NULL )
  return -1;

 return vio->write(vio, buf, count);
}

off_t   roar_vio_lseek(struct roar_vio_calls * vio, off_t offset, int whence) {
 ROAR_DBG("roar_vio_lseek(vio=%p, offset=%u, whence=%i) = ?", vio, (unsigned int)offset, whence);

 if ( vio == NULL )
  return -1;

 if ( vio->lseek == NULL )
  return -1;

 return vio->lseek(vio, offset, whence);
}

int     roar_vio_nonblock(struct roar_vio_calls * vio, int state) {
 ROAR_DBG("roar_vio_nonblock(vio=%p, state=%i) = ?", vio, state);

 if ( vio == NULL )
  return -1;

 if ( vio->nonblock == NULL )
  return -1;

 return vio->nonblock(vio, state);
}

int     roar_vio_sync    (struct roar_vio_calls * vio) {
 ROAR_DBG("roar_vio_sync(vio=%p) = ?", vio);

 if ( vio == NULL )
  return -1;

 if ( vio->sync == NULL )
  return -1;

 return vio->sync(vio);
}

int     roar_vio_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 ROAR_DBG("roar_vio_ctl(vio=%p, cmd=%i, data=%p) = ?", vio, cmd, data);

 if ( vio == NULL )
  return -1;

 ROAR_DBG("roar_vio_ctl(vio=%p, cmd=0x%.8x, data=%p): vio->ctl=%p", vio, cmd, data, vio->ctl);

 if ( vio->ctl == NULL )
  return -1;

 return vio->ctl(vio, cmd, data);
}

int     roar_vio_close    (struct roar_vio_calls * vio) {
 ROAR_DBG("roar_vio_close(vio=%p) = ?", vio);

 if ( vio == NULL )
  return -1;

 if ( vio->close == NULL )
  return -1;

 return vio->close(vio);
}

// specal commands:
int     roar_vio_accept  (struct roar_vio_calls * calls, struct roar_vio_calls * dst) {
 if (dst == NULL || calls == NULL)
  return -1;

 return roar_vio_ctl(dst, ROAR_VIO_CTL_ACCEPT, calls);
}

int     roar_vio_shutdown(struct roar_vio_calls * vio,   int how) {
 return roar_vio_ctl(vio, ROAR_VIO_CTL_SHUTDOWN, &how);
}

// converters:
int     roar_vio_open_file     (struct roar_vio_calls * calls, char * filename, int flags, mode_t mode) {
#ifdef _CAN_OPERATE
 int fh;

 if ( calls == NULL || filename == NULL )
  return -1;

#ifdef ROAR_TARGET_WIN32
 flags |= O_BINARY;
#endif

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

 if ( roar_vio_open_fh(calls, fh) == -1 )
  return -1;

#ifdef ROAR_TARGET_WIN32
 calls->read     = roar_vio_winsock_read;
 calls->write    = roar_vio_winsock_write;
 calls->nonblock = roar_vio_winsock_nonblock;
 calls->sync     = roar_vio_winsock_sync;
 calls->ctl      = roar_vio_winsock_ctl;
 calls->close    = roar_vio_winsock_close;
#else
 calls->sync     = roar_vio_null_sync;
#endif

 return 0;
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

int     roar_vio_simple_new_stream_obj (struct roar_vio_calls * calls,
                                        struct roar_connection * con,
                                        struct roar_stream * s,
                                        int rate, int channels, int bits, int codec, int dir) {
 int fh;

 if ( calls == NULL )
  return -1;

 if ( (fh = roar_simple_new_stream_obj(con, s, rate, channels, bits, codec, dir)) == -1 )
  return -1;

 return roar_vio_open_fh_socket(calls, fh);
}

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
 struct roar_vio_sysio_ioctl * sysioctl;
 int tmp;
 int s_r = 0, s_w = 0;

 if ( vio == NULL || cmd == -1 )
  return -1;

 ROAR_DBG("roar_vio_basic_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "basic";
    return 0;
   break;
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_WRITE_FH:
  case ROAR_VIO_CTL_GET_SELECT_FH:
  case ROAR_VIO_CTL_GET_SELECT_READ_FH:
  case ROAR_VIO_CTL_GET_SELECT_WRITE_FH:
    ROAR_DBG("roar_vio_basic_ctl(vio=%p, cmd=ROAR_VIO_CTL_GET_*FH(0x%.8x), data=%p) = 0 // fh=%i", vio, cmd, data, roar_vio_get_fh(vio));
    *(int*)data = roar_vio_get_fh(vio);
    return 0;
   break;
  case ROAR_VIO_CTL_SET_NOSYNC:
    vio->sync = NULL;
    return 0;
   break;
  case ROAR_VIO_CTL_ACCEPT:
    tmp = accept(roar_vio_get_fh(vio), NULL, 0);
    if ( tmp == -1 )
     return -1;

    // most proably a socket.
    if ( roar_vio_open_fh_socket(data, tmp) == -1 ) {
#ifdef ROAR_TARGET_WIN32
     closesocket(tmp);
#else
     close(tmp);
#endif
     return -1;
    }

    return 0;
   break;
  case ROAR_VIO_CTL_SHUTDOWN:
    tmp = *(int*)data;

    if ( tmp & ROAR_VIO_SHUTDOWN_READ ) {
     s_r = 1;
     tmp -= ROAR_VIO_SHUTDOWN_READ;
    }

    if ( tmp & ROAR_VIO_SHUTDOWN_WRITE ) {
     s_w = 1;
     tmp -= ROAR_VIO_SHUTDOWN_WRITE;
    }

    if ( tmp != 0 ) /* we currently only support R and W shutdowns */
     return -1;

    if ( s_r && s_w ) {
     tmp = SHUT_RDWR;
    } else if ( s_r ) {
     tmp = SHUT_RD;
    } else if ( s_w ) {
     tmp = SHUT_WR;
    } else {
     return 0; // nothing to do.
    }

    return shutdown(roar_vio_get_fh(vio), tmp);
   break;
  case ROAR_VIO_CTL_SYSIO_IOCTL:
    sysioctl = data;
    return ioctl(roar_vio_get_fh(vio), sysioctl->cmd, sysioctl->argp);
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

int     roar_vio_null_sync    (struct roar_vio_calls * vio) {
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
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "pass";
    return 0;
   break;
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

//ll
