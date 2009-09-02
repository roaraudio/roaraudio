//emul_esd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

#ifndef ROAR_WITHOUT_DCOMP_EMUL_ESD
#ifdef ROAR_HAVE_ESD

#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_MINIMAL)
#define _NAME(x) (x)
#else
#define _NAME(x) ((char*)NULL)
#endif

#define _cmd_t   int
#define _INTSIZE sizeof(_cmd_t)

#define _NOT_TO_IMPLEMENT    NULL
#define _UNIMPLEMNTED_IN_ESD NULL
#define _NEED_SAMPLE_SUPPORT NULL

#define _ROAR2ESD(x) ((x)+1)
#define _ESD2ROAR(x) ((x)-1)

struct emul_esd_command g_emul_esd_commands[] = {
 {ESD_PROTO_CONNECT,      ESD_KEY_LEN  +     _INTSIZE, _NAME("CONNECT"),      emul_esd_on_connect},
 {ESD_PROTO_LOCK,         ESD_KEY_LEN  +     _INTSIZE, _NAME("LOCK"),         NULL},
 {ESD_PROTO_UNLOCK,       ESD_KEY_LEN  +     _INTSIZE, _NAME("UNLOCK"),       NULL},
 {ESD_PROTO_STREAM_PLAY,  ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_PLAY"),  emul_esd_on_stream},
 {ESD_PROTO_STREAM_REC,   ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_REC"),   emul_esd_on_stream},
 {ESD_PROTO_STREAM_MON,   ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_MON"),   emul_esd_on_stream},
 {ESD_PROTO_SAMPLE_CACHE, ESD_NAME_MAX + 3 * _INTSIZE, _NAME("SAMPLE_CACHE"), _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_SAMPLE_FREE,                     _INTSIZE, _NAME("SAMPLE_FREE"),  _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_SAMPLE_PLAY,                     _INTSIZE, _NAME("SAMPLE_PLAY"),  _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_SAMPLE_LOOP,                     _INTSIZE, _NAME("SAMPLE_LOOP"),  _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_SAMPLE_STOP,                     _INTSIZE, _NAME("SAMPLE_STOP"),  _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_SAMPLE_KILL,  0                          , _NAME("SAMPLE_KILL"),  _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_STANDBY,      ESD_KEY_LEN +      _INTSIZE, _NAME("STANDBY"),      emul_esd_on_standby},
 {ESD_PROTO_RESUME,       ESD_KEY_LEN +      _INTSIZE, _NAME("RESUME"),       emul_esd_on_standby},
 {ESD_PROTO_SAMPLE_GETID, ESD_NAME_MAX               , _NAME("SAMPLE_GETID"), _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_STREAM_FILT,  ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_FILT"),  emul_esd_on_stream},
 {ESD_PROTO_SERVER_INFO,                     _INTSIZE, _NAME("SERVER_INFO"),  emul_esd_on_server_info},
 {ESD_PROTO_ALL_INFO,                        _INTSIZE, _NAME("ALL_INFO"),     emul_esd_on_all_info},
 {ESD_PROTO_SUBSCRIBE,    0                          , _NAME("SUBSCRIBE"),    _UNIMPLEMNTED_IN_ESD},
 {ESD_PROTO_UNSUBSCRIBE,  0                          , _NAME("UNSUBSCRIBE"),  _UNIMPLEMNTED_IN_ESD},
 {ESD_PROTO_STREAM_PAN,                  3 * _INTSIZE, _NAME("STREAM_PAN"),   emul_esd_on_stream_pan},
 {ESD_PROTO_SAMPLE_PAN,                  3 * _INTSIZE, _NAME("SAMPLE_PAN"),   _NEED_SAMPLE_SUPPORT},
 {ESD_PROTO_STANDBY_MODE,                    _INTSIZE, _NAME("STANDBY_MODE"), emul_esd_on_standbymode},
 {ESD_PROTO_LATENCY,      0                          , _NAME("LATENCY"),      emul_esd_on_latency},
 {ESD_PROTO_MAX,          0                          , _NAME("MAX"),          _NOT_TO_IMPLEMENT},
 {-1, 0, _NAME("END OF LIST"), _NOT_TO_IMPLEMENT}
};

// command handling:
int emul_esd_exec_command(int client, int cmd, struct roar_vio_calls * vio) {
 struct emul_esd_command * cur;
 void * data = NULL;
 ssize_t ret;
 size_t  done = 0;
 int r;
 int i;

 ROAR_DBG("emul_esd_exec_command(*) = ?");

 if ( client == -1 || cmd < ESD_PROTO_CONNECT || cmd > ESD_PROTO_MAX || vio == NULL )
  return -1;

 ROAR_DBG("emul_esd_exec_command(*) = ?");

 for (i = 0; (cur = &(g_emul_esd_commands[i]))->cmd != -1; i++) {
  if ( cur->cmd == cmd ) {
   if ( cur->datalen > 0 ) {
    if ( (data = malloc(cur->datalen)) == NULL ) {
     // we will do a protocol error in case we do not drop the client
     clients_delete(client);
     return -1;
    }

    while ( done < cur->datalen ) {
     ret = roar_vio_read(vio, data+done, cur->datalen-done);

     if ( ret < 1 ) {
      free(data);
      clients_delete(client);
      return -1;
     } else {
      done += ret;
     }
    }
   }

   if ( cur->handler == NULL ) {
    ROAR_WARN("emul_esd_exec_command(client=%i, cmd=%s(%i), vio=%p): client uses unimplemted command",
               client, cur->name, cmd, vio
             );
    clients_delete(client);
    r = -1;
   } else {
    r = cur->handler(client, cur, data, vio);
   }

   if ( data != NULL )
    free(data);

   return r;
  }
 }

 return -1;
}

int emul_esd_check_client(int client, struct roar_vio_calls * vio) {
 struct roar_vio_calls calls;
 _cmd_t cmd;

 if ( client == -1 )
  return -1;

 if ( vio == NULL ) {
  vio = &calls;
  if ( roar_vio_open_fh(vio, clients_get_fh(client)) == -1 )
   return -1;
 }

 if ( roar_vio_read(vio, &cmd, _INTSIZE) != _INTSIZE ) {
  // really bad protocol error
  clients_delete(client);
  return -1;
 }

 return emul_esd_exec_command(client, cmd, vio);
}

// porto lib:
int emul_esd_int_read_buf  (int client, int * data, void * buf) {
 _cmd_t d;

 if ( data == NULL || buf == NULL )
  return -1;

 d = *(_cmd_t*)buf;

 *data = d;

 return 0;
}
int emul_esd_int_read      (int client, int * data, struct roar_vio_calls * vio) {
 _cmd_t d;

 if ( data == NULL )
  return -1;

 if ( roar_vio_read(vio, &d, _INTSIZE) != _INTSIZE )
  return -1;

 *data = d;

 return 0;
}

int emul_esd_int_write     (int client, int   data, struct roar_vio_calls * vio) {
 _cmd_t d = data;

 return roar_vio_write(vio, &d, _INTSIZE) == _INTSIZE ? 0 : -1;
}

int emul_esd_test_auth     (int client, void * data, struct roar_vio_calls * vio) {
 // accept all clients for the moment.
 return emul_esd_int_write(client, 1, vio);
}

int emul_esd_test_byteorder(int client, void * data) {
 // TODO: do a real test
 return 0;
}

// handler:
int emul_esd_on_connect    (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {

 ROAR_DBG("emul_esd_on_connect(client=%i, cmd=%p, data=%p, vio=%p) = ?", client, cmd, data, vio);

 if ( client == -1 || data == NULL || vio == NULL )
  return -1;

 ROAR_DBG("emul_esd_on_connect(client=%i, cmd=%p, data=%p, vio=%p) = ?", client, cmd, data, vio);

 if ( emul_esd_test_auth(client, data, vio) == -1 )
  return -1;

 ROAR_DBG("emul_esd_on_connect(client=%i, cmd=%p, data=%p, vio=%p) = ?", client, cmd, data, vio);

 if ( emul_esd_test_byteorder(client, data+ESD_KEY_LEN) == -1 )
  return -1;

 ROAR_DBG("emul_esd_on_connect(client=%i, cmd=%p, data=%p, vio=%p) = ?", client, cmd, data, vio);

 return 0;
}

int emul_esd_on_stream     (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 struct roar_client        *  c;
 int stream;
 int dir = -1;
 int esdformat;
 int rate;

 if ( client == -1 || cmd == NULL || data == NULL || vio == NULL )
  return -1;

 switch (cmd->cmd) {
  case ESD_PROTO_STREAM_PLAY: dir = ROAR_DIR_PLAY;    break;
  case ESD_PROTO_STREAM_REC:  dir = ROAR_DIR_RECORD;  break;
  case ESD_PROTO_STREAM_MON:  dir = ROAR_DIR_MONITOR; break;
  case ESD_PROTO_STREAM_FILT: dir = ROAR_DIR_FILTER;  break;
  default:
    clients_delete(client);
    return -1;
 }

 if ( clients_get(client, &c) == -1 ) {
  return -1;
 }

 ROAR_DBG("emul_esd_on_stream(client=%i, ...): creating stream...", client);
 if ((stream = streams_new()) == -1 ) {
  clients_delete(client);
  return -1;
 }

 ROAR_DBG("emul_esd_on_stream(client=%i, ...): getting stream...", client);
 if ( streams_get(stream, &ss) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 s = ROAR_STREAM(ss);

 ROAR_DBG("emul_esd_on_stream(client=%i, ...): set client of stream...", client);
 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 emul_esd_int_read_buf(client, &esdformat, data);
 emul_esd_int_read_buf(client, &rate,      data+_INTSIZE);

 strncpy(c->name, data + 2*_INTSIZE, ROAR_BUFFER_NAME > ESD_NAME_MAX ? ESD_NAME_MAX : ROAR_BUFFER_NAME);
 c->name[ROAR_BUFFER_NAME-1] = 0;

 ROAR_DBG("emul_esd_on_stream(*): esdformat=0x%.8X, rate=%i", esdformat, rate);

 s->info.rate = rate;

 switch (esdformat & ESD_MASK_BITS) {
  case ESD_BITS8:  s->info.bits =  8; s->info.codec = ROAR_CODEC_PCM_U_LE; break;
  case ESD_BITS16: s->info.bits = 16; s->info.codec = ROAR_CODEC_DEFAULT;  break;
  default:
    streams_delete(stream);
    clients_delete(client);
    return -1;
 }

 switch (esdformat & ESD_MASK_CHAN) {
  case ESD_MONO:   s->info.channels = 1; break;
  case ESD_STEREO: s->info.channels = 2; break;
  default:
    streams_delete(stream);
    clients_delete(client);
    return -1;
 }

 ss->codec_orgi = s->info.codec;

 ROAR_DBG("emul_esd_on_stream(*): s->info = {.rate=%i, .bits=%i, .channels=%i, .codec=%i}", s->info.rate, s->info.bits, s->info.channels, s->info.codec);

 if ( streams_set_dir(stream, dir, 1) == -1 ) {
  clients_delete(client);
  return -1;
 }

 if ( client_stream_exec(client, stream) == -1 ) {
  clients_delete(client);
  return -1;
 }

 return 0;
}

int emul_esd_on_latency    (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 int lag = ROAR_OUTPUT_CFREQ;

 lag *= 2.0 * 44100.0 / (float)g_sa->rate;
 
 return emul_esd_int_write(client, lag, vio);
}

int emul_esd_on_standby    (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 int ok = 0;

 if ( emul_esd_test_auth(client, data, vio) == -1 ) {
  return emul_esd_int_write(client, ok, vio);
 }

 ok = 1;

 if (cmd->cmd == ESD_PROTO_STANDBY) {
  g_standby = 1;
 } else {
  g_standby = 0;
 }


 return emul_esd_int_write(client, ok, vio);
}

int emul_esd_on_standbymode(int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 int mode = ESM_ERROR;

 if ( g_standby ) {
  if ( g_autostandby ) {
   mode = ESM_ON_AUTOSTANDBY;
  } else {
   mode = ESM_ON_STANDBY;
  }
 } else {
  mode = ESM_RUNNING;
 }

 return emul_esd_int_write(client, mode, vio);
}

int emul_esd_on_stream_pan (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 struct roar_stream_server * ss;
 int stream;
 int left, right;
 int ok = 0;

 emul_esd_int_read_buf(client, &stream, data + 0*_INTSIZE);
 emul_esd_int_read_buf(client, &left,   data + 1*_INTSIZE);
 emul_esd_int_read_buf(client, &right,  data + 2*_INTSIZE);

 stream = _ESD2ROAR(stream);

 if ( streams_get(stream, &ss) != -1 ) {
  ss->mixer.mixer[0] = left  == 256 ? 65535 : left  * 256;
  ss->mixer.mixer[1] = right == 256 ? 65535 : right * 256;
  if ( streams_set_mixer(stream) != -1 )
   ok = 1;
 }

 return emul_esd_int_write(client, ok, vio);
}

int emul_esd_on_server_info(int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 int version = 0;
 int rate    = g_sa->rate;
 int format  = 0;

 switch (g_sa->bits) {
  case  8: format |= ESD_BITS8;  break;
  case 16: format |= ESD_BITS16; break;
 }

 switch (g_sa->channels) {
  case  1: format |= ESD_MONO;   break;
  case  2: format |= ESD_STEREO; break;
 }

 if ( emul_esd_int_write(client, version, vio) == -1 )
  return -1;

 if ( emul_esd_int_write(client, rate, vio) == -1 )
  return -1;

 if ( emul_esd_int_write(client, format, vio) == -1 )
  return -1;

 return 0;
}

int emul_esd_on_all_info   (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 struct roar_stream_server *   ss;
 struct roar_audio_info    * info;
 struct roar_client        *    c;
 char name[ESD_NAME_MAX];
 char * sname;
 int id, rate, left, right, format;
 int i;

 if ( emul_esd_on_server_info(client, cmd, data, vio) == -1 )
  return -1;

 for (i = 0; i < (ROAR_STREAMS_MAX+2); i++) {
  memset(name, 0, sizeof(name));

  id   = rate  = format = 0;
  left = right = 0;

  if ( i >= ROAR_STREAMS_MAX ) {
   id = -1;
  } else {
   if ( streams_get(i, &ss) == -1 )
    continue;

   if ( streams_get_dir(i) != ROAR_DIR_PLAY )
    continue;

   info = &(ROAR_STREAM(ss)->info);

   id = i;
   rate = info->rate;

   format |= ESD_PLAY;

   switch (info->bits) {
    case  8: format |= ESD_BITS8;  break;
    case 16: format |= ESD_BITS16; break;
   }

   switch (info->channels) {
    case  1:
      if ( ss->mixer.mixer[0] == ss->mixer.scale ) {
       left = right = 256;
      } else {
       left = right = ss->mixer.mixer[0] / 256;
      }
      format |= ESD_MONO;
     break;
    case  2:
      if ( ss->mixer.mixer[0] == ss->mixer.scale ) {
       left = 256;
      } else {
       left = ss->mixer.mixer[0] / 256;
      }

      if ( ss->mixer.mixer[1] == ss->mixer.scale ) {
       right = 256;
      } else {
       right = ss->mixer.mixer[1] / 256;
      }

      format |= ESD_STEREO;
     break;
    default:
      left = right = 0;
   }

   sname = streams_get_name(id);

   if ( sname == NULL || sname[0] == 0 ) {
    if ( clients_get(streams_get_client(id), &c) != -1 ) {
     sname = c->name;
    }
   }

   if ( sname == NULL || sname[0] == 0 )
    sname = "(unknown)";

   strncpy(name, sname, sizeof(name) > ROAR_BUFFER_NAME ? ROAR_BUFFER_NAME : sizeof(name));
   name[sizeof(name)-1] = 0;

  }

  id = _ROAR2ESD(id);

  if ( emul_esd_int_write(client, id, vio) == -1 )
   return -1;
  if ( roar_vio_write(vio, name, sizeof(name)) != sizeof(name) )
   return -1;
  if ( emul_esd_int_write(client, rate, vio) == -1 )
   return -1;
  if ( emul_esd_int_write(client, left, vio) == -1 )
   return -1;
  if ( emul_esd_int_write(client, right, vio) == -1 )
   return -1;
  if ( emul_esd_int_write(client, format, vio) == -1 )
   return -1;

  if ( i == ROAR_STREAMS_MAX+1) {
   // write 'length'...
   if ( emul_esd_int_write(client, id, vio) == -1 )
    return -1;
  }
 }

 return 0;
}

#endif
#endif

//ll
