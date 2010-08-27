//ltm.c:

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

static int roar_ltm_numbits(int f) {
 int found = 0;

 while (f) {
  if ( f & 0x1 )
   found++;

  f >>= 1;
 }

 return found;
}

static int roar_ltm_pack_req(int mt, int window,
                             int * streams, size_t slen,
                             struct roar_message * mes, char ** buf,
                             int regtype) {
 uint16_t * data;
 size_t i;

 if ( mt == 0 || streams == NULL || slen == 0 || mes == NULL || buf == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 memset(mes, 0, sizeof(struct roar_message));

 switch (regtype) {
  case ROAR_LTM_SST_REGISTER:
  case ROAR_LTM_SST_UNREGISTER:
    mes->cmd = ROAR_CMD_SET_STREAM_PARA;
   break;
  case ROAR_LTM_SST_GET_RAW:
    mes->cmd = ROAR_CMD_GET_STREAM_PARA;
   break;
  default:
    roar_errno = ROAR_ERROR_NOTSUP;
    return -1;
   break;
 }

 if ( slen == 1 ) {
  mes->stream  = *streams;
  mes->datalen = 6 * 2;
 } else {
  mes->stream = -1;
  mes->datalen = (6 + slen) * 2;
 }

 if ( mes->datalen > LIBROAR_BUFFER_MSGDATA ) {
  roar_errno = ROAR_ERROR_NOTSUP;
  return -1;
 }

 *buf = NULL;

 data = (uint16_t*)mes->data;

 data[0] = 0;
 data[1] = ROAR_STREAM_PARA_LTM;
 data[2] = regtype;
 data[3] = window;
 data[4] = 0;
 data[5] = mt;

 for (i = 0; i < slen; i++) {
  data[6+i] = streams[i];
 }

 for (i = 0; i < (mes->datalen/2); i++) {
  data[i] = ROAR_HOST2NET16(data[i]);
 }

 return 0;
}

static int roar_ltm_regunreg(struct roar_connection * con, int mt, int window, int * streams, size_t slen, int type) {
 struct roar_message mes;
 char * buf = NULL;
 int    ret;

 if ( roar_ltm_pack_req(mt, window, streams, slen, &mes, &buf, type) == -1 )
  return -1;

 ret = roar_req(con, &mes, &buf);

 if ( buf != NULL )
  free(buf);

 if ( ret != -1 ) {
  if ( mes.cmd != ROAR_CMD_OK )
   ret = -1;
 }

 return ret;
}

int roar_ltm_register(struct roar_connection * con, int mt, int window, int * streams, size_t slen) {
 return roar_ltm_regunreg(con, mt, window, streams, slen, ROAR_LTM_SST_REGISTER);
}
int roar_ltm_unregister(struct roar_connection * con, int mt, int window, int * streams, size_t slen) {
 return roar_ltm_regunreg(con, mt, window, streams, slen, ROAR_LTM_SST_UNREGISTER);
}

ssize_t roar_ltm_get_raw(struct roar_connection * con,
                         int mt, int window,
                         int * streams, size_t slen,
                         void * buf, size_t * buflen, int64_t ** array) {
 int bits = roar_ltm_numbits(mt);
 int64_t * buffer = buf;
 size_t i;

 if ( buflen == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 if ( slen == 0 ) {
  *buflen = 0;
  return bits;
 }

 if ( con == NULL || streams == NULL || buf == NULL || array == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 if ( *buflen < (bits * slen) ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 for (i = 0; i < slen; i++) {
  array[i] = &(buffer[bits*i]);
 }

 roar_errno = ROAR_ERROR_NOSYS;
 return -1;
}

int64_t roar_ltm_extract(int64_t * buf, size_t len, int mt, int req) {
 int have, need;

 roar_err_clear();

 have = roar_ltm_numbits(req);

 need = roar_ltm_numbits(mt);

 if ( have != 1 || (mt & req) != req || len != (size_t)need || buf == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return 0;
 }

 while (req && (req & 0x1) == 0) {
  if ( mt & 0x1 )
   buf++;

  mt  <<= 1;
  req <<= 1;
 }

 return *buf;
}

int64_t roar_ltm_extract1(int64_t * buf, size_t len, int mt, int req) {
 roar_err_clear();

 if ( buf == NULL || len != 1 || mt != req ) {
  roar_errno = ROAR_ERROR_INVAL;
  return 0;
 }

 return *buf;
}

//ll
