//httpd.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroareio a part of RoarAudio,
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

#ifndef _LIBROAREIO_HTTPD_H_
#define _LIBROAREIO_HTTPD_H_

#include "libroareio.h"

#define _ROAR_EIO_HTTPD_VERSION(major,minor) ((major)<<8|(minor))

#define ROAR_HTTPD_VERSION_HTTP09    _ROAR_EIO_HTTPD_VERSION(0,9)
#define ROAR_HTTPD_VERSION_HTTP10    _ROAR_EIO_HTTPD_VERSION(1,0)
#define ROAR_HTTPD_VERSION_HTTP11    _ROAR_EIO_HTTPD_VERSION(1,1)

#define ROAR_HTTPD_METHOD_NONE       0
#define ROAR_HTTPD_METHOD_GET        1
#define ROAR_HTTPD_METHOD_POST       2
#define ROAR_HTTPD_METHOD_PUT        3
#define ROAR_HTTPD_METHOD_HEAD       4
#define ROAR_HTTPD_METHOD_DELETE     5
#define ROAR_HTTPD_METHOD_TRACE      6
#define ROAR_HTTPD_METHOD_OPTIONS    7
#define ROAR_HTTPD_METHOD_CONNECT    8
//#define ROAR_HTTPD_METHOD_       9

#define ROAR_HTTPD_STATUS_OK         200
#define ROAR_HTTPD_STATUS_NOT_FOUND  404
#define ROAR_HTTPD_STATUS_INTSERVERR 500
#define ROAR_HTTPD_STATUS_NOT_IMPL   501
#define ROAR_HTTPD_STATUS_VERNOTSUP  505

struct roar_httpd_request {
 int method;
 char * resource;
 char * query_string;
 int version;
 struct roar_keyval * header;
};

struct roar_httpd_response {
 int version;
 int status;
 struct roar_vio_calls * file;
 struct roar_keyval    * header;
};

struct roar_httpd {
 struct roar_httpd_request;
 struct roar_httpd_response;
 int (*cb_eoh)(struct roar_httpd * httpd);
 struct roar_vio_calls * client;
 struct roar_vio_calls   viostore[1];
 struct roar_buffer * header;
 struct roar_buffer * output;
};

struct roar_httpd * roar_http_new(struct roar_vio_calls * client, int (*cb_eoh)(struct roar_httpd * httpd));
int                 roar_http_free(struct roar_httpd * httpd);
int                 roar_http_update(struct roar_httpd * httpd);

#endif

//ll
