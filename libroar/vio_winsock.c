//vio_winsock.c:

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

#ifdef ROAR_TARGET_WIN32
ssize_t roar_vio_winsock_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 return recv(roar_vio_get_fh(vio), buf, count, 0);
}

ssize_t roar_vio_winsock_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 return send(roar_vio_get_fh(vio), buf, count, 0);
}

int     roar_vio_winsock_nonblock(struct roar_vio_calls * vio, int state) {
 return -1;
}
int     roar_vio_winsock_sync    (struct roar_vio_calls * vio) {
 return 0;
}

int     roar_vio_winsock_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 if ( vio == NULL || cmd == -1 )
  return -1;

 ROAR_DBG("roar_vio_winsock_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "winsock";
    return 0;
   break;
  case ROAR_VIO_CTL_GET_SELECT_FH:
  case ROAR_VIO_CTL_GET_SELECT_READ_FH:
  case ROAR_VIO_CTL_GET_SELECT_WRITE_FH:
    ROAR_DBG("roar_vio_winsock_ctl(vio=%p, cmd=ROAR_VIO_CTL_GET_SELECT_*FH(0x%.8x), data=%p) = 0 // fh=%i", vio, cmd, data, roar_vio_get_fh(vio));
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
     closesocket(tmp);
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
 }

 return -1;
}

int     roar_vio_winsock_close   (struct roar_vio_calls * vio) {

 closesocket(roar_vio_get_fh(vio));

 return 0;
}
#endif

//ll
