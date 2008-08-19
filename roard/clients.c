//clients.c:

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

 if ( g_clients[id] == NULL )
  return -1;

 if (g_clients[id]->execed != -1) {
//  return streams_delete(g_clients[id]->execed);
  g_clients[id]->execed = -1;
 }

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  streams_delete(g_clients[id]->streams[i]);
 }

 if ( g_clients[id]->fh != -1 )
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

#define MAX_STREAMLESS 8

int clients_check_all (void) {
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
    FD_SET(fh, &r);

    if ( fh > max_fh )
     max_fh = fh;

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
   //printf("D: client=%i, stream=%i, g_clients[i=%i] = %p\n", i, j, i, g_clients[i]);
   if ( g_clients[i] == NULL ) // streams_check() bellow can delete our client (why?)
    break;
   if ( (fh = streams_get_fh(g_clients[i]->streams[j])) != -1 ) {
    if ( FD_ISSET(fh, &r) ) {
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

 con.fh = g_clients[id]->fh;

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

 if ( data )
  free(data);

 ROAR_DBG("clients_check(id=%i) = 0", id);
 return rv;
}

int clients_send_mon  (struct roar_audio_info * sa, uint32_t pos) {
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
   streams_send_mon(g_clients[i]->execed);
//   if ( streams_send_mon(g_clients[i]->execed) == -1 )
//    clients_delete(i); // delete client in case we could not write
  } 
 }

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

 if ( g_clients[client] == NULL )
  return -1;

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  if ( g_clients[client]->streams[i] == stream ) {
   g_clients[client]->execed = stream;
   streams_set_fh(stream, g_clients[client]->fh);
   streams_set_socktype(stream, ROAR_SOCKET_TYPE_GENSTR);
   return 0;
  }
 }

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

//ll
