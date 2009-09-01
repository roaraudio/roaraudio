//clients.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

int clients_init (void) {
 int i;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++)
  g_clients[i] = NULL;

 return 0;
}

int clients_free (void) {
 int i;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++)
  if ( g_clients[i] )
   clients_delete(i);

 return 0;
}

int clients_new (void) {
 int i;
 int s;
 struct roar_client * n;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++) {
  if ( g_clients[i] == NULL ) {
   n = malloc(sizeof(struct roar_client));
   if ( n != NULL ) {
    n->pid    = -1;
    n->uid    = -1;
    n->gid    = -1;
    n->fh     = -1;

    *n->name = 0;
    *n->host = 0;

    n->proto = ROAR_PROTO_ROARAUDIO;

    n->acl   = NULL;

    n->execed = -1;
    for (s = 0; s < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; s++)
     n->streams[s] = -1;

    g_clients[i] = n;

    ROAR_DBG("clients_new(void) = %i", i);
    return i;
   } else {
    ROAR_ERR("clients_new(void): Can not alloc memory for new client: %s", strerror(errno));
    ROAR_ERR("clients_new(void) = -1");
    return -1;
   }
  }
 }

 return -1;
}

int clients_delete (int id) {
 int i;
 int close_client_fh = 1;

 if ( g_clients[id] == NULL )
  return -1;

 if (g_clients[id]->execed != -1) {
//  return streams_delete(g_clients[id]->execed);
  g_clients[id]->execed = -1;
  close_client_fh = 0;
 }

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  streams_delete(g_clients[id]->streams[i]);
 }

 if ( g_clients[id]->fh != -1 && close_client_fh )
  close(g_clients[id]->fh);

 free(g_clients[id]);
 g_clients[id] = NULL;

 ROAR_DBG("clients_delete(id=%i) = 0", id);
 return 0;
}

int clients_get       (int id, struct roar_client ** client) {
 *client = g_clients[id];

 if ( *client == NULL )
  return -1;

 return 0;
}

int clients_set_fh    (int id, int    fh) {

 if ( g_clients[id] == NULL )
  return -1;

 g_clients[id]->fh = fh;

 return 0;
}

int clients_get_fh    (int id) {
 if ( g_clients[id] == NULL )
  return -1;

 return g_clients[id]->fh;
}

int clients_set_pid   (int id, int    pid) {
 if ( g_clients[id] == NULL )
  return -1;

 g_clients[id]->pid = pid;

 return 0;
}

int clients_set_uid   (int id, int    uid) {
 if ( g_clients[id] == NULL )
  return -1;

 g_clients[id]->uid = uid;

 return 0;
}

int clients_set_gid   (int id, int    gid) {
 if ( g_clients[id] == NULL )
  return -1;

 g_clients[id]->gid = gid;

 return 0;
}

int clients_set_proto (int id, int    proto) {
 if ( g_clients[id] == NULL )
  return -1;

 g_clients[id]->proto = proto;

 return 0;
}

#define MAX_STREAMLESS 8

