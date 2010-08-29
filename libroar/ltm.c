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

struct roar_ltm_result {
 int mt;
 int window;
 size_t streams;
 size_t nummt;
 size_t structlen;
 int64_t data[1];
};

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


#ifdef DEBUG
#define HEXDUMP64(x) roar_ltm_hexdump64((x))
static void roar_ltm_hexdump64(void * p) {
 unsigned char * d = p;
 ROAR_DBG("roar_ltm_hexdump64(p=%p): hex: %.2x%.2x %.2x%.2x %.2x%.2x %.2x%.2x", p, d[0], d[1], d[2], d[3], d[4], d[5], d[6], d[7]);
}
#else
#define HEXDUMP64(x)
#endif

struct roar_ltm_result * roar_ltm_get(struct roar_connection * con,
                                      int mt, int window,
                                      int * streams, size_t slen,
                                      struct roar_ltm_result * oldresult) {
 struct roar_ltm_result * res = NULL;
 struct roar_message mes;
 size_t needed_structlen = 0;
 int64_t  * d64;
 char * buf = NULL;
 int    ret;
 int    i;

 if ( con == NULL )
  return NULL;

 if ( streams == NULL || slen == 0 )
  return NULL;

 if ( roar_ltm_pack_req(mt, window, streams, slen, &mes, &buf, ROAR_LTM_SST_GET_RAW) == -1 )
  return NULL;

 ret = roar_req(con, &mes, &buf);

 if ( ret == -1 || mes.cmd != ROAR_CMD_OK ) {
  if ( buf != NULL )
   free(buf);
  return NULL;
 }

 if ( buf == NULL ) {
  d64 = ( int64_t*)&(mes.data);
 } else {
  d64 = ( int64_t*)buf;
 }


 for (i = 0; i < mes.datalen/8; i++) {
  ROAR_DBG("roar_ltm_get(*): d64[i=%i]=%lli", i, (long long int)d64[i]);
  HEXDUMP64(&(d64[i]));
  d64[i] = ROAR_NET2HOST64(d64[i]);
  HEXDUMP64(&(d64[i]));
  ROAR_DBG("roar_ltm_get(*): d64[i=%i]=%lli", i, (long long int)d64[i]);
 }

 needed_structlen = sizeof(struct roar_ltm_result) + mes.datalen;

 if ( oldresult != NULL ) {
  if ( oldresult->structlen >= needed_structlen ) {
   res = oldresult;
   needed_structlen = oldresult->structlen;
  } else {
   roar_ltm_freeres(oldresult);
  }
 } else {
  res = roar_mm_malloc(needed_structlen);
 }

 if ( res == NULL ) {
  if ( buf != NULL )
   free(buf);
  return NULL;
 }

 memset(res, 0, needed_structlen);
 res->structlen = needed_structlen;

 res->mt      = mt;
 res->window  = window;
 res->streams = slen;
 res->nummt   = roar_ltm_numbits(mt);

 memcpy(res->data, d64, mes.datalen);

 if ( buf != NULL )
  free(buf);
 return res;
}

#define _CKNULL(x) if ( (x) == NULL ) return -1
#define _RETMEMBERCKED(x,m) _CKNULL(x); return (x)->m

int roar_ltm_get_numstreams(struct roar_ltm_result * res) {
 _RETMEMBERCKED(res, streams);
}
int roar_ltm_get_mt(struct roar_ltm_result * res) {
 _RETMEMBERCKED(res, mt);
}
int roar_ltm_get_window(struct roar_ltm_result * res) {
 _RETMEMBERCKED(res, window);
}

static int64_t * roar_ltm_get_streamptr(struct roar_ltm_result * res, int streamidx) {
 int64_t * ptr;
 int numchans;
 int i;

 ROAR_DBG("roar_ltm_get_streamptr(res=%p, streamidx=%i) = ?", res, streamidx);

 if ( res == NULL || streamidx < 0 || streamidx >= res->streams )
  return NULL;

 ptr = res->data;

 ROAR_DBG("roar_ltm_get_streamptr(res=%p, streamidx=%i): res->data=%p", res, streamidx, res->data);

 for (i = 0; i < streamidx; i++) {
  numchans = *ptr & 0xFFFF;

  ROAR_DBG("roar_ltm_get_streamptr(res=%p, streamidx=%i): i=%i, numchans=%i", res, streamidx, i, numchans);

  ptr += res->nummt * numchans;
  ptr++;
 }

 ROAR_DBG("roar_ltm_get_streamptr(res=%p, streamidx=%i) = %p", res, streamidx, ptr);
 return ptr;
}

int roar_ltm_get_numchans(struct roar_ltm_result * res, int streamidx) {
 int64_t * ptr = roar_ltm_get_streamptr(res, streamidx);

 if ( ptr == NULL )
  return -1;

 return *ptr & 0xFFFF;
}

int64_t roar_ltm_extract(struct roar_ltm_result * res, int mt, int streamidx, int channel) {
 int64_t * ptr = roar_ltm_get_streamptr(res, streamidx);
 int numchans;
 int resmt;

 ROAR_DBG("roar_ltm_extract(res=%p, mt=0x%.4x, streamidx=%i, channel=%i) = ?", res, mt, streamidx, channel);

 if ( roar_ltm_numbits(mt) != 1 )
  return -1;

 ROAR_DBG("roar_ltm_extract(res=%p, mt=0x%.4x, streamidx=%i, channel=%i) = ?", res, mt, streamidx, channel);

 if ( ptr == NULL )
  return -1;

 numchans = *ptr & 0xFFFF;

 ROAR_DBG("roar_ltm_extract(res=%p, mt=0x%.4x, streamidx=%i, channel=%i): numchans=%i", res, mt, streamidx, channel, numchans);

 if ( channel >= numchans )
  return -1;

 ptr++;

 ptr += res->nummt * channel;

 // now ptr points to the first mt for the given channel.

 resmt = res->mt;

 while (!(mt & 0x1)) {
  if ( resmt & 0x1 )
   ptr++;
  mt    >>= 1;
  resmt >>= 1;
 }

 ROAR_DBG("roar_ltm_extract(res=%p, mt=?, streamidx=%i, channel=%i): ptr=%p", res, streamidx, channel, ptr);
 ROAR_DBG("roar_ltm_extract(res=%p, mt=?, streamidx=%i, channel=%i) = %lli", res, streamidx, channel, (long long int)*ptr);

 return *ptr;
}

//ll
