//req.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

int req_on_noop        (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 mes->cmd     = ROAR_CMD_OK;
 mes->pos     = g_pos;
 mes->datalen = 0;
 return 0;
}

int req_on_identify    (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_client * c;
 int max_len;

 if ( mes->datalen < 1 )
  return -1;

 clients_get(client, &c);

 if ( mes->data[0] == 1 ) {
  if ( c->pid == -1 ) {
   c->pid       = ROAR_NET2HOST32(*(uint32_t*)((mes->data)+1));
   ROAR_DBG("req_on_identify(): new PID: c->pid = %i", c->pid);
  }

  ROAR_DBG("req_on_identify(): final PID: c->pid = %i", c->pid);

  max_len = (mes->datalen - 5) < (ROAR_BUFFER_NAME-1) ? (mes->datalen - 5) : (ROAR_BUFFER_NAME-1);

  strncpy(c->name, mes->data + 5, max_len);
  c->name[max_len] = 0;

  mes->cmd     = ROAR_CMD_OK;
  mes->pos     = g_pos;
  mes->datalen = 0;

  ROAR_DBG("req_on_identify(*): client=%i, pid=%i", client, c->pid);
  ROAR_DBG("req_on_identify(*) = 0");
  return 0;
 }

 return -1;
}

int req_on_auth        (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 // TODO: add code to support some auth.
 mes->cmd     = ROAR_CMD_OK;
 mes->pos     = g_pos;
 mes->datalen = 0;
 return 0;
}


int req_on_whoami      (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 mes->cmd     = ROAR_CMD_OK;
 mes->pos     = g_pos;
 mes->datalen = 1;
 mes->data[0] = client;
 return 0;
}


