//vio_tantalos.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

int     roar_vio_open_tantalos    (struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                                   char * key, struct roar_vio_defaults * odef) {
#ifdef ROAR_HAVE_LIBSLP
 struct roar_slp_cookie   cookie;
 char * url;
 int i;

 if ( roar_slp_cookie_init(&cookie, NULL) == -1 )
  return -1;

 if ( roar_slp_search(&cookie, ROAR_SLP_URL_TYPE_DOWNLOAD_HTTP) == -1 )
  return -1;

 for (i = 0; i < cookie.matchcount; i++) {
  url = cookie.match[i].url + ROAR_SLP_URL_TYPE_DOWNLOAD_HTTP_LEN - 4;
 }

 return -1;
#else
 return -1;
#endif
}

#ifdef ROAR_HAVE_LIBSLP
ssize_t roar_vio_tantalos_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_tantalos_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_tantalos_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_tantalos_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_tantalos_sync    (struct roar_vio_calls * vio);
int     roar_vio_tantalos_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     roar_vio_tantalos_close   (struct roar_vio_calls * vio);
#endif

//ll
