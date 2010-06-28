//emul_rplay.c:

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

#ifndef ROAR_WITHOUT_DCOMP_EMUL_RPLAY

struct emul_rplay_command emul_rplay_commands[] = {
 {"access",      NULL, -1, -1, NULL},
 {"application", NULL,  1, -1, NULL},
 {"continue",    NULL,  1, -1, emul_rplay_on_continue},
 {"die",         NULL,  1, -1, NULL},
 {"done",        NULL,  1, -1, NULL}, // #ifdef DEBUG
 {"find",        NULL,  1,  1, NULL},
 {"get",         NULL,  1,  1, NULL},
 {"help",        NULL, -1, -1, emul_rplay_on_help},
 {"info",        NULL,  1,  1, NULL},
 {"list",        NULL,  0,  1, NULL},
 {"modify",      NULL,  2, -1, NULL},
 {"monitor",     NULL,  1, -1, NULL},
 {"pause",       NULL,  1, -1, emul_rplay_on_pause},
 {"play",        NULL,  1, -1, emul_rplay_on_play},
 {"put",         NULL,  2, -1, emul_rplay_on_put},
 {"quit",        NULL,  0,  0, emul_rplay_on_quit},
 {"reset",       NULL,  0,  0, NULL},
 {"set",         NULL,  1, -1, NULL},
 {"skip",        NULL,  1,  1, NULL},
 {"status",      NULL,  0,  0, emul_rplay_on_status},
 {"stop",        NULL,  1, -1, NULL},
 {"version",     NULL,  0,  0, NULL},
 {"volume",      NULL,  0,  1, NULL},
 {"wait",        NULL, -1, -1, NULL},
 {NULL, NULL, -1, -1, NULL}
};

static inline int is_true(const char * str) {
 const char * ts[] = {"true", "t", "1", "yes", "y", "on"};
 int i;

 for (i = 0; i < sizeof(ts)/sizeof(*ts); i++)
  if ( !strcasecmp(str, ts[i]) )
   return 1;

 return 0;
}

static inline int is_false(const char * str) {
 return !is_true(str);
}

static int format_to_codec(const char * str, const int bo) {

 if ( !strcasecmp(str, "ulaw") || !strcasecmp(str, "u_law") || !strcasecmp(str, "u-law") )
  return ROAR_CODEC_MULAW;

 if ( !strncasecmp(str, "ulinear", 7) ) {
  switch (bo) {
   case ROAR_BYTEORDER_LE:
     return ROAR_CODEC_PCM_U_LE;
    break;
   case ROAR_BYTEORDER_BE:
     return ROAR_CODEC_PCM_U_BE;
    break;
   case ROAR_BYTEORDER_PDP:
     return ROAR_CODEC_PCM_U_PDP;
    break;
   default:
     return -1;
    break;
  }
 } else if ( !strncasecmp(str, "linear", 6) ) {
  switch (bo) {
   case ROAR_BYTEORDER_LE:
     return ROAR_CODEC_PCM_S_LE;
    break;
   case ROAR_BYTEORDER_BE:
     return ROAR_CODEC_PCM_S_BE;
    break;
   case ROAR_BYTEORDER_PDP:
     return ROAR_CODEC_PCM_S_PDP;
    break;
   default:
     return -1;
    break;
  }
 }

 return -1;
}

int emul_rplay_check_client  (int client, struct roar_vio_calls * vio) {
 struct roar_vio_calls calls;
 char buf[1024];
 ssize_t len;

 if ( client == -1 )
  return -1;

 if ( vio == NULL ) {
  vio = &calls;
  if ( roar_vio_open_fh_socket(vio, clients_get_fh(client)) == -1 )
   return -1;
 }

 if ( (len = roar_vio_read(vio, buf, sizeof(buf)-1)) <= 0 ) {
  // really bad protocol error
  clients_delete(client);
  return -1;
 }

 for (; buf[len-1] == '\r' || buf[len-1] == '\n'; len--);

 buf[len] = 0;

 return emul_rplay_exec_command(client, vio, buf);
}

int emul_rplay_exec_command  (int client, struct roar_vio_calls * vio, char * command) {
 struct emul_rplay_command * cmd;
 struct roar_keyval * kv;
 ssize_t kvlen;
 char * para = NULL;
 char * c;
 int last_was_space = 0;

 for (c = command; *c != 0; c++) {
  if ( *c == ' ' || *c == '\t' ) {
   last_was_space = 1;
   *c = 0;
  } else {
   if ( last_was_space ) {
    para = c;
    break;
   }
  }
 }

 if ( para == NULL ) {
  kv = NULL; 
  kvlen = 0;
 } else {
  kvlen = roar_keyval_split(&kv, para, " \t", "=", 0);
  if ( kvlen == -1 )
   return emul_rplay_send_error(client, NULL, vio, NULL, 0, "Can not parse parameter list");
 }

 for (cmd = emul_rplay_commands; cmd->name != NULL; cmd++) {
  if ( !strcasecmp(cmd->name, command) )
   break;
 }

 if ( cmd->name == NULL )
  return emul_rplay_send_error(client, NULL, vio, kv, kvlen, "unknown command");

 if ( cmd->handler == NULL )
  return emul_rplay_send_error(client, cmd, vio, kv, kvlen, "unsupported command");

 return cmd->handler(client, cmd, vio, kv, kvlen);
}

