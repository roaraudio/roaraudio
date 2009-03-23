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

 if ( dstr == NULL )
  dstr = "//";

 if ( roar_vio_dstr_init_defaults(def, ROAR_VIO_DEF_TYPE_SOCKET, O_RDWR, 0644) == -1 )
  return -1;

 if ( roar_vio_socket_init_tcp4_def(def, "localhost", port) == -1 )
  return -1;

 if ( !strncmp(dstr, "//", 2) )
  dstr += 2;

 if ( (ed = strstr(dstr, "/")) != NULL )
  *ed = 0;

 ROAR_WARN("roar_vio_proto_init_def(*): def->o_flags=%i", def->o_flags);

 ret = roar_vio_socket_init_dstr_def(def, dstr, -1, SOCK_STREAM, def);

 ROAR_WARN("roar_vio_proto_init_def(*): def->o_flags=%i", def->o_flags);

 if ( ed != NULL )
  *ed = '/';

 return ret;
}

int roar_vio_open_proto      (struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                              char * dstr, int proto, struct roar_vio_defaults * odef) {
 char * host;
 char * tmp;

 ROAR_WARN("roar_vio_open_proto(calls=%p, dst=%p, dstr='%s', proto=%i, odef=%p) = ?", calls, dst, dstr, proto, odef);

 if ( calls == NULL || dst == NULL || odef == NULL )
  return -1;

 ROAR_WARN("roar_vio_open_proto(*): odef->o_flags=%i", odef->o_flags);
 ROAR_DBG("roar_vio_open_proto(*) = ?");

 if ( roar_vio_open_pass(calls, dst) == -1 )
  return -1;

 ROAR_DBG("roar_vio_open_proto(*) = ?");

 if ( dstr != NULL ) {
  dstr += 2;
  host  = dstr;

  if ( (tmp = strstr(dstr, "/")) == NULL )
   return -1;

  *tmp++ = 0;
  dstr   = tmp;

  if ( (tmp = strstr(dstr, "#")) != NULL )
   *tmp = 0;
 } else {
  ROAR_DBG("roar_vio_open_proto(*): no dstr!, odef->type=%i", odef->type);
  if ( odef->type == ROAR_VIO_DEF_TYPE_FILE ) {
   dstr = odef->d.file;
   host = "localhost";

   for (; *dstr == '/'; dstr++);

  } else if ( odef->type == ROAR_VIO_DEF_TYPE_SOCKET ) {
   dstr = ""; // index document
   host = odef->d.socket.host;
  } else {
   return -1;
  }
 }

 ROAR_DBG("roar_vio_open_proto(*) = ?");
 ROAR_WARN("roar_vio_open_proto(*): proto=%i, host='%s', file='%s'", proto, host, dstr);

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
 char buf[1024];
 char b0[80], b1[80];
 int  status;
 int  len;

 if ( calls == NULL || dst == NULL || host == NULL || file == NULL )
  return -1;

 roar_vio_printf(dst, "GET /%s HTTP/1.1\r\n", file);
 roar_vio_printf(dst, "Host: %s\r\n", host);
 roar_vio_printf(dst, "User-Agent: roar_vio_open_proto_http() $Revision: 1.4 $\r\n");
 roar_vio_printf(dst, "Connection: close\r\n");
 roar_vio_printf(dst, "\r\n");

 roar_vio_sync(dst);

 if ( (len = roar_vio_read(dst, buf, 1023)) < 1 )
  return -1;

 buf[len] = 0;

 if ( sscanf(buf, "%79s %i %79s\n", b0, &status, b1) != 3 ) {
  return -1;
 }

 if ( status != 200 )
  return -1;

 ROAR_WARN("roar_vio_open_proto_http(*): status=%i", status);
// ROAR_WARN("roar_vio_open_proto_http(*): buf='%s'", buf);

 if ( !strcmp((buf+len)-4, "\r\n\r\n") )
  return 0;

 while (*buf != '\r' && *buf != '\n') {
  if ( (len = roar_vio_read(dst, buf, 1023)) < 1 )
   return -1;
 }

 return 0;
}

int roar_vio_open_proto_gopher (struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * host, char * file) {
 if ( calls == NULL || dst == NULL || host == NULL || file == NULL )
  return -1;

 ROAR_DBG("roar_vio_open_proto_gopher(calls=%p, dst=%p, host='%s', file='%s') = ?", calls, dst, host, file);

 if ( file[1] == '/' )
  file += 2;

 roar_vio_printf(dst, "/%s\r\n", file);

 roar_vio_sync(dst); // for encryption/compression layers

 return 0;
}

//ll