int req_on_new_stream  (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int stream;
 struct roar_stream * s;
 struct roar_stream * source_stream;
 struct roar_audio_info * info;
 struct roar_audio_info * source_info;

 ROAR_DBG("req_on_new_stream(client=%i, ...): creating stream...", client);
 if ((stream = streams_new()) == -1 )
  return -1;

 ROAR_DBG("req_on_new_stream(client=%i, ...): getting stream...", client);
 if ( streams_get(stream, (struct roar_stream_server **)&s) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 ROAR_DBG("req_on_new_stream(client=%i, ...): set client of stream...", client);
 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 ROAR_DBG("req_on_new_stream(client=%i, ...): loading stream from message...", client);
 if ( roar_stream_m2s(s, mes) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 ROAR_DBG("req_on_new_stream(client=%i, ...): setting id and codec of stream...", client);
 ROAR_STREAM(s)->id = stream; // roar_stream_m2s() resets this
 ROAR_STREAM_SERVER(s)->codec_orgi = ROAR_STREAM(s)->info.codec;

 ROAR_DBG("req_on_new_stream(client=%i, ...): setting direction stream...", client);
 // int streams_set_dir    (int id, int dir, int defaults)
 if ( streams_set_dir(stream, ROAR_STREAM(s)->dir, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 ROAR_DBG("req_on_new_stream(client=%i, ...): setting up direction specific stream settings...", client);
 switch (ROAR_STREAM(s)->dir) {
  case ROAR_DIR_LIGHT_IN:
  case ROAR_DIR_LIGHT_OUT:
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
    info = &(ROAR_STREAM(s)->info);

    info->channels = 0;
    info->bits     = 0;
    info->rate     = 0;
#else
    streams_delete(stream);
    return -1;
#endif

   break;
  case ROAR_DIR_MIDI_IN:
  case ROAR_DIR_MIDI_OUT:
#ifndef ROAR_WITHOUT_DCOMP_MIDI
    info = &(ROAR_STREAM(s)->info);

    info->channels = ROAR_MIDI_CHANNELS_DEFAULT;
    info->bits     = ROAR_MIDI_BITS;
    info->rate     = 0;
#else
    streams_delete(stream);
    return -1;
#endif

   break;

  case ROAR_DIR_RAW_IN:
#ifndef ROAR_WITHOUT_DCOMP_RAW
    if ( ROAR_STREAM(s)->pos_rel_id == -1     ||
         ROAR_STREAM(s)->pos_rel_id == stream ||
         streams_get_dir(ROAR_STREAM(s)->pos_rel_id) != ROAR_DIR_RAW_OUT
       ) {
     ROAR_STREAM(s)->pos_rel_id = -1; // force this here as it will try to delete itself while deleting
                                      // in case rel_id == stream
     streams_delete(stream);
     return -1;
    }
#else
  case ROAR_DIR_RAW_OUT:
    streams_delete(stream);
    return -1;
#endif

   break;
  case ROAR_DIR_THRU:

    if ( ROAR_STREAM(s)->pos_rel_id == -1 || ROAR_STREAM(s)->pos_rel_id == stream ) {
     ROAR_STREAM(s)->pos_rel_id = -1; // force this here as it will try to delete itself while deleting
                                      // in case rel_id == stream
     streams_delete(stream);
     return -1;
    }

    if ( streams_get(ROAR_STREAM(s)->pos_rel_id, (struct roar_stream_server **)&source_stream) == -1 ) {
     streams_delete(stream);
     return -1;
    }

    info        = &(ROAR_STREAM(s)->info);
    source_info = &(ROAR_STREAM(source_stream)->info);

    info->channels = source_info->channels;
    info->bits     = source_info->bits;
    info->rate     = source_info->rate;
    info->codec    = source_info->codec;
    ROAR_STREAM_SERVER(s)->codec_orgi = ROAR_STREAM_SERVER(source_stream)->codec_orgi;

   break;
  case ROAR_DIR_FILTER:
    info        = &(ROAR_STREAM(s)->info);

    if ( ROAR_STREAM(s)->pos_rel_id == -1 ) {
     source_info = g_sa;
    } else {
     if ( streams_get(ROAR_STREAM(s)->pos_rel_id, (struct roar_stream_server **)&source_stream) == -1 ) {
      streams_delete(stream);
      return -1;
     }
     source_info = &(ROAR_STREAM(source_stream)->info);
    }

    if ( info->channels != source_info->channels || info->bits != source_info->bits ||
         info->codec    != source_info->codec    || info->rate != source_info->rate ) {
     // the stream parameters don't match the one of the stream being filtered.
     // -> delete and reject the stream.
     streams_delete(stream);
     return -1;
    }
   break;
 }

 ROAR_DBG("req_on_new_stream(client=%i, ...): returning (OK)...", client);

 mes->cmd     = ROAR_CMD_OK;
 mes->stream  = stream;
 mes->datalen = 0;

 return 0;
}

int req_on_exec_stream (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int r;

 ROAR_DBG("req_on_exec_stream(client=%i, mes={stream=%i,...},...): execing stream", client, mes->stream);


 if ( streams_is_ready(mes->stream) ) {
  flags[1] |= COMMAND_FLAG_OUT_CLOSECON;
 } else {
  if ( (r = client_stream_exec(client, mes->stream)) == -1 )
   return -1;
 }

 ROAR_DBG("req_on_exec_stream(client=%i, mes={stream=%i,...},...): returning (OK)...", client, mes->stream);
 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_con_stream  (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 char   host[80] = {0};
 int    port = 0;
 int    type;
 int    fh;
 int    len;

 if ( mes->datalen < 4 )
  return -1;

 if ( *(mes->data) != 0 )
  return -1;

 if ( mes->datalen > 80 ) // we do not support long messages here
  return -1;

 type = (unsigned)mes->data[1];
 port = ROAR_NET2HOST16(((uint16_t*)mes->data)[1]);

 len = mes->datalen - 4;

 strncpy(host, &(mes->data[4]), len);
 host[len] = 0;

 if ( type > ROAR_SOCKET_TYPE_MAX )
  return -1;

 if ( type == ROAR_SOCKET_TYPE_FILE ) // disabled because of security resons
  return -1;

 if ( type == ROAR_SOCKET_TYPE_FORK ) // why should we connect to ourself?
  return -1;

 ROAR_DBG("req_on_con_stream(*): CONNECT(type=%i, host='%s', port=%i)", type, host, port);

 if ( (fh = roar_socket_open(ROAR_SOCKET_MODE_CONNECT, type, host, port)) == -1 )
  return -1;

 if ( client_stream_set_fh(client, mes->stream, fh) == -1 ) {
  close(fh);
  return 1;
 }

 mes->datalen = 0;
 mes->cmd     = ROAR_CMD_OK;

 return 0;
}

int req_on_passfh      (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int sock = clients_get_fh(client);
 int16_t * d = (int16_t*)mes->data;
 struct roard_listen * lsock;
 int listening;
 int fh;
 int i;

 if ( (fh = roar_socket_recv_fh(sock, NULL, NULL)) == -1 )
  return -1;

 if ( mes->stream != -1 ) { // stream pass:
  if ( client_stream_set_fh(client, mes->stream, fh) == -1 ) {
   close(fh);
   return -1;
  }

  mes->datalen = 0;
  mes->cmd     = ROAR_CMD_OK;

  return 0;
 }

// non-stream pass:

/*
 0: Version,   16
 1: Flags,     16
 2: Protocol,  16
 3: Byteorder, 16
 Options...
*/

 if ( mes->datalen < 4*2 )
  return -1;

 for (i = 0; i < 4; i++) {
  d[i] = ROAR_NET2HOST16(d[i]);
 }

 if ( d[0] != 0 ) // version
  return -1;

 listening = d[1] & ROAR_CLIENTPASS_FLAG_LISTEN;

 if ( listening )
  d[1] -= ROAR_CLIENTPASS_FLAG_LISTEN;

 if ( d[1] != 0 ) // flags
  return -1;

 if ( listening ) {
  if ( get_listen(&lsock, NULL) == -1 ) {
   close(fh);
   return -1;
  }

  roar_vio_open_fh_socket(&(lsock->sock), fh);
  lsock->used   = 1;
  lsock->proto  = d[2];
 } else {
  if ( clients_new_from_fh(fh, d[2], d[3], 1) == -1 )
   return -1;
 }

 mes->datalen = 0;
 mes->cmd     = ROAR_CMD_OK;

 return 0;
}

#ifdef ROAR_SUPPORT_META
int req_on_set_meta    (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int type;
 int mode;
 int namelen, vallen;
 char   val[255+1];
 char   name[ROAR_META_MAX_NAMELEN+1];

 if ( mes->datalen < 3 )
  return -1;

 if ( mes->data[0] != 0 ) // version
  return -1;

 mode = (unsigned) mes->data[1];
 type = (unsigned) mes->data[2];

 ROAR_DBG("req_on_set_meta(*): mode=%i, type=%i", mode, type);

 if ( mode == ROAR_META_MODE_CLEAR ) {
  stream_meta_clear(mes->stream);
  mes->datalen = 0;
  mes->cmd     = ROAR_CMD_OK;
  return 0;
 } else if ( mode == ROAR_META_MODE_DELETE ) { // unsuppoerted at the moment
  return -1;
 } else if ( mode == ROAR_META_MODE_FINALIZE ) {
  stream_meta_finalize(mes->stream);
  mes->datalen = 0;
  mes->cmd     = ROAR_CMD_OK;
  return 0;
 } else if ( mode == ROAR_META_MODE_SET || mode == ROAR_META_MODE_ADD ) {
  if ( mes->datalen < 5 )
   return -1;

  namelen = (unsigned) mes->data[3];
  vallen  = (unsigned) mes->data[4];

  ROAR_DBG("req_on_set_meta(*): namelen=%i, vallen=%i", namelen, vallen);

  if ( mes->datalen < (5 + namelen + vallen) )
   return -1;

  if ( namelen > ROAR_META_MAX_NAMELEN )
   return -1;

  strncpy(name, &(mes->data[5]), namelen);
  name[namelen] = 0;

  if ( vallen > 255 )
   return -1;

  strncpy(val, &(mes->data[5+namelen]), vallen);
  val[vallen] = 0;

  if ( mode == ROAR_META_MODE_SET ) {
   if ( stream_meta_set(mes->stream, type, name, val) == -1 )
    return -1;
  } else {
   if ( stream_meta_add(mes->stream, type, name, val) == -1 )
    return -1;
  }

  mes->datalen = 0;
  mes->cmd     = ROAR_CMD_OK;
  return 0;
 } else { // unknown mode!
  return -1;
 }

 return -1;
}

int req_on_get_meta    (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int vallen;
 int type;
 char val[LIBROAR_BUFFER_MSGDATA-1];

 if ( mes->datalen != 2 )
  return -1;

 if ( mes->data[0] != 0 ) // version
  return -1;

 type = (unsigned) mes->data[1];

 if ( stream_meta_get(mes->stream, type, NULL, val, LIBROAR_BUFFER_MSGDATA-2) == -1 )
  return -1;

 vallen = strlen(val);

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 2 + vallen;

 mes->data[0] = 0;
 mes->data[1] = (unsigned char) vallen;

 val[vallen] = 0;

 strncpy(&(mes->data[2]), val, vallen+1);

 return 0;
}

int req_on_list_meta   (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int i;
 int len = 0;
 int types[ROAR_META_MAX_PER_STREAM];

 if ( mes->datalen != 1 )
  return -1;

 if ( mes->data[0] != 0 ) // version
  return -1;

 if ( (len = stream_meta_list(mes->stream, types, ROAR_META_MAX_PER_STREAM)) == -1 )
  return -1;

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 1 + len;
 mes->data[0] = 0;

 for (i = 0; i < len; i++)
  mes->data[i+1] = types[i];

 return 0;
}
#endif

int req_on_server_oinfo    (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_stream s;
//ROAR_DIR_OUTPUT

 memset(&s, 0, sizeof(struct roar_stream));

 s.dir           = ROAR_DIR_MIXING;
 s.pos_rel_id    = -1;
 s.info.rate     = g_sa->rate;
 s.info.bits     = g_sa->bits;
 s.info.channels = g_sa->channels;
 s.info.codec    = g_sa->codec;
 s.pos           = g_pos;

 if ( roar_stream_s2m(&s, mes) == -1 )
  return -1;

 mes->cmd = ROAR_CMD_OK;

 return 0;
}


int req_on_get_standby (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 mes->cmd     = ROAR_CMD_OK;
 mes->pos     = g_pos;
 mes->datalen = 2;

 *((uint16_t*)mes->data) = ROAR_HOST2NET16((unsigned) g_standby);

 return 0;
}

int req_on_set_standby (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 if ( mes->datalen != 2 )
  return -1;

 g_standby = ROAR_NET2HOST16(*((uint16_t*)mes->data));

 mes->cmd     = ROAR_CMD_OK;
 mes->pos     = g_pos;
 mes->datalen = 0;

 return 0;
}

int req_on_exit      (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int term = 0;

 if ( mes->datalen == 1 )
  term = mes->data[0];

 mes->cmd     = ROAR_CMD_OK;
 mes->pos     = g_pos;
 mes->datalen = 0;

 ROAR_DBG("req_on_exit(*): term=%i", term);

 if ( term ) {
  cleanup_listen_socket(1);
 } else {
  alive = 0;
 }

 return 0;
}

int req_on_list_clients(int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 unsigned char filter, cmp;
 uint32_t id;
 int clients[ROAR_CLIENTS_MAX];
 int i, c = 0;

 if ( roar_ctl_m2f(mes, &filter, &cmp, &id) == -1 )
  return -1;

 // TODO: add code to support filter
 if ( filter != ROAR_CTL_FILTER_ANY )
  return -1;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++) {
  if ( g_clients[i] != NULL ) {
   clients[c++] = i;
  }
 }

 roar_ctl_ia2m(mes, clients, c);

 mes->cmd = ROAR_CMD_OK;

 return 0;
}
int req_on_list_streams(int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 unsigned char filter, cmp;
 uint32_t id;
 int streams[ROAR_STREAMS_MAX];
 int i, c = 0;

 if ( roar_ctl_m2f(mes, &filter, &cmp, &id) == -1 )
  return -1;

 // TODO: add code to support filter
 if ( filter != ROAR_CTL_FILTER_ANY )
  return -1;

 for (i = 0; i < ROAR_STREAMS_MAX; i++) {
  if ( g_streams[i] != NULL ) {
   streams[c++] = i;
  }
 }

 roar_ctl_ia2m(mes, streams, c);

 mes->cmd = ROAR_CMD_OK;

 return 0;
}

int req_on_get_client  (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_client * c;

 if ( mes->datalen != 1 )
  return -1;

 if ( clients_get(mes->data[0], &c) == -1 )
  return -1;

 mes->cmd = ROAR_CMD_OK;

 return roar_ctl_c2m(mes, c);
}

int req_on_get_stream  (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_stream_server * s;

 if ( mes->datalen != 1 )
  return -1;

 if ( streams_get(mes->data[0], &s) == -1 )
  return -1;

 mes->cmd = ROAR_CMD_OK;
 mes->stream = mes->data[0];

 return roar_stream_s2m(ROAR_STREAM(s), mes);
}

int req_on_get_stream_para (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_stream * s;
 struct roar_stream_server * ss;
 struct roar_audio_info * audio_info;
 uint16_t * d = (uint16_t *) mes->data;
 int i;
 char * str;

 if ( mes->datalen != 4 )
  return -1;

 for (i = 0; i < mes->datalen/2; i++) {
  d[i] = ROAR_NET2HOST16(d[i]);
 }

 if ( d[0] != 0 ) {
  ROAR_WARN("req_on_get_stream_para(*): unsupported command version: %i, %i", (int)d[0], (int)d[1]);
  return -1;
 }

 switch (d[1]) {
  case ROAR_STREAM_PARA_INFO:
    if ( streams_get(mes->stream, &ss) == -1 ) {
     ROAR_WARN("req_on_get_stream_para(*): request on non existing (or other error?) stream %i", mes->stream);
     return -1;
    }

    if ( streams_calc_delay(mes->stream) == -1 ) {
     ROAR_WARN("req_on_get_stream_para(*): can not calc delay for stream %i", mes->stream);
    }

    s = ROAR_STREAM(ss);

    audio_info = &(s->info);

    mes->datalen = 2*12;

    d[ 2] = ROAR_OUTPUT_CALC_OUTBUFSIZE(audio_info);
    d[ 3] = ss->pre_underruns;
    d[ 4] = ss->post_underruns;
    d[ 5] = ss->codec_orgi;
    d[ 6] = (ss->flags & 0xFFFF) | (ss->primary ? ROAR_FLAG_PRIMARY : 0) | (ss->driver_id != -1 ? ROAR_FLAG_OUTPUT : 0);
    d[ 7] = ss->delay/1000;
    d[ 8] = ss->state;
    d[ 9] = (ss->flags & 0xFFFF0000) >> 16;
    d[10] = ss->mixer_stream;
    d[11] = ss->role;

    ROAR_DBG("req_on_get_stream_para(*): ss->driver_id=%i", ss->driver_id);

    ROAR_DBG("req_on_get_stream_para(*): delay=%i, send delay=%i", ss->delay, d[7]);

    for (i = 0; i < mes->datalen/2; i++) {
     d[i] = ROAR_HOST2NET16(d[i]);
    }

    mes->pos = s->pos;
   break;

  case ROAR_STREAM_PARA_NAME:
   str = streams_get_name(mes->stream);

   if ( str == NULL )
    return -1;

    mes->datalen = 4 + strlen(str);

    if ( mes->datalen > LIBROAR_BUFFER_MSGDATA )
     return -1;

    strncpy(((char*)&(mes->data))+4, str, mes->datalen);

    d[0] = ROAR_HOST2NET16(d[0]);
    d[1] = ROAR_HOST2NET16(d[1]);
   break;

  case ROAR_STREAM_PARA_CHANMAP:
    if ( streams_get(mes->stream, &ss) == -1 ) {
     ROAR_WARN("req_on_get_stream_para(*): request on non existing (or other error?) stream %i", mes->stream);
     return -1;
    }

    s = ROAR_STREAM(ss);

    memcpy(&(mes->data[4]), ss->chanmap.in, s->info.channels);
    mes->datalen = 2*2 + s->info.channels;

    d[0] = ROAR_HOST2NET16(d[0]);
    d[1] = ROAR_HOST2NET16(d[1]);
   break;

  default:
    ROAR_WARN("req_on_get_stream_para(*): unsupported command: %i", d[1]);
    return -1;
 }

 mes->cmd = ROAR_CMD_OK;
 return 0;
}

int req_on_set_stream_para (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 uint16_t * d = (uint16_t *) mes->data;
 int i;

 if ( mes->datalen < 2*2 )
  return -1;

 for (i = 0; i < 2; i++) {
  d[i] = ROAR_NET2HOST16(d[i]);
 }

 if ( d[0] != 0 )
  return -1;

 switch (d[1]) {
  case ROAR_STREAM_PARA_FLAGS:
    if ( mes->datalen != 2*4 )
     return -1;

    d[2] = ROAR_NET2HOST16(d[2]);
    d[3] = ROAR_NET2HOST16(d[3]);

    ROAR_DBG("req_on_set_stream_para(*): request seems to be valid");

    if ( d[2] == ROAR_RESET_FLAG ) {
     if ( streams_reset_flag(mes->stream, d[3]) == -1 )
      return -1;
    } else {
     if ( streams_set_flag(mes->stream, d[3]) == -1 )
      return -1;
    }
   break;
  case ROAR_STREAM_PARA_CHANMAP:
    if ( streams_set_map(mes->stream, &(mes->data[4]), mes->datalen - 4) == -1 )
     return -1;
   break;
  case ROAR_STREAM_PARA_ROLE:
    if ( mes->datalen != 2*3 )
     return -1;

    d[2] = ROAR_NET2HOST16(d[2]);

    if ( streams_set_role(mes->stream, d[2]) == -1 )
     return -1;
   break;
  default:
    ROAR_WARN("req_on_set_stream_para(*): unsupported command version: %i, %i", d[0], d[1]);
    return -1;
   break;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_kick (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 uint16_t * info = (uint16_t *) mes->data;

 if ( mes->datalen != 4 )
  return -1;

 info[0] = ROAR_NET2HOST16(info[0]);
 info[1] = ROAR_NET2HOST16(info[1]);

 if ( info[0] == ROAR_OT_CLIENT ) {
  clients_delete(info[1]);
 } else if ( info[0] == ROAR_OT_STREAM ) {
  if ( streams_get_flag(info[1], ROAR_FLAG_IMMUTABLE) == 1 )
   return -1;

  streams_delete(info[1]);
 } else if ( info[0] == ROAR_OT_SOURCE ) {
  if ( streams_get_flag(info[1], ROAR_FLAG_IMMUTABLE) == 1 )
   return -1;

  if ( streams_get_flag(info[1], ROAR_FLAG_SOURCE) == 1 ) {
   streams_delete(info[1]);
  } else {
   return -1;
  }
 } else {
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_attach      (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 uint16_t * info = (uint16_t *) mes->data;

 if ( mes->datalen < 6 )
  return -1;

 info[0] = ROAR_NET2HOST16(info[0]);
 info[1] = ROAR_NET2HOST16(info[1]);
 info[2] = ROAR_NET2HOST16(info[2]);

 if ( info[0] != 0 )
  return -1;

 if ( info[1] == ROAR_ATTACH_SIMPLE ) {
  if ( client_stream_move(info[2], mes->stream) == -1 )
   return -1;
 } else {
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_set_vol (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_stream_server * s;
 uint16_t * info = (uint16_t *) mes->data;
 uint16_t   version;
 uint16_t   scale = 65535;
 int stream;
 int i;
 int chans;

 ROAR_DBG("req_on_set_vol(*) = ?");
 ROAR_DBG("req_on_set_vol(*): mes->datalen=%i", mes->datalen);

 if ( mes->datalen < (4*2) )
  return -1;

 version = ROAR_NET2HOST16(info[0]);
 ROAR_DBG("req_on_set_vol(*): version=%i", (int)version);

 switch (version) {
  case 0:
    stream = ROAR_NET2HOST16(info[1]);
   break;
  case 1:
    stream = mes->stream;
    scale  = ROAR_NET2HOST16(info[1]);
   break;
  default:
    return -1;
   break;
 }
 ROAR_DBG("req_on_set_vol(*): stream=%i", stream);

 if ( scale == 0 )
  return -1;

 // TODO: change this code.
 //       we should not directly change the stream object but use some stream_*()-func
 //       for that job.

 if ( stream < 0 || stream >= ROAR_STREAMS_MAX )
  return -1;

 s = g_streams[stream];

 if ( s == NULL )
  return -1;

 ROAR_DBG("req_on_set_vol(*): s=%p", s);

 info[2] = ROAR_NET2HOST16(info[2]);

 if ( info[2] == ROAR_SET_VOL_ALL ) {
  chans = (mes->datalen/2) - 3;
  ROAR_DBG("req_on_set_vol(*): mode is ROAR_SET_VOL_ALL, channes=%i", chans);

  if ( chans >= ROAR_MAX_CHANNELS )
   return -1;

  ROAR_DBG("req_on_set_vol(*): mixer at %p", s->mixer.mixer);

  for (i = 0; i < chans; i++) {
   s->mixer.mixer[i] = ROAR_NET2HOST16(info[i+3]);
   ROAR_DBG("req_on_set_vol(*): channel %i: %i", i, ROAR_NET2HOST16(info[i+3]));
  }

  s->mixer.scale = scale;

  ROAR_DBG("req_on_set_vol(*): mixer changed!");

 } else if ( info[2] == ROAR_SET_VOL_ONE ) {
  ROAR_DBG("req_on_set_vol(*): mode is ROAR_SET_VOL_ONE");
  if ( ROAR_NET2HOST16(info[3]) >= ROAR_MAX_CHANNELS )
   return -1;

  s->mixer.mixer[ROAR_NET2HOST16(info[3])] = ROAR_NET2HOST16(info[4]);

  s->mixer.scale = scale;
 } else {
  return -1;
 }

 if ( streams_set_mixer(stream) == -1 )
  return -1;

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_get_vol (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 uint16_t * info = (uint16_t *) mes->data;
 uint16_t   version = -1;
 int stream;
 struct roar_stream_server * s;
 int i;
 int chans;

 ROAR_DBG("req_on_get_vol(*) = ?");
 ROAR_DBG("req_on_get_vol(*): mes->datalen=%i", mes->datalen);

 if ( mes->datalen < 2 ) {
  return -1;
 }

 version = ROAR_NET2HOST16(info[0]);

 switch (version) {
  case 0:
    if ( mes->datalen < (2*2) )
     return -1;

    stream = ROAR_NET2HOST16(info[1]);
   break;
  case 1:
    stream = mes->stream;
   break;
  default:
    return -1;
   break;
 }

 ROAR_DBG("req_on_get_vol(*): stream=%i", stream);

 // TODO: change this code.
 //       we should not directly change the stream object but use some stream_*()-func
 //       for that job.

 if ( stream < 0 || stream >= ROAR_STREAMS_MAX )
  return -1;

 s = g_streams[stream];

 if ( s == NULL )
  return -1;

 ROAR_DBG("req_on_get_vol(*): s=%p", s);

 // ok, we have everything

 info[0] = ROAR_HOST2NET16(version);

 switch (version) {
  case 0:
    info[1] = ROAR_HOST2NET16(chans = ROAR_STREAM(s)->info.channels);

    for (i = 0; i < chans; i++)
     info[2+i] = ROAR_HOST2NET16(s->mixer.mixer[i]);

     mes->datalen = (2 + chans)*2;
   break;
  case 1:
    info[1] = ROAR_HOST2NET16(chans = ROAR_STREAM(s)->info.channels);
    info[2] = ROAR_HOST2NET16(s->mixer.scale);
    info[3] = ROAR_HOST2NET16(s->mixer.rpg_mul);
    info[4] = ROAR_HOST2NET16(s->mixer.rpg_div);

    for (i = 0; i < chans; i++)
     info[5+i] = ROAR_HOST2NET16(s->mixer.mixer[i]);

     mes->datalen = (5 + chans)*2;
   break;
  default:
    return -1;
   break;
 }

 mes->cmd = ROAR_CMD_OK;

 return 0;
}

int req_on_add_data (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_buffer * b;
 char               * buf;

 if ( roar_buffer_new_data(&b, mes->datalen, (void **)&buf) == -1 ) {
  ROAR_ERR("req_on_add_data(*): Can not alloc buffer space!");
  ROAR_DBG("req_on_add_data(*) = -1");
  return -1;
 }

 if ( data == NULL ) {
  memcpy(buf, mes->data, mes->datalen);
 } else {
  memcpy(buf, *data, mes->datalen);
 }

 if ( stream_add_buffer(mes->stream, b) == -1 ) {
  roar_buffer_free(b);
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK_STOP;
 mes->datalen = 0;

 return 0;
}

int req_on_beep        (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 struct roar_beep bs;
 int16_t * info = (int16_t*)mes->data;
 int stream;

 memset(&bs, 0, sizeof(bs));

 if ( mes->datalen > 0 ) {
  if ( mes->datalen < 2 )
   return -1;

  if ( ROAR_NET2HOST16(info[0]) != 0 ) /* version */
   return -1;

  if ( mes->datalen != 8*2 )
   return -1;

  bs.vol  = ROAR_NET2HOST16(info[1]);
  bs.time = ROAR_NET2HOST16(info[2]);
  bs.freq = ROAR_NET2HOST16(info[3]);
  bs.type = ROAR_NET2HOST16(info[4]);
  bs.x    = ROAR_NET2HOST16(info[5]);
  bs.y    = ROAR_NET2HOST16(info[6]);
  bs.z    = ROAR_NET2HOST16(info[7]);
 }

 if ( (stream = beep_start(client, &bs)) == -1 )
  return -1;

 mes->stream  = stream;
 mes->cmd     = ROAR_CMD_OK_STOP;
 mes->datalen = 0;

 return 0;
}

//ll
