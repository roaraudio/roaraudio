//emul_rsound.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND

static int emul_rsound_lastcon = -1;

int emul_rsound_new_client  (int client, int data);

int emul_rsound_on_connect  (int fh, struct roard_listen * lsock) {
 int oldfh = emul_rsound_lastcon;
 int client;
 union {
  int32_t i[2];
  char c[8];
 } buf;

 if ( emul_rsound_lastcon == -1 ) {
  emul_rsound_lastcon = fh;
  return -2;
 } else {
  emul_rsound_lastcon = -1;

  client = clients_new();

  if ( client == -1 )
   return -1;

  if ( clients_set_fh(client, fh) == -1 ) {
   ROAR_ERR("net_get_new_client(void): Can not set client's fh");

   clients_delete(client);
   close(oldfh);
   close(fh);

   ROAR_DBG("net_get_new_client(void) = -1");
   return -1;
  }

  if ( emul_rsound_new_client(client, oldfh) == -1 ) {
   clients_delete(client);
   return -1;
  }

  buf.i[0] = ROAR_HOST2NET32(0);
  buf.i[1] = ROAR_HOST2NET32(512);

  ROAR_NETWORK_WRITE(oldfh, buf.c, 8);

  return client;
 }

 return -1;
}

int emul_rsound_new_client  (int client, int data) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 struct roar_client        *  c;
 int stream;

 if ( clients_get(client, &c) == -1 ) {
  return -1;
 }

 if ((stream = streams_new()) == -1 ) {
  clients_delete(client);
  return -1;
 }

 if ( streams_get(stream, &ss) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 s = ROAR_STREAM(ss);

 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 ss->codec_orgi = s->info.codec = ROAR_CODEC_DEFAULT;

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  clients_delete(client);
  return -1;
 }

/*
 if ( client_stream_exec(client, stream) == -1 ) {
  clients_delete(client);
  return -1;
 }
*/

 if ( client_stream_set_fh(client, stream, data) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 return 0;
}

#endif

//ll
