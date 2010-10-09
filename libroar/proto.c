//proto.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#define _ROAR_MESS_BUF_LEN_V0  (1 /* version */ + 1 /* cmd */ + 2 /* stream */ + 4 /* pos */ + 2 /* datalen */)
#define _ROAR_MESS_BUF_LEN_V1  (1 /* version */ + 1 /* cmd */ + 2 /* stream */ + 4 /* pos */ + 2 /* datalen */ + \
                                1 /* flags */)
#define _ROAR_MESS_BUF_LEN_V2  (1 /* version */ + 1 /* cmd */ + 2 /* stream */ + 8 /* pos */ + 2 /* datalen */ + \
                                4 /* flags */   + 2 /* seq */)
#define _ROAR_MESS_BUF_LEN     _ROAR_MESS_BUF_LEN_V0
#define _ROAR_MESS_BUF_LEN_MAX ROAR_MAX3(_ROAR_MESS_BUF_LEN_V0, _ROAR_MESS_BUF_LEN_V1, _ROAR_MESS_BUF_LEN_V2)

#define _ROAR_MESS_CRC_LEN_V0  0 /* No CRC */
#define _ROAR_MESS_CRC_LEN_V1  1
#define _ROAR_MESS_CRC_LEN_V2  4
#define _ROAR_MESS_CRC_LEN_MAX ROAR_MAX3(_ROAR_MESS_CRC_LEN_V0, _ROAR_MESS_CRC_LEN_V1, _ROAR_MESS_CRC_LEN_V2)

int roar_send_message (struct roar_connection * con, struct roar_message * mes, char * data) {
 struct roar_vio_calls * vio;

 if ( (vio = roar_get_connection_vio2(con)) == NULL )
  return -1;

 return roar_vsend_message(vio, mes, data);
}

int roar_vsend_message(struct roar_vio_calls * vio, struct roar_message * mes, char *  data) {
 char buf[_ROAR_MESS_BUF_LEN_MAX];
 char crc[_ROAR_MESS_CRC_LEN_MAX];
 size_t headerlen = 0;
 size_t crclen    = 0;
 char * bufptr;

 roar_errno = ROAR_ERROR_UNKNOWN;

 ROAR_DBG("roar_send_message(*): try to send an request...");

 headerlen = _ROAR_MESS_BUF_LEN;

 mes->version = _ROAR_MESSAGE_VERSION;

 buf[0] = mes->version; // first byte is always the version.

 switch (mes->version) {
  case 0:
    buf[1]              = (unsigned char) mes->cmd;
    *(uint16_t*)(buf+2) = ROAR_HOST2NET16(mes->stream);
    *(uint32_t*)(buf+4) = ROAR_HOST2NET32(mes->pos);
    *(uint16_t*)(buf+8) = ROAR_HOST2NET16(mes->datalen);
    headerlen           = _ROAR_MESS_BUF_LEN_V0;
   break;
  case 1:
    buf[1]              = (unsigned char) (mes->flags & 0xFF);
    buf[2]              = (unsigned char)  mes->cmd;
    // ...
    roar_errno = ROAR_ERROR_NOTSUP;
    return -1;
   break;
  case 2:
    headerlen           = 16;
    buf[1]              = (unsigned char) mes->cmd;
    *(uint16_t*)(buf+2) = ROAR_HOST2NET16(mes->stream);
    *(uint32_t*)(buf+4) = ROAR_HOST2NET32(mes->flags);
    if ( mes->flags & ROAR_MF_LSPOS ) {
     *(uint64_t*)(buf+8) = ROAR_HOST2NET16(mes->pos64);
     bufptr = buf+16;
     headerlen          += 4;
    } else {
     *(uint32_t*)(buf+8) = ROAR_HOST2NET16(mes->pos);
     bufptr = buf+12;
    }
    *(uint16_t*)(bufptr+0) = ROAR_HOST2NET16(mes->datalen);
    *(uint16_t*)(bufptr+2) = ROAR_HOST2NET16(mes->seq);
   break;
  default:
    roar_errno = ROAR_ERROR_NOTSUP;
    return -1;
 }

 if ( roar_vio_write(vio, buf, headerlen) != headerlen ) {
  roar_errno = ROAR_ERROR_PIPE;
  return -1;
 }

 if ( mes->datalen != 0 ) {
  if ( roar_vio_write(vio, data == NULL ? mes->data : data, mes->datalen) != mes->datalen ) {
   roar_errno = ROAR_ERROR_PIPE;
   return -1;
  }
 }

 if ( crclen != 0 ) {
  if ( roar_vio_write(vio, crc, crclen) != crclen ) {
   roar_errno = ROAR_ERROR_PIPE;
   return -1;
  }
 }

 roar_errno = ROAR_ERROR_NONE;

 ROAR_DBG("roar_send_message(*) = 0");
 return 0;
}

int roar_recv_message (struct roar_connection * con, struct roar_message * mes, char ** data) {
 struct roar_vio_calls * vio;

 if ( (vio = roar_get_connection_vio2(con)) == NULL )
  return -1;

 return roar_vrecv_message(vio, mes, data);
}

