//vio_proto.c:

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

int roar_vio_proto_init_def  (struct roar_vio_defaults * def, char * dstr, int proto, struct roar_vio_defaults * odef) {
 int                        port = 0;
 int                        ret;
 char                     * ed;

 if ( def == NULL )
  return -1;

 switch (proto) {
  case ROAR_VIO_PROTO_P_HTTP:    port = 80; break;
  case ROAR_VIO_PROTO_P_GOPHER:  port = 70; break;
  default:
    return -1;
 }

 if ( roar_vio_dstr_init_defaults(def, ROAR_VIO_DEF_TYPE_SOCKET, O_RDWR, 0644) == -1 )
  return -1;

 if ( roar_vio_socket_init_tcp4_def(def, "localhost", port) == -1 )
  return -1;

 if ( !strncmp(dstr, "//", 2) )
  dstr += 2;

 if ( (ed = strstr(dstr, "/")) != NULL )
  *ed = 0;

 ret = roar_vio_socket_init_dstr_def(def, dstr, -1, SOCK_STREAM, def);

 if ( ed != NULL )
  *ed = '/';

 return ret;
}

int roar_vio_open_proto      (struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                              char * dstr, int proto, struct roar_vio_defaults * odef) {
 char * host;
 char * tmp;

 ROAR_WARN("roar_vio_open_proto(calls=%p, dst=%p, dstr='%s', proto=%i, odef=%p) = ?", calls, dst, dstr, proto, odef);

 if ( calls == NULL || dst == NULL || dstr == NULL )
  return -1;

 ROAR_DBG("roar_vio_open_proto(*) = ?");

 if ( roar_vio_open_pass(calls, dst) == -1 )
  return -1;

 dstr += 2;
 host  = dstr;

 if ( (tmp = strstr(dstr, "/")) == NULL )
  return -1;

 *tmp++ = 0;
 dstr   = tmp;

 if ( (tmp = strstr(dstr, "#")) != NULL )
  *tmp = 0;

 ROAR_DBG("roar_vio_open_proto(*) = ?");

 switch (proto) {
  case ROAR_VIO_PROTO_P_HTTP:
    return roar_vio_open_proto_http(calls, dst, host, dstr);
   break;
  case ROAR_VIO_PROTO_P_GOPHER:
    return roar_vio_open_proto_gopher(calls, dst, host, dstr);
   break;
 }

 return -1;
}

int roar_vio_open_proto_http   (struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * host, char * file) {
 return -1;
}

int roar_vio_open_proto_gopher (struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * host, char * file) {
 int len;

 if ( calls == NULL || dst == NULL || host == NULL || file == NULL )
  return -1;

 ROAR_DBG("roar_vio_open_proto_gopher(calls=%p, dst=%p, host='%s', file='%s') = ?", calls, dst, host, file);

 len = strlen(file);

 if ( roar_vio_write(dst, file, len) != len )
  return -1;

 if ( roar_vio_write(dst, "\n", 1) != 1 )
  return -1;

 roar_vio_sync(dst); // for encryption/compression layers

 return 0;
}

//ll