int emul_rplay_send_error    (int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen, const char * msg) {
 struct roar_keyval * kvr;
 const char * command;
 const char * cd = NULL;

 if ( cmd != NULL ) {
  command = cmd->name;
 } else {
  command = "(unknown)";
 }

 if ( kv != NULL ) {
  kvr = roar_keyval_lookup(kv, "client-data", kvlen, 0);
  if ( kvr != NULL )
   cd = kvr->value;
 }

 if ( cd == NULL )
  cd = "";

 return roar_vio_printf(vio, "-error=\"%s\" command=\"%s\" client-data=\"%s\"\n", msg, command, cd) <= 0 ? -1 : 0;
}


int emul_rplay_on_status(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 const char * hostname  = "localhost";
 const char * version   = "RoarAudio";
       char   uptime[16];
 const char * byteorder = "native";
       int    fragsize  = ROAR_OUTPUT_CALC_OUTBUFSIZE(g_sa);
       int    h, m, s;

 s  = g_pos / g_sa->rate / g_sa->channels;
 h  = s / 3600;
 s -= h * 3600;
 m  = s / 60;
 s -= m * 60;

 sprintf(uptime, "%.2i:%.2i:%.2i", h, m, s);

 switch (ROAR_CODEC_BYTE_ORDER(g_sa->codec)) {
  case ROAR_CODEC_LE:
    byteorder = "little-endian";
   break;
  case ROAR_CODEC_BE:
    byteorder = "big-endian";
   break;
  case ROAR_CODEC_PDP:
    byteorder = "pdp-endian";
   break;
 }

 roar_vio_printf(vio,
                 "+host=%s version=%s uptime=%s "
                 "audio-bits=%i audio-byte-order=%s audio-channels=%i "
                 "audio-device=internal-mixer "
                 "audio-format=linear-%i "
                 "audio-fragsize=%i "
                 "audio-port=speaker,headphone,lineout audio-rate=10 "
                 "audio-sample-rate=%i "
                 "volume=254 "
                 "curr-rate=10 priority-threshold=0 audio-close=0 audio-device-status=open"
                 "\n",
                      hostname, version, uptime,
                      g_sa->bits, byteorder, g_sa->channels,
                      g_sa->bits,
                      fragsize,
                      g_sa->rate
                );

 return 0;
}


int emul_rplay_on_quit(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 clients_delete(client);
 return -1;
}

int emul_rplay_on_help(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 struct emul_rplay_command * c;

 roar_vio_printf(vio, "+message=\"command summary\" command=help\n");

 for (c = emul_rplay_commands; c->name != NULL; c++) {
  roar_vio_printf(vio, "%-8s %s\n", c->name, c->usage == NULL ? "" : c->usage);
 }

 roar_vio_printf(vio, ".\n");

 return -1;
}


int emul_rplay_on_play(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 struct roar_keyval        * kvr, * rate, * bits, * channels, * format, * byteorder;
 struct roar_audio_info      info;
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 int stream;
 int bo = -1;
 char * cd = NULL;

 if ( (kvr = roar_keyval_lookup(kv, "input", kvlen, 0)) == NULL ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "no input parameter");
  return -1;
 }

 if ( !!strcasecmp(kvr->value, "flow") ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "non-flow input not supported");
  return -1;
 }

