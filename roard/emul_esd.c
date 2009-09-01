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

struct emul_esd_command g_emul_esd_commands[] = {
 {ESD_PROTO_CONNECT,      ESD_KEY_LEN  +     _INTSIZE, _NAME("CONNECT"),      emul_esd_on_connect},
 {ESD_PROTO_LOCK,         ESD_KEY_LEN  +     _INTSIZE, _NAME("LOCK"),         NULL},
 {ESD_PROTO_UNLOCK,       ESD_KEY_LEN  +     _INTSIZE, _NAME("UNLOCK"),       NULL},
 {ESD_PROTO_STREAM_PLAY,  ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_PLAY"),  NULL},
 {ESD_PROTO_STREAM_REC,   ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_REC"),   NULL},
 {ESD_PROTO_STREAM_MON,   ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_MON"),   NULL},
 {ESD_PROTO_SAMPLE_CACHE, ESD_NAME_MAX + 3 * _INTSIZE, _NAME("SAMPLE_CACHE"), NULL},
 {ESD_PROTO_SAMPLE_FREE,                     _INTSIZE, _NAME("SAMPLE_FREE"),  NULL},
 {ESD_PROTO_SAMPLE_PLAY,                     _INTSIZE, _NAME("SAMPLE_PLAY"),  NULL},
 {ESD_PROTO_SAMPLE_LOOP,                     _INTSIZE, _NAME("SAMPLE_LOOP"),  NULL},
 {ESD_PROTO_SAMPLE_STOP,                     _INTSIZE, _NAME("SAMPLE_STOP"),  NULL},
 {ESD_PROTO_SAMPLE_KILL,  0                          , _NAME("SAMPLE_KILL"),  NULL},
 {ESD_PROTO_STANDBY,      ESD_KEY_LEN +      _INTSIZE, _NAME("STANDBY"),      NULL},
 {ESD_PROTO_RESUME,       ESD_KEY_LEN +      _INTSIZE, _NAME("RESUME"),       NULL},
 {ESD_PROTO_SAMPLE_GETID, ESD_NAME_MAX               , _NAME("SAMPLE_GETID"), NULL},
 {ESD_PROTO_STREAM_FILT,  ESD_NAME_MAX + 2 * _INTSIZE, _NAME("STREAM_FILT"),  NULL},
 {ESD_PROTO_SERVER_INFO,                     _INTSIZE, _NAME("SERVER_INFO"),  NULL},
 {ESD_PROTO_ALL_INFO,                        _INTSIZE, _NAME("ALL_INFO"),     NULL},
 {ESD_PROTO_SUBSCRIBE,    0                          , _NAME("SUBSCRIBE"),    NULL},
 {ESD_PROTO_UNSUBSCRIBE,  0                          , _NAME("UNSUBSCRIBE"),  NULL},
 {ESD_PROTO_STREAM_PAN,                  3 * _INTSIZE, _NAME("STREAM_PAN"),   NULL},
 {ESD_PROTO_SAMPLE_PAN,                  3 * _INTSIZE, _NAME("SAMPLE_PAN"),   NULL},
 {ESD_PROTO_STANDBY_MODE,                    _INTSIZE, _NAME("STANDBY_MODE"), NULL},
 {ESD_PROTO_LATENCY,      0                          , _NAME("LATENCY"),      NULL},
 {ESD_PROTO_MAX,          0                          , _NAME("MAX"),          NULL},
 {-1, 0, _NAME("END OF LIST"), NULL}
};

// command handling:
int emul_esd_exec_command(int client, int cmd, struct roar_vio_calls * vio) {
 struct emul_esd_command * cur;
 void * data = NULL;
 int r;
 int i;

 if ( client == -1 || cmd < ESD_PROTO_CONNECT || cmd > ESD_PROTO_MAX || vio == NULL )
  return -1;

 for (i = 0; (cur = &(g_emul_esd_commands[i]))->cmd != -1; i++) {
  if ( cur->cmd == cmd ) {
   if ( cur->datalen > 0 ) {
    if ( (data = malloc(cur->datalen)) == NULL ) {
     // we will do a protocol error in case we do not drop the client
     clients_delete(client);
     return -1;
    }

    if ( roar_vio_read(vio, data, cur->datalen) != cur->datalen ) {
     free(data);
     clients_delete(client);
     return -1;
    }
   }

   if ( cur->handler == NULL ) {
    ROAR_WARN("emul_esd_exec_command(client=%i, cmd=%s(%i), vio=%p): client uses unimplemted command",
               client, cur->name, cmd, vio
             );
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

int emul_esd_test_byteorder(int client, void * data, struct roar_vio_calls * vio) {
 // TODO: do a real test
 return 0;
}

// handler:
int emul_esd_on_connect    (int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio) {
 return -1;
}


#endif
#endif

//ll
