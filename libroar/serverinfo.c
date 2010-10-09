//serverinfo.c:

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

struct ie {
 int type;
 size_t len;
 char * buf;
};

struct roar_server_info * roar_server_info(struct roar_connection * con) {
 struct roar_server_info * ret;
 struct roar_message mes;
 uint16_t * d16;
 char * data = NULL;

 memset(&mes, 0, sizeof(mes));

 mes.cmd = ROAR_CMD_SERVER_INFO;
 mes.datalen = 2*2;
 d16 = (uint16_t*)mes.data;

 d16[0] = ROAR_HOST2NET16(0); // version
 d16[1] = ROAR_HOST2NET16(ROAR_IT_SERVER);

 if ( roar_req(con, &mes, &data) != 0 )
  return NULL;

 if ( mes.cmd != ROAR_CMD_OK )
  return NULL;

 ret = roar_server_info_from_mes(&mes, data);

 if ( data != NULL )
  free(data);

 return ret;
}

int roar_server_info_free(struct roar_server_info * info) {
 if ( info == NULL )
  return -1;

 roar_mm_free(info);

 return 0;
}

#define _add(t, m) do { if ( info->m != NULL && (sl = strlen(info->m)) != 0 ) { iebuf[idx].type = (t); iebuf[idx].len = sl; iebuf[idx].buf = (info->m); idx++; needlen += 4 + sl; } } while (0)

int roar_server_info_to_mes(struct roar_message * mes, struct roar_server_info * info, void ** data) {
 size_t needlen = 4;
 size_t sl;
 int idx = 0;
 struct ie iebuf[sizeof(struct roar_server_info)/sizeof(char*)];
 uint16_t * d16, * dptr;
 char * textpart;
 int i;
 char * mesdata;

 if ( mes == NULL || info == NULL )
  return -1;

 _add(ROAR_ITST_VERSION, version);
 _add(ROAR_ITST_LOCATION, location);
 _add(ROAR_ITST_DESCRIPTION, description);
 _add(ROAR_ITST_CONTACT, contact);
 _add(ROAR_ITST_SERIAL, serial);
 _add(ROAR_ITST_ADDRESS, address);
 _add(ROAR_ITST_UIURL, uiurl);
 _add(ROAR_ITST_UN_SYSNAME, un.sysname);
 _add(ROAR_ITST_UN_RELEASE, un.release);
 _add(ROAR_ITST_UN_NODENAME, un.nodename);
 _add(ROAR_ITST_UN_MACHINE, un.machine);

 if ( needlen > LIBROAR_BUFFER_MSGDATA ) {
  if ( data == NULL )
   return -1;

  mesdata = malloc(needlen);

  if ( mesdata == NULL )
   return -1;
 } else {
  mesdata = mes->data;
 }

 memset(mes, 0, sizeof(struct roar_message));

 mes->datalen = needlen;

 d16 = (uint16_t*)mesdata;

 mesdata[0] = 0; // version
 mesdata[1] = 0; // reserved

 d16[1] = ROAR_HOST2NET16(idx);

 dptr = &(d16[2]);

 for (i = 0; i < idx; i++) {
  dptr[0] = ROAR_HOST2NET16(iebuf[i].type & 0xFF);
  dptr[1] = ROAR_HOST2NET16(iebuf[i].len);
  dptr += 2;
 }

 textpart = mesdata + (4 + 4*idx);

 for (i = 0; i < idx; i++) {
  memcpy(textpart, iebuf[i].buf, iebuf[i].len);
  textpart += iebuf[i].len;
 }

 return 0;
}

struct roar_server_info * roar_server_info_from_mes(struct roar_message * mes, void * data) {
 struct ie iebuf[sizeof(struct roar_server_info)/sizeof(char*)];
 struct ie * ieptr;
 struct roar_server_info * ret = NULL;
 uint16_t * d16, * dptr;
 int idx;
 int i;
 size_t needlen = 4;
 char * textpart;
 char * textbuf;
 char ** tptr;
 char * mesdata;