int clients_check_all (void) {
#ifdef ROAR_HAVE_SELECT
 struct timeval tv;
 fd_set r, e;
 int i, j;
 int ret;
 int fh;
 int max_fh = -1;
 int have = 0;
 struct {
  int id;
  int fh;
 } streamless[MAX_STREAMLESS];
 int have_streamless = 0;
 int have_stream;

 FD_ZERO(&r);
 FD_ZERO(&e);

 tv.tv_sec  = 0;
 tv.tv_usec = 1;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++) {
  if ( g_clients[i] == NULL )
   continue;

  if ( (fh = g_clients[i]->fh) != -1 ) {
   have++;

   ROAR_DBG("clients_check_all(*): fh=%i", fh);

   FD_SET(fh, &r);
   FD_SET(fh, &e);

   if ( fh > max_fh )
    max_fh = fh;
  }

  have_stream = 0;

  for (j = 0; j < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; j++) {
   if ( (fh = streams_get_fh(g_clients[i]->streams[j])) != -1 ) {
    if ( fh > -1 ) {
     FD_SET(fh, &r);

     if ( fh > max_fh )
      max_fh = fh;
    }

    have_stream = 1;
   }
   //printf("D: client=%i, stream=%i, fh=%i\n", i, j, fh);
  }

  if ( !have_stream && have_streamless < MAX_STREAMLESS ) {
   streamless[have_streamless  ].id = i;
   if ( (streamless[have_streamless++].fh = g_clients[i]->fh) == -1 )
    have_streamless--;
  }
 }

 if ( max_fh == -1 )
  return 0;

 if ( (ret = select(max_fh + 1, &r, NULL, &e, &tv)) < 1 ) {
  return ret < 0 ? ret : have;
 }

 for (i = 0; i < ROAR_CLIENTS_MAX; i++) {
  if ( g_clients[i] == NULL )
   continue;

  if ( (fh = g_clients[i]->fh) != -1 ) {
   if ( FD_ISSET(fh, &r) ) {
    if ( g_clients[i]->execed == -1 ) {
     clients_check(i);
     if ( g_clients[i] != NULL && g_clients[i]->execed != -1 ) {
      FD_CLR(fh, &r);
     }
/*
    } else {
     streams_check(g_clients[i]->execed);
*/
    }
   }

   if ( FD_ISSET(fh, &e) ) {
    clients_delete(i);
    continue;
   }
  }

  if ( g_clients[i] == NULL )
   continue;

  for (j = 0; j < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; j++) {
   ROAR_DBG("clients_check_all(*): D: client=%i, stream=%i, g_clients[i=%i] = %p", i, j, i, g_clients[i]);
   if ( g_clients[i] == NULL ) // streams_check() bellow can delete our client (why?)
    break;

   //ROAR_WARN("clients_check_all(*): client=%i: client exists", i);
   ROAR_DBG("clients_check_all(*): client=%i, stream=%i: id=%i", i, j, g_clients[i]->streams[j]);

   if ( (fh = streams_get_fh(g_clients[i]->streams[j])) != -1 ) {
    ROAR_DBG("clients_check_all(*): client=%i, stream=%i: fh=%i", i, j, fh);
    if ( fh == -2 ) {
     streams_check(g_clients[i]->streams[j]);
    } else if ( FD_ISSET(fh, &r) ) {
     streams_check(g_clients[i]->streams[j]);
    }
   }
  }
 }

 if ( have_streamless ) {
   FD_ZERO(&r);

   tv.tv_sec  = 0;
   tv.tv_usec = 1;

   max_fh = -1;

   for (i = 0; i < have_streamless; i++) {
    if ( ! g_clients[j = streamless[i].id] )
     continue;

    if ( g_clients[j]->execed != -1 )
     continue;

    fh = streamless[i].fh;

    ROAR_DBG("clients_check_all(void): fh=%i", fh);
    FD_SET(fh, &r);

    if ( fh > max_fh )
     max_fh = fh;
   }

   if ( (ret = select(max_fh + 1, &r, NULL, NULL, &tv)) < 0 ) {
    return ret;
   }

   for (i = 0; i < have_streamless; i++) {
    if ( FD_ISSET(streamless[i].fh, &r) ) {
     clients_check(streamless[i].id);
    }
   }
 }

 ROAR_DBG("clients_check_all(void) = %i // have value", have);
 return have;
#else
 return -1;
#endif
}

int clients_check     (int id) {
 struct roar_message    m;
 struct roar_connection con;
 char * data = NULL;
 int oldcmd;
 int r;
 int rv = 0;

 if ( g_clients[id] == NULL )
  return -1;
 if ( g_clients[id]->fh == -1 )
  return -1;

 roar_connect_fh(&con, g_clients[id]->fh);

 switch (g_clients[id]->proto) {
  case ROAR_PROTO_ROARAUDIO:
    r = roar_recv_message(&con, &m, &data);

    if ( r == -1 ) { // should we drop the client?
     clients_delete(id);
     return -1;
    }

    roar_debug_message_print(&m);

    oldcmd = m.cmd;

    if ( (r = command_exec(id, &m, data)) == -1 ) {
     m.cmd     = ROAR_CMD_ERROR;
     m.datalen = 0;
     ROAR_DBG("clients_check(*): Exec of command faild!");
    } else {
     if ( m.cmd == oldcmd ) {
      m.cmd     = ROAR_CMD_OK;
      m.datalen = 0;
     } else if ( m.cmd == ROAR_CMD_OK_STOP ) {
      m.cmd     = ROAR_CMD_OK;
      rv        = 1;
     }
    }

    roar_send_message(&con, &m, NULL);
   break;
  default:
    rv = -1;
 }

 if ( data )
  free(data);

 ROAR_DBG("clients_check(id=%i) = %i", id, rv);
 return rv;
}

