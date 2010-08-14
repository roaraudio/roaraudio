//vs.c:

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

#define FLAG_NONE     0x0000
#define FLAG_STREAM   0x0001

#define _seterr(x) do { if ( error != NULL ) *error = (x); } while(1)

struct roar_vs {
 int flags;
 struct roar_connection con_store;
 struct roar_connection * con;
 struct roar_stream       stream;
 struct roar_vio_calls    vio;
};

const char * roar_vs_strerr(int error) {
 const struct {
  int err;
  const char * msg;
 } msgs[] = {
  {ROAR_ERROR_NONE,    "none"},
  {ROAR_ERROR_PERM,    "perm"},
  {ROAR_ERROR_NOENT,   "noent"},
  {ROAR_ERROR_BADMSG,  "badmsg"},
  {ROAR_ERROR_BUSY,    "busy"},
  {ROAR_ERROR_CONNREFUSED, "connrefused"},
  {ROAR_ERROR_NOSYS,   "nosys"},
  {ROAR_ERROR_NOTSUP,  "notsup"},
  {ROAR_ERROR_PIPE,    "pipe"},
  {ROAR_ERROR_PROTO,   "proto"},
  {ROAR_ERROR_RANGE,   "range"},
  {ROAR_ERROR_MSGSIZE, "msgsize"},
  {ROAR_ERROR_NOMEM,   "nomem"},
  {ROAR_ERROR_INVAL,   "inval"},
  {-1, NULL}
 };
 int i;

 for (i = 0; msgs[i].msg != NULL; i++)
  if ( msgs[i].err == error )
   return msgs[i].msg;

 return "(unknown)";
}

static roar_vs_t * roar_vs_init(int * error) {
 roar_vs_t * vss = roar_mm_malloc(sizeof(roar_vs_t));

 if ( vss == NULL ) {
  _seterr(ROAR_ERROR_NOMEM);
  return NULL;
 }

 memset(vss, 0, sizeof(roar_vs_t));

 return vss;
}

roar_vs_t * roar_vs_new_from_con(struct roar_connection * con, int * error) {
 roar_vs_t * vss = roar_vs_init(error);

 if ( vss == NULL )
  return NULL;

 vss->con = con;

 return vss;
}

roar_vs_t * roar_vs_new(const char * server, const char * name, int * error) {
 roar_vs_t * vss = roar_vs_init(error);
 int ret;

 if ( vss == NULL )
  return NULL;

 vss->con = &(vss->con_store);

 ret = roar_simple_connect(vss->con, (char*)server, (char*)name);

 if ( ret == -1 ) {
  roar_vs_close(vss, ROAR_VS_TRUE, NULL);
  _seterr(ROAR_ERROR_UNKNOWN);
  return NULL;
 }

 return vss;
}

int roar_vs_stream(roar_vs_t * vss, const struct roar_audio_info * info, int dir, int * error) {
 int ret;

 if ( vss->flags & FLAG_STREAM ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 ret = roar_vio_simple_new_stream_obj(&(vss->vio), vss->con, &(vss->stream),
                                      info->rate, info->channels, info->bits, info->codec,
                                      dir
                                     );

 if ( ret == -1 ) {
  _seterr(ROAR_ERROR_UNKNOWN);
  return -1;
 }

 vss->flags |= FLAG_STREAM;

 return 0;
}

roar_vs_t * roar_vs_new_simple(const char * server, const char * name, int rate, int channels, int codec, int bits, int dir, int * error) {
 roar_vs_t * vss = roar_vs_new(server, name, error);
 struct roar_audio_info info;
 int ret;

 if (vss == NULL)
  return NULL;

 memset(&info, 0, sizeof(info));

 info.rate     = rate;
 info.channels = channels;
 info.codec    = codec;
 info.bits     = bits;

 ret = roar_vs_stream(vss, &info, dir, error);

 if (ret == -1) {
  roar_vs_close(vss, ROAR_VS_TRUE, NULL);
  return NULL;
 }

 return vss;
}

int roar_vs_close(roar_vs_t * vss, int killit, int * error) {
 if ( vss->flags & FLAG_STREAM ) {
  if ( killit ) {
   roar_kick(vss->con, ROAR_OT_STREAM, roar_stream_get_id(&(vss->stream)));
  }

  roar_vio_close(&(vss->vio));
 }

 if ( vss->con == &(vss->con_store) ) {
  roar_disconnect(vss->con);
 }

 roar_mm_free(vss);
 return 0;
}

ssize_t roar_vs_write(roar_vs_t * vss, const void * buf, size_t len, int * error) {
 ssize_t ret;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 ret = roar_vio_write(&(vss->vio), (void*)buf, len);

 if ( ret == -1 ) {
  _seterr(ROAR_ERROR_UNKNOWN);
 }

 return ret;
}

ssize_t roar_vs_read (roar_vs_t * vss,       void * buf, size_t len, int * error) {
 ssize_t ret;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 ret = roar_vio_read(&(vss->vio), buf, len);

 if ( ret == -1 ) {
  _seterr(ROAR_ERROR_UNKNOWN);
 }

 return ret;
}


//ll