 ROAR_DBG("roar_server_info(mes=%p{.datalen=%llu) = ?", mes, (long long unsigned int)mes->datalen);

 if ( mes == NULL )
  return NULL;

 if ( data == NULL ) {
  mesdata = mes->data;
 } else {
  mesdata = data;
 }

 memset(iebuf, 0, sizeof(iebuf));

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 // some basic texts like version:
 if ( mes->datalen < needlen )
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 if ( mesdata[0] != 0 ) /* version */
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 if ( mesdata[1] != 0 ) /* reserved */
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 d16 = (uint16_t*)mesdata;

 idx = ROAR_NET2HOST16(d16[1]);

 ROAR_DBG("roar_server_info(mes=%p): idx=%i", mes, idx);

 // return error if our index buffer is too short.
 if ( idx > (sizeof(iebuf)/sizeof(*iebuf)) )
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 ROAR_DBG("roar_server_info(mes=%p): needlen=%llu", mes, (long long unsigned int)needlen);

 needlen += 4*idx;

 // recheck if we have a complet index.
 if ( mes->datalen < needlen )
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 d16 = (uint16_t*)mesdata;
 dptr = &(d16[2]);

 textpart = mesdata + (4 + 4*idx);

 ROAR_DBG("roar_server_info(mes=%p): needlen=%llu", mes, (long long unsigned int)needlen);

 for (i = 0; i < idx; i++) {
  iebuf[i].type = ROAR_NET2HOST16(dptr[0]) & 0xFF;
  iebuf[i].len  = ROAR_NET2HOST16(dptr[1]);
  iebuf[i].buf  = textpart;
  needlen  += iebuf[i].len;
  textpart += iebuf[i].len;
  dptr += 2;
  ROAR_DBG("roar_server_info(mes=%p): iebuf[i]={.len=%llu,...}", mes, (long long unsigned int)iebuf[i].len);
 }

 ROAR_DBG("roar_server_info(mes=%p): needlen=%llu", mes, (long long unsigned int)needlen);

 // recheck if we have all the data...
 if ( mes->datalen < needlen )
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 // alloc the needed space. this can be reduced in future to the actual needed value.
 ret = roar_mm_malloc(2*sizeof(struct roar_server_info) + mes->datalen);

 if ( ret == NULL )
  return NULL;

 ROAR_DBG("roar_server_info(mes=%p) = ?", mes);

 // for the size see the alloc call above.
 memset(ret, 0, 2*sizeof(struct roar_server_info) + mes->datalen);

 textbuf = (char*)ret + sizeof(struct roar_server_info);

 for (i = 0; i < idx; i++) {
  ieptr = &(iebuf[i]);

   // ignore empty fields
  if ( ieptr->len == 0 )
   continue;
  if ( ieptr->len == 1 && ieptr->buf[0] == 0 )
   continue;

#define _ck(TYPE,member) case TYPE: tptr = &(ret->member); break;
  switch (ieptr->type) {
   _ck(ROAR_ITST_VERSION, version);
   _ck(ROAR_ITST_LOCATION, location);
   _ck(ROAR_ITST_DESCRIPTION, description);
   _ck(ROAR_ITST_CONTACT, contact);
   _ck(ROAR_ITST_SERIAL, serial);
   _ck(ROAR_ITST_ADDRESS, address);
   _ck(ROAR_ITST_UIURL, uiurl);
   _ck(ROAR_ITST_UN_SYSNAME, un.sysname);
   _ck(ROAR_ITST_UN_RELEASE, un.release);
   _ck(ROAR_ITST_UN_NODENAME, un.nodename);
   _ck(ROAR_ITST_UN_MACHINE, un.machine);
   default:
     continue;
    break;
  }
#undef _ck

  *tptr = textbuf;
   memcpy(textbuf, ieptr->buf, ieptr->len);
   textbuf += ieptr->len;
   *textbuf = 0; // set \0
   textbuf++;
 }

 ROAR_DBG("roar_server_info(mes=%p) = %p", mes, ret);

 return ret;
}


//ll
