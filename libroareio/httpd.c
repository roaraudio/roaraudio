//httpd.c:

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
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroareio.h"

struct roar_httpd * roar_http_new(struct roar_vio_calls * client, int (*cb_eoh)(struct roar_httpd * httpd)) {
 struct roar_httpd * httpd;

 if ( client == NULL || cb_eoh == NULL )
  return NULL;

 if ( (httpd = roar_mm_malloc(sizeof(struct roar_httpd))) == NULL )
  return NULL;

 memset(httpd, 0, sizeof(struct roar_httpd));

 httpd->state  = ROAR_HTTPD_STATE_REQ;
 httpd->client = client;
 httpd->cb_eoh = cb_eoh;

 return NULL;
}

int                 roar_http_free(struct roar_httpd * httpd) {
 if ( httpd->response.file != NULL )
  roar_vio_close(httpd->response.file);

 if ( httpd->client != NULL )
  roar_vio_close(httpd->client);

 roar_mm_free(httpd);

 return 0;
}

// TODO: update this function to use iobuffer:
static int roar_http_eoh (struct roar_httpd * httpd) {
 struct roar_httpd_request  * req  = &(httpd->request);
 struct roar_httpd_response * resp = &(httpd->response);
 int i;

 // TODO: parse HTTP here

 resp->version = req->version;
 resp->status  = -1;
 httpd->cb_eoh(httpd);

 if ( resp->status == -1 ) {
  if ( resp->file == NULL ) {
   resp->status = ROAR_HTTPD_STATUS_NOT_FOUND;
  } else {
   resp->status = ROAR_HTTPD_STATUS_OK;
  }
 }

 roar_vio_printf(httpd->client, "HTTP/%i.%i %i State %i\r\n", _ROAR_EIO_HTTPD_VERSION_MAJOR(resp->version),
                                                              _ROAR_EIO_HTTPD_VERSION_MINOR(resp->version),
                                                              resp->status, resp->status);

 if ( resp->header != NULL ) {
  for (i = 0; resp->header[i].key != NULL; i++) {
   roar_vio_printf(httpd->client, "%s: %s\r\n", resp->header[i].key, resp->header[i].value);
  }
  roar_mm_free(resp->header);
 }

 return 0;
}

int                 roar_http_update(struct roar_httpd * httpd) {
 void *  data;
 size_t  len;
 ssize_t ret;

 switch (httpd->state) {
  case ROAR_HTTPD_STATE_REQ:
   break;
  case ROAR_HTTPD_STATE_EOH:
    return roar_http_eoh(httpd);
   break;
  case ROAR_HTTPD_STATE_RESP:
   if ( httpd->iobuffer != NULL ) {
    if ( roar_buffer_get_len(httpd->iobuffer, &len) == -1 )
     return -1;
   } else {
    len = 0;
   }

   if ( len != 0 ) {
    if ( roar_buffer_get_data(httpd->iobuffer, &data) == -1 )
     return -1;

    if ( (ret = roar_vio_write(httpd->client, data, len)) == -1 )
     return -1;

    if ( roar_buffer_set_offset(httpd->iobuffer, ret) == -1 )
     return -1; // bad error

    return 0;
   }

   if ( httpd->iobuffer != NULL ) {
    len = 2048;
    if ( roar_buffer_set_len(httpd->iobuffer, len) == -1 )
     return -1;
   } else {
    if ( roar_buffer_new(&(httpd->iobuffer), len) == -1 )
     return -1;
   }

   if ( roar_buffer_get_data(httpd->iobuffer, &data) == -1 )
    return -1;

   if ( (ret = roar_vio_read(httpd->response.file, data, len)) == -1 ) {
    if ( roar_buffer_set_len(httpd->iobuffer, 0) == -1 )
     return -1; // bad error
   }

   len = ret;

   if ( roar_buffer_set_len(httpd->iobuffer, len) == -1 )
    return -1;

   if ( (ret = roar_vio_write(httpd->client, data, len)) == -1 )
    return -1;

   if ( roar_buffer_set_offset(httpd->iobuffer, ret) == -1 )
    return -1; // bad error

   return 0;
   break;
 }

 return -1;
}


//ll