int roar_vrecv_message(struct roar_vio_calls * vio, struct roar_message * mes, char ** data) {
 // TODO: add CRC support.
 char buf[_ROAR_MESS_BUF_LEN_MAX];
// char crc[_ROAR_MESS_CRC_LEN_MAX];
 size_t headerlen = 0;
// size_t crclen    = 0;
 size_t needlen;
 char * bufptr;

 roar_errno = ROAR_ERROR_UNKNOWN;

 ROAR_DBG("roar_recv_message(*): try to get a response form the server...");

 if ( mes == NULL )
  return -1;

 if ( data != NULL )
  *data = NULL;

 memset(mes, 0, sizeof(struct roar_message));

 if ( roar_vio_read(vio, buf, _ROAR_MESS_BUF_LEN_V0) != _ROAR_MESS_BUF_LEN_V0 ) {
  roar_errno = ROAR_ERROR_PROTO;
  return -1;
 }

 ROAR_DBG("roar_recv_message(*): Got a header");

 mes->version = buf[0];
 headerlen    = _ROAR_MESS_BUF_LEN_V0;

 switch (mes->version) {
  case 0:
    // we already have all data, so create the struct
    mes->cmd     = (unsigned char)buf[1];
    mes->stream  = ROAR_NET2HOST16(*(uint16_t*)(buf+2));
    mes->pos     = ROAR_NET2HOST32(*(uint32_t*)(buf+4));
    mes->datalen = ROAR_NET2HOST16(*(uint16_t*)(buf+8));
   break;
  case 2:
    mes->cmd     = (unsigned char)buf[1];
    mes->stream  = ROAR_NET2HOST16(*(uint16_t*)(buf+2));
    mes->flags   = ROAR_NET2HOST32(*(uint32_t*)(buf+4));
    if ( mes->flags & ROAR_MF_LSPOS ) {
     headerlen = 20;
    } else {
     headerlen = 16;
    }
   break;
  default:
    roar_errno = ROAR_ERROR_PROTO;
    return -1;
   break;
 }

 // check specal case where headerlen < _ROAR_MESS_BUF_LEN_V0, this means that we need to put
 // some data we read as header into the data part of the message.
 if ( headerlen > _ROAR_MESS_BUF_LEN_V0 ) {
  needlen = headerlen - _ROAR_MESS_BUF_LEN_V0;
  if ( roar_vio_read(vio, buf+_ROAR_MESS_BUF_LEN_V0, needlen) != (ssize_t)needlen ) {
   roar_errno = ROAR_ERROR_PROTO;
   return -1;
  }
 }

 switch (mes->version) {
  case 2:
    if ( mes->flags & ROAR_MF_LSPOS ) {
     mes->pos64   = ROAR_NET2HOST32(*(uint64_t*)(buf+8));
     bufptr       = buf+16;
    } else {
     mes->pos     = ROAR_NET2HOST32(*(uint32_t*)(buf+8));
     bufptr       = buf+12;
    }
    mes->datalen = ROAR_NET2HOST16(*(uint16_t*)(bufptr+0));
    mes->seq     = ROAR_NET2HOST16(*(uint16_t*)(bufptr+1));
   break;
 }

 if ( (int16_t)mes->stream == -1 )
  mes->stream = -1;

 ROAR_DBG("roar_recv_message(*): command=%i(%s)", mes->cmd,
           mes->cmd == ROAR_CMD_OK ? "OK" : (mes->cmd == ROAR_CMD_ERROR ? "ERROR" : "UNKNOWN"));

// Below we only have data handling, handling of header is finished:

 if ( mes->datalen == 0 ) {
  ROAR_DBG("roar_recv_message(*): no data in this pkg");
  ROAR_DBG("roar_recv_message(*) = 0");
  roar_errno = ROAR_ERROR_NONE;
  return 0;
 }

 if ( mes->datalen <= LIBROAR_BUFFER_MSGDATA ) {
  if ( roar_vio_read(vio, mes->data, mes->datalen) == mes->datalen ) {
   ROAR_DBG("roar_recv_message(*): Got data!");
   ROAR_DBG("roar_recv_message(*) = 0");
   roar_errno = ROAR_ERROR_NONE;
   return 0;
  }

  roar_errno = ROAR_ERROR_PIPE;
  return -1;
 } else {
  if ( data == NULL ) {
   roar_errno = ROAR_ERROR_MSGSIZE;
   return -1;
  }

  if ( (*data = malloc(mes->datalen)) == NULL ) {
   roar_errno = ROAR_ERROR_NOMEM;
   return -1;
  }

  if ( mes->datalen == 0 ) {
   roar_errno = ROAR_ERROR_NONE;
   return 0;
  }

  if ( roar_vio_read(vio, *data, mes->datalen) == mes->datalen ) {
   ROAR_DBG("roar_recv_message(*): Got data!");
   ROAR_DBG("roar_recv_message(*) = 0");
   roar_errno = ROAR_ERROR_NONE;
   return 0;
  }

  roar_errno = ROAR_ERROR_PIPE;
  return -1;
 }

 // what happened here?
 return -1;
}

int roar_req (struct roar_connection * con, struct roar_message * mes, char ** data) {
 struct roar_vio_calls * vio;

 if ( (vio = roar_get_connection_vio2(con)) == NULL )
  return -1;

 return roar_vreq(vio, mes, data);
}

int roar_vreq         (struct roar_vio_calls * vio, struct roar_message * mes, char ** data) {
 if ( roar_vsend_message(vio, mes, data ? *data : NULL) != 0 )
  return -1;

 if ( data != NULL )
  free(*data);

 roar_vio_sync(vio); // we need to do this becasue of ssl/compressed links

 return roar_vrecv_message(vio, mes, data);
}

//ll
