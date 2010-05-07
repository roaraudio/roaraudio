//vio_proto.c:

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

int roar_vio_proto_init_def  (struct roar_vio_defaults * def, char * dstr, int proto, struct roar_vio_defaults * odef) {
#ifndef ROAR_WITHOUT_VIO_PROTO
 int                        port = 0;
 int                        ret;
 char                     * ed;

 if ( def == NULL )
  return -1;

 switch (proto) {
  case ROAR_VIO_PROTO_P_HTTP:    port =   80; break;
  case ROAR_VIO_PROTO_P_GOPHER:  port =   70; break;
  case ROAR_VIO_PROTO_P_ICY:     port = 8000; break;
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

 ROAR_DBG("roar_vio_proto_init_def(*): def->o_flags=%i", def->o_flags);

 ret = roar_vio_socket_init_dstr_def(def, dstr, -1, SOCK_STREAM, def);

 ROAR_DBG("roar_vio_proto_init_def(*): def->o_flags=%i", def->o_flags);

 if ( ed != NULL )
  *ed = '/';

 ROAR_DBG("roar_vio_proto_init_def(*): dstr='%s'", dstr);

 return ret;
#else
 return -1;
#endif
}

int roar_vio_open_proto      (struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                              char * dstr, int proto, struct roar_vio_defaults * odef) {
#ifndef ROAR_WITHOUT_VIO_PROTO
 struct roar_vio_proto * self;
 char * host;
 char * tmp;

 ROAR_DBG("roar_vio_open_proto(calls=%p, dst=%p, dstr='%s', proto=%i, odef=%p) = ?", calls, dst, dstr, proto, odef);

 if ( calls == NULL || dst == NULL || odef == NULL )
  return -1;

 ROAR_DBG("roar_vio_open_proto(*): odef->o_flags=%i", odef->o_flags);
 ROAR_DBG("roar_vio_open_proto(*) = ?");

 if ( (self = roar_mm_malloc(sizeof(struct roar_vio_proto))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct roar_vio_proto));

 self->next      = dst;

 calls->inst     = self;

 calls->read     = roar_vio_proto_read;
 calls->write    = roar_vio_proto_write;
// calls->lseek    = roar_vio_proto_lseek; // TODO: this is currently not supported
 calls->nonblock = roar_vio_proto_nonblock;
 calls->sync     = roar_vio_proto_sync;
 calls->ctl      = roar_vio_proto_ctl;
 calls->close    = roar_vio_proto_close;

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
 ROAR_DBG("roar_vio_open_proto(*): proto=%i, host='%s', file='%s'", proto, host, dstr);

 switch (proto) {
  case ROAR_VIO_PROTO_P_HTTP:
  case ROAR_VIO_PROTO_P_ICY:
    return roar_vio_open_proto_http(calls, dst, host, dstr);
   break;
  case ROAR_VIO_PROTO_P_GOPHER:
    return roar_vio_open_proto_gopher(calls, dst, host, dstr);
   break;
 }

 ROAR_DBG("roar_vio_open_proto(*) = -1 // no matching protocol");
 return -1;
#else
 return -1;
#endif
}

#ifndef ROAR_WITHOUT_VIO_PROTO
ssize_t roar_vio_proto_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_proto * self = vio->inst;
 ssize_t ret;
 ssize_t have = 0;
 size_t  len;

 if ( self->reader.buffer != NULL ) {
  len = count;
  if ( roar_buffer_shift_out(&(self->reader.buffer), buf, &len) == -1 ) {
   // This is very bad.
   return -1;
  }

  if ( len ) {
   have   = len;
   buf   += len;
   count -= len;
  }
 }

 if ( count == 0 )
  return have;

 ROAR_DBG("roar_vio_proto_read(*): have=%lli, count=%lli", (long long int)have, (long long int)count);

 if ( (ret = roar_vio_read(self->next, buf, count)) == -1 )
  return ret;

 return have + ret;
}

ssize_t roar_vio_proto_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_proto * self = vio->inst;

 return roar_vio_write(self->next, buf, count);
}

// TODO: this is currently not implemented as this is hard to implement with buffers:
off_t   roar_vio_proto_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);

int     roar_vio_proto_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_vio_proto * self = vio->inst;

 /* we can simply use the next layer's nonblock as all we do in addtion *
  * to call there functions are our buffers which do not block normaly  */

 return roar_vio_nonblock(self->next, state);
}

int     roar_vio_proto_sync    (struct roar_vio_calls * vio) {
 struct roar_vio_proto * self = vio->inst;

 return roar_vio_sync(self->next);
}

int     roar_vio_proto_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 struct roar_vio_proto * self = vio->inst;

 if (vio == NULL || cmd == -1)
  return -1;

 ROAR_DBG("roar_vio_proto_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "proto";
    return 0;
   break;
  case ROAR_VIO_CTL_GET_NEXT:
    *(struct roar_vio_calls **)data = self->next;
    return 0;
   break;
  case ROAR_VIO_CTL_SET_NEXT:
    self->next = *(struct roar_vio_calls **)data;
    return 0;
   break;
 }

 return roar_vio_ctl(self->next, cmd, data);
}