/*
               "input-format=%s input-sample-rate=%d input-bits=%d "
               "input-channels=%d input-byte-order=%s",
*/

 rate      = roar_keyval_lookup(kv, "input-sample-rate", kvlen, 0);
 bits      = roar_keyval_lookup(kv, "input-bits",        kvlen, 0);
 channels  = roar_keyval_lookup(kv, "input-channels",    kvlen, 0);
 format    = roar_keyval_lookup(kv, "input-format",      kvlen, 0);
 byteorder = roar_keyval_lookup(kv, "input-byte-order",  kvlen, 0);

 if ( rate == NULL || bits == NULL || channels == NULL || format == NULL || byteorder == NULL ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "missing audio parameter");
  return -1;
 }

 info.rate     = atoi(rate->value);
 info.bits     = atoi(bits->value);
 info.channels = atoi(channels->value);

 if ( !strcasecmp(byteorder->value, "big-endian") || !strcasecmp(byteorder->value, "big") ) {
  bo = ROAR_BYTEORDER_BE;
 } else if ( !strcasecmp(byteorder->value, "little-endian") || !strcasecmp(byteorder->value, "little") ) {
  bo = ROAR_BYTEORDER_LE;
 } else if ( !strcasecmp(byteorder->value, "pdp-endian") || !strcasecmp(byteorder->value, "pdp") ) {
  bo = ROAR_BYTEORDER_PDP;
 } else {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "unknown byte order");
  return -1;
 }

 info.codec = format_to_codec(format->value, bo);

 if ((stream = streams_new()) == -1 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not create new stream");
  return -1;
 }

 if ( streams_get(stream, &ss) == -1 ) {
  streams_delete(stream);
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not get stream object");
  return -1;
 }

 s = ROAR_STREAM(ss);

 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not add stream to client");
  return -1;
 }

 memcpy(&(s->info), &info, sizeof(info));
 ss->codec_orgi = s->info.codec;

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not set dir on stream");
  return -1;
 }

 if ( (kvr = roar_keyval_lookup(kv, "client-data", kvlen, 0)) != NULL ) {
  cd = kvr->value;
 }

 if ( cd == NULL )
  cd = "";

//        connection_reply(c, "%cid=#%d sound=\"%s\" command=%s client-data=\"%s\" list-name=\"%s\"",
// roar_vio_printf(vio, "+id=#%i sound=\"%s\" command=%s client-data=\"%s\" list-name=\"%s\"");
 roar_vio_printf(vio, "+id=#%i command=%s client-data=\"%s\"\n", stream, "play", cd);

 return 0;
}

int emul_rplay_on_put(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 struct roar_keyval * kvr;
 int stream;
 size_t len;
 char * cd = NULL;

//23:00 < ph3-der-loewe>   rptp_putline(flow_fd, "put id=#%d size=0", spool_id);
 if ( (kvr = roar_keyval_lookup(kv, "id", kvlen, 0)) == NULL ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "no id parameter");
  return -1;
 }

 stream = atoi(kvr->value+1);

 if ( (kvr = roar_keyval_lookup(kv, "size", kvlen, 0)) == NULL ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "no size parameter");
  return -1;
 }

 len = atoi(kvr->value);

 if ( len != 0 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "currently only zero size put supported");
  return -1;
 }

 if ( client_stream_exec(client, stream) == -1 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not exec stream");
  return -1;
 }


 if ( (kvr = roar_keyval_lookup(kv, "client-data", kvlen, 0)) != NULL ) {
  cd = kvr->value;
 }

 if ( cd == NULL )
  cd = "";

/*
        connection_reply(c, "%cid=#%d size=%d command=put client-data=\"%s\"",
                         RPTP_OK, spool_id, sound_size, client_data);
*/
 roar_vio_printf(vio, "+id=#%i command=%s client-data=\"%s\"\n", stream, "put", cd);

 return 0;
}

int emul_rplay_on_set(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen);
int emul_rplay_on_modify(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen);

int emul_rplay_on_pause(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 struct roar_keyval * kvr;
 int stream;
 char * cd = NULL;

 if ( kvlen < 1 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "no id parameter");
  return -1;
 }

 stream = atoi(kv->key+1);

 if ( streams_set_flag(stream, ROAR_FLAG_PAUSE) == -1 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not set pause flag");
  return -1;
 }

 if ( (kvr = roar_keyval_lookup(kv, "client-data", kvlen, 0)) != NULL ) {
  cd = kvr->value;
 }

 if ( cd == NULL )
  cd = "";

 roar_vio_printf(vio, "+id=#%i command=%s client-data=\"%s\"\n", stream, "pause", cd);

 return 0;
}

int emul_rplay_on_continue(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen) {
 struct roar_keyval * kvr;
 int stream;
 char * cd = NULL;

 if ( kvlen < 1 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "no id parameter");
  return -1;
 }

 stream = atoi(kv->key+1);

 if ( streams_reset_flag(stream, ROAR_FLAG_PAUSE) == -1 ) {
  emul_rplay_send_error(client, cmd, vio, kv, kvlen, "can not reset pause flag");
  return -1;
 }

 if ( (kvr = roar_keyval_lookup(kv, "client-data", kvlen, 0)) != NULL ) {
  cd = kvr->value;
 }

 if ( cd == NULL )
  cd = "";

 roar_vio_printf(vio, "+id=#%i command=%s client-data=\"%s\"\n", stream, "coninue", cd);

 return 0;
}

int emul_rplay_on_stop(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen);

#endif

//ll
