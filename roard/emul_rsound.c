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
  int32_t i[4];
  char c[16];
 } buf;

 // TODO: add error handling
 roar_socket_nonblock(fh, ROAR_SOCKET_NONBLOCK);

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

  // LATENCY:
  // we currently don't know what we need to set here.
  buf.i[0] = ROAR_HOST2NET32(0);

  // block size used by roard.
  buf.i[1] = ROAR_HOST2NET32(ROAR_OUTPUT_BUFFER_SAMPLES * roar_info2framesize(g_sa) / 8);

  // Magic zeros to enable protocol handling.
  // (RSD_CONN_PROTO)
  buf.i[2] = ROAR_HOST2NET32(0);
  buf.i[3] = ROAR_HOST2NET32(0);

  ROAR_NETWORK_WRITE(oldfh, buf.c, sizeof(buf.c));

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

 ss->codec_orgi = s->info.codec = ROAR_CODEC_RIFF_WAVE;

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

int emul_rsound_vrecv_msg(struct emul_rsound_msg * msg, struct roar_vio_calls * vio) {
 ssize_t ret;
 int     num;

 if ( msg == NULL || vio == NULL )
  return -1;

 ret = roar_vio_read(vio, msg->header, EMUL_RSOUND_MSG_HEADER_LEN);

 if ( ret != EMUL_RSOUND_MSG_HEADER_LEN )
  return -1;

 msg->header[ret] = 0;

 if ( msg->header[0] != 'R' || msg->header[1] != 'S' || msg->header[2] != 'D' )
  return -1;

 if ( sscanf(&(msg->header[3]), "%5d", &num) != 1 )
  return -1;

 if ( num > EMUL_RSOUND_MSG_DATA_LEN )
  return -1;

 msg->datalen = num;

 ret = roar_vio_read(vio, msg->data, num);

 if ( ret != (ssize_t)num )
  return -1;

 msg->data[num] = 0;

 msg->datasp   = msg->data;
 msg->dataslen = msg->datalen;

 for (num = 0; msg->data[num] == ' '; num++) {
  msg->datasp++;
  msg->dataslen--;
 }

 return 0;
}

int emul_rsound_vsend_msg(struct emul_rsound_msg * msg, struct roar_vio_calls * vio) {
 ssize_t ret;

 if ( msg == NULL || vio == NULL )
  return -1;

 snprintf(msg->header, EMUL_RSOUND_MSG_HEADER_LEN+1, "RSD%5d", msg->datalen);

 ret = roar_vio_write(vio, msg->header, EMUL_RSOUND_MSG_HEADER_LEN);

 if ( ret != EMUL_RSOUND_MSG_HEADER_LEN )
  return -1;

 ret = roar_vio_write(vio, msg->data, msg->datalen);

 if ( ret != (ssize_t)msg->datalen )
  return -1;

 return 0;
}

int emul_rsound_check_client(int client, struct roar_vio_calls * vio) {
 struct roar_vio_calls     rvio;
 struct emul_rsound_msg     msg;
 struct roar_client        *  c;
 struct roar_stream_server * ss;
 int                   streamid;
 int                          i;
 ssize_t                    ptr;
 size_t                 max_len;

 if ( vio == NULL ) {
  vio = &rvio;
  roar_vio_open_fh_socket(vio, clients_get_fh(client));
 }

 // we get called in a loop, in case this fails no problem, just
 // return -1, caller will delete us in case of real error.
 if ( emul_rsound_vrecv_msg(&msg, vio) == -1 )
  return -1;

 if ( !strncmp(msg.datasp, "INFO", 4) ) {
  if ( clients_get(client, &c) == -1 )
   return clients_delete(client);

  streamid = -1;
  for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++)
   if ( c->streams[i] > streamid )
    streamid = c->streams[i];

  if ( streamid == -1 )
   return clients_delete(client);

  if ( streams_get(streamid, &ss) == -1 )
   return clients_delete(client);

  ptr = roar_info2samplesize(&(ROAR_STREAM(ss)->info));

  if ( ptr == -1 )
   return clients_delete(client);

  ptr *= ROAR_STREAM(ss)->pos;
  ptr /= 8; // bits -> bytes

  i = snprintf(msg.data+msg.datalen, EMUL_RSOUND_MSG_DATA_LEN - msg.datalen, " %lld", (long long int)ptr);

  msg.datalen += i;

  return emul_rsound_vsend_msg(&msg, vio);
 } else if ( !strncmp(msg.datasp, "NULL", 4) ) {
  // NULL is simular to NOOP
 } else if ( !strncmp(msg.datasp, "STOP", 4) ) {
  // This is quit.
  return clients_delete(client);
 } else if ( !strncmp(msg.datasp, "IDENTITY ", 9) ) {
  if ( msg.dataslen < (8+1+1) )
   return clients_delete(client);

  msg.datasp   += 9;
  msg.dataslen -= 9;

  if ( clients_get(client, &c) == -1 )
   return clients_delete(client);

  max_len = msg.dataslen < (ROAR_BUFFER_NAME-1) ? msg.dataslen : (ROAR_BUFFER_NAME-1);

  strncpy(c->name, msg.datasp, max_len);
  c->name[max_len] = 0;
 } else {
  // Unknown command, kill the client.
  return clients_delete(client);
 }

 return 0;
}
#endif

//ll