int     roar_vio_proto_close   (struct roar_vio_calls * vio) {
 struct roar_vio_proto * self = vio->inst;

 if ( roar_vio_close(self->next) == -1 )
  return -1;

 roar_mm_free(self);

 return 0;
}


int roar_vio_open_proto_http   (struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * host, char * file) {
 struct roar_vio_proto * self;
 struct roar_buffer * bufbuf;
 void * vpbuf;
 char * buf;
 char * endofheader = NULL;
 char b0[80], b1[80];
 int  status;
 int  len;

 ROAR_DBG("roar_vio_open_proto_http(calls=%p, dst=%p, host='%s', file='%s') = ?", calls, dst, host, file);

 if ( calls == NULL || dst == NULL || host == NULL || file == NULL )
  return -1;

 self         = calls->inst;
 calls->write = NULL; // Disable write as we do not support this

 if ( roar_buffer_new_data(&bufbuf, 1024, &vpbuf) == -1 )
  return -1;

 buf = vpbuf;

 ROAR_DBG("roar_vio_open_proto_http(calls=%p, dst=%p, host='%s', file='%s') = ?", calls, dst, host, file);

 roar_vio_printf(dst, "GET /%s HTTP/1.1\r\n", file);
 roar_vio_printf(dst, "Host: %s\r\n", host);
 roar_vio_printf(dst, "User-Agent: roar_vio_open_proto_http() $Revision: 1.17 $\r\n");
 roar_vio_printf(dst, "Connection: close\r\n");
 roar_vio_printf(dst, "\r\n");

 ROAR_DBG("roar_vio_open_proto_http(*) = ?");

 roar_vio_sync(dst);

 ROAR_DBG("roar_vio_open_proto_http(*) = ?");

 if ( (len = roar_vio_read(dst, buf, 1023)) < 1 ) {
  ROAR_DBG("roar_vio_open_proto_http(*) = -1");
  roar_buffer_free(bufbuf);
  return -1;
 }

 buf[len] = 0;

 ROAR_DBG("roar_vio_open_proto_http(*) = ?");

 if ( sscanf(buf, "%79s %i %79s\n", b0, &status, b1) != 3 ) {
  ROAR_DBG("roar_vio_open_proto_http(*) = -1");
  roar_buffer_free(bufbuf);
  return -1;
 }

 ROAR_DBG("roar_vio_open_proto_http(*) = ?");

 if ( status != 200 ) {
  ROAR_DBG("roar_vio_open_proto_http(*) = -1 // status=%i", status);
  roar_buffer_free(bufbuf);
  return -1;
 }

 ROAR_DBG("roar_vio_open_proto_http(*): status=%i", status);
// ROAR_WARN("roar_vio_open_proto_http(*): buf='%s'", buf);

 endofheader = strstr(buf, "\r\n\r\n");

 ROAR_DBG("roar_vio_open_proto_http(*): endofheader=%p\n", endofheader);

 while ( endofheader == NULL ) {
  if ( (len = roar_vio_read(dst, buf, 1023)) < 1 )
   return -1;

  buf[len] = 0;
  endofheader = strstr(buf, "\r\n\r\n");
  ROAR_DBG("roar_vio_open_proto_http(*): endofheader=%p\n", endofheader);
 }

 ROAR_DBG("roar_vio_open_proto_http(*): endofheader=%p\n", endofheader);
 ROAR_DBG("roar_vio_open_proto_http(*): buf=%p\n", buf);

 if ( (endofheader - buf) == (len - 4) ) {
  roar_buffer_free(bufbuf);
  bufbuf = NULL;
 }

 if ( bufbuf != NULL ) {
  roar_buffer_set_offset(bufbuf, endofheader - buf + 4);
  roar_buffer_set_len(bufbuf,    len - (endofheader - buf + 4) - 1);
 }
 self->reader.buffer = bufbuf;

/*
 if ( !strcmp((buf+len)-4, "\r\n\r\n") )
  return 0;

 while (*buf != '\r' && *buf != '\n') {
  if ( (len = roar_vio_read(dst, buf, 1023)) < 1 )
   return -1;
 }
*/

 return 0;
}

int roar_vio_open_proto_gopher (struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * host, char * file) {
 if ( calls == NULL || dst == NULL || host == NULL || file == NULL )
  return -1;

 calls->write = NULL; // Disable write as we do not support this

 ROAR_DBG("roar_vio_open_proto_gopher(calls=%p, dst=%p, host='%s', file='%s') = ?", calls, dst, host, file);

 if ( file[1] == '/' )
  file += 2;

 roar_vio_printf(dst, "/%s\r\n", file);

 roar_vio_sync(dst); // for encryption/compression layers

 return 0;
}
#endif

//ll
