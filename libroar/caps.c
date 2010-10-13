//caps.c:

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

int roar_caps_to_msg(struct roar_message * mes,  struct roar_caps    * caps, void ** data) {
 char * datap;
 size_t needlen = 4;

 if ( mes == NULL || caps == NULL )
  return -1;

 needlen += caps->len;

 if ( needlen > sizeof(mes->data) ) {
  if ( data == NULL )
   return -1;

  if ( (*data = malloc(needlen)) == NULL )
   return -1;

  datap = *data;
 } else {
  datap = mes->data;
 }

 datap[0] = caps->version;
 datap[1] = caps->type;
 datap[2] = (caps->flags & 0xFF00) >> 8;
 datap[3] = caps->flags & 0xFF;

 if ( caps->len != 0 ) {
  memcpy(&(datap[4]), caps->data, caps->len);
 }

 mes->datalen = needlen;

 return 0;
}

int roar_caps_from_msg(struct roar_caps  * caps, struct roar_message * mes,  void  * data) {
 char * datap;

 if ( mes == NULL || caps == NULL )
  return -1;

 if ( data != NULL ) {
  datap = data;
 } else {
  datap = mes->data;
 }

 // versin check.
 if ( datap[0] != 0 || mes->datalen < 4 )
  return -1;

 memset(caps, 0, sizeof(struct roar_caps));

 caps->version = datap[0];
 caps->type    = datap[1];
 caps->flags   = (datap[2] << 8) | datap[3];

 if ( mes->datalen == 4 ) {
  caps->data    = NULL;
  caps->len     = 0;
 } else {
  caps->data    = &(datap[4]);
  caps->len     = mes->datalen - 4;
 }

 return 0;
}

int roar_caps_stds(struct roar_connection * con, struct roar_stds ** out, struct roar_stds * in, int flags) {
 struct roar_message mes;
 struct roar_caps caps;
 char * data = NULL;
 size_t i;

 if ( flags == -1 )
  flags = 0;

 if ( con == NULL )
  return -1;

 if ( out != NULL )
  flags |= ROAR_CF_REQUEST;

 if ( (flags & ROAR_CF_REQUEST) && out == NULL )
  return -1;

 if ( in != NULL )
  if ( in->stds_len == 0 )
   in = NULL;

 memset(&caps, 0, sizeof(caps));

 caps.version = 0;
 caps.type    = ROAR_CT_STANDARDS;
 caps.flags   = flags;

 memset(&mes, 0, sizeof(mes));

 if ( in != NULL ) {
  // we use a hack here to avoid double alloc:
  // first pass the data in native byte order.
  // after we got a buffer swap in-buffer.
  caps.data = in->stds;
  caps.len  = in->stds_len * 4;
 }

 if ( roar_caps_to_msg(&mes, &caps, &data) == -1 )
  return -1;

 if ( in != NULL ) {
  if ( data == NULL ) {
   for (i = 0; i < in->stds_len; i++) {
    ((uint32_t*)mes.data)[i+1] = ROAR_HOST2NET32(((uint32_t*)mes.data)[i+1]);
   }
  } else {
   for (i = 0; i < in->stds_len; i++) {
    ((uint32_t*)data)[i+1] = ROAR_HOST2NET32(((uint32_t*)data)[i+1]);
   }
  }
 }

 mes.cmd = ROAR_CMD_CAPS;

 if ( roar_req(con, &mes, &data) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK ) {
  if ( data != NULL )
   free(data);
  return -1;
 }

 if ( roar_caps_from_msg(&caps, &mes, data) == -1 ) {
  if ( data != NULL )
   free(data);
  return -1;
 }

 // check if response matches the request:
 if ( caps.version != 0 || caps.type != ROAR_CT_STANDARDS || (caps.len & 0x3) != 0 ) {
  if ( data != NULL )
   free(data);
  return -1;
 }

 if ( out != NULL ) {
  *out = roar_stds_new(caps.len/4);
  if ( *out == NULL ) {
   if ( data != NULL )
    free(data);
   return -1;
  }

  for (i = 0; i < (*out)->stds_len; i++) {
   (*out)->stds[i] = ROAR_NET2HOST32(((uint32_t*)caps.data)[i+1]);
  }
 }

 if ( data != NULL )
  free(data);

 return 0;
}

struct roar_stds * roar_stds_new(size_t len) {
 struct roar_stds * ret;

 if ( len == 0 )
  return NULL;

 ret = roar_mm_malloc(sizeof(struct roar_stds));

 if ( ret == NULL )
  return NULL;

 memset(ret, 0, sizeof(struct roar_stds));

 ret->stds_len = len;
 ret->stds     = roar_mm_malloc(len*sizeof(uint32_t));

 if ( ret->stds == NULL ) {
  roar_mm_free(ret);
  return NULL;
 }

 memset(ret->stds, 0, len*sizeof(uint32_t));

 return ret;
}

int roar_stds_free(struct roar_stds * stds) {
 if ( stds == NULL )
  return -1;

 if ( stds->stds != NULL )
  roar_mm_free(stds->stds);

 roar_mm_free(stds);

 return 0;
}

//ll