int clients_send_mon  (struct roar_audio_info * sa, uint32_t pos) {
 int i;
// int fh;
 int j;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++) {
  if ( g_clients[i] == NULL )
   continue;

/*
  if ( (fh = g_clients[i]->fh) == -1 )
   continue;
*/

  ROAR_DBG("clients_send_mon(*): client=%i, execed=%i", i, g_clients[i]->execed);

  if ( g_clients[i]->execed == -1 ) {
   // TODO: add some code to send a message to the client insetd of the raw data.
   for (j = 0; j < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; j++) {
    //if ( (fh = streams_get_fh(g_clients[i]->streams[j])) != -1 ) {
    if ( g_clients[i]->streams[j] != -1 ) {
     ROAR_DBG("clients_send_mon(*): client=%i, stream=%i -> %i", i, j, g_clients[i]->streams[j]);
     streams_send_mon(g_clients[i]->streams[j]);
    }
   }
  } else {
//   streams_check(g_clients[i]->execed);
   streams_send_mon(g_clients[i]->execed);
//   if ( streams_send_mon(g_clients[i]->execed) == -1 )
//    clients_delete(i); // delete client in case we could not write
  } 
 }

 // TODO: FIXME: should this really be -1?
 return -1;
}

int clients_send_filter(struct roar_audio_info * sa, uint32_t pos) {
 int i;
 int fh;

 for (i = 0; i < ROAR_CLIENTS_MAX; i++) {
  if ( g_clients[i] == NULL )
   continue;

  if ( (fh = g_clients[i]->fh) == -1 )
   continue;

  if ( g_clients[i]->execed == -1 ) {
   // TODO: add some code to send a message to the client insetd of the raw data.
  } else {
//   streams_check(g_clients[i]->execed);
   streams_send_filter(g_clients[i]->execed);
//   if ( streams_send_mon(g_clients[i]->execed) == -1 )
//    clients_delete(i); // delete client in case we could not write
  }
 }

 return -1;
}

int client_stream_exec   (int client, int stream) {
 int i;

 if ( g_clients[client] == NULL ) {
  ROAR_WARN("client_stream_exec(client=%i, stream=%i) = -1 // client does not exist", client, stream);
  return -1;
 }

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  if ( g_clients[client]->streams[i] == stream ) {
   g_clients[client]->execed = stream;
   streams_set_fh(stream, g_clients[client]->fh);
   streams_set_socktype(stream, ROAR_SOCKET_TYPE_GENSTR);
   return 0;
  }
 }

 ROAR_WARN("client_stream_exec(client=%i, stream=%i) = -1 // client does not own stream", client, stream);
 return -1;
}

int client_stream_set_fh (int client, int stream, int fh) {
 int i;

 if ( g_clients[client] == NULL )
  return -1;

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  if ( g_clients[client]->streams[i] == stream ) {
   streams_set_fh(stream, fh);
   return 0;
  }
 }

 return -1;
}

int client_stream_add    (int client, int stream) {
 int i;

 if ( g_clients[client] == NULL )
  return -1;

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  if ( g_clients[client]->streams[i] == -1 ) {
   g_clients[client]->streams[i] = stream;
   streams_set_client(stream, client);
   return 0;
  }
 }

 return -1;
}

int client_stream_delete (int client, int stream) {
 int i;

 if ( g_clients[client] == NULL )
  return -1;

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  if ( g_clients[client]->streams[i] == stream ) {
   g_clients[client]->streams[i] = -1;

   if ( stream == g_clients[client]->execed ) {
    ROAR_DBG("client_stream_delete(client=%i, stream=%i): stream is execed one, deleting client!", client, stream);
    clients_delete(client);
   }

   ROAR_DBG("client_stream_delete(client=%i, stream=%i) = 0", client, stream);
   return 0;
  }
 }

 ROAR_DBG("client_stream_delete(client=%i, stream=%i) = -1", client, stream);
 return -1;
}

int client_stream_move   (int client, int stream) {
 int old_client = streams_get_client(stream);

 ROAR_DBG("client_stream_move(client=%i, stream=%i): old_client = %i", client, stream, old_client);

 if ( old_client != -1 )
  if ( client_stream_delete(old_client, stream) == -1 )
   return -1;

 return client_stream_add(client, stream); 
}

//ll
