//req.c:

#include "roard.h"

int req_on_noop        (int client, struct roar_message * mes, char * data) {
 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;
 return 0;
}

int req_on_identify    (int client, struct roar_message * mes, char * data) {
 struct roar_client * c;
 int max_len;

 if ( mes->datalen < 1 )
  return -1;

 clients_get(client, &c);

 if ( mes->data[0] == 1 ) {
  c->pid       = ROAR_NET2HOST32(*(uint32_t*)((mes->data)+1));

  max_len = (mes->datalen - 5) < (ROAR_BUFFER_NAME-1) ? (mes->datalen - 5) : (ROAR_BUFFER_NAME-1);

  strncpy(c->name, mes->data + 5, max_len);
  c->name[max_len] = 0;

  mes->cmd     = ROAR_CMD_OK;
  mes->datalen = 0;

  ROAR_DBG("req_on_identify(*): client=%i, pid=%i", client, c->pid);
  ROAR_DBG("req_on_identify(*) = 0");
  return 0;
 }

 return -1;
}

int req_on_auth        (int client, struct roar_message * mes, char * data) {
 // TODO: add code to support some auth.
 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;
 return 0;
}


int req_on_new_stream  (int client, struct roar_message * mes, char * data) {
 int stream;
 struct roar_stream * s;

 if ((stream = streams_new()) == -1 )
  return -1;

 if ( streams_get(stream, (struct roar_stream_server **)&s) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 if ( roar_stream_m2s(s, mes) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->stream  = stream;
 mes->datalen = 0;

 return 0;
}

int req_on_exec_stream (int client, struct roar_message * mes, char * data) {
 int r;

 if ( (r = client_stream_exec(client, mes->stream)) == -1 )
  return -1;

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_con_stream  (int client, struct roar_message * mes, char * data) {
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

 strncmp(host, &(mes->data[4]), len);
 host[len] = 0;

 if ( type > ROAR_SOCKET_TYPE_MAX )
  return -1;

 if ( type == ROAR_SOCKET_TYPE_FILE ) // disabled because of security resons
  return -1;

 if ( type == ROAR_SOCKET_TYPE_FORK ) // why should we connect to ourself?
  return -1;

 if ( (fh = roar_socket_open(ROAR_SOCKET_MODE_CONNECT, type, host, port)) == -1 )
  return -1;

 if ( client_stream_set_fh(client, mes->stream, fh) == -1 ) {
  close(fh);
  return 1;
 }

 return 0;

 mes->datalen = 0;
 mes->cmd     = ROAR_CMD_OK;
}


int req_on_set_meta    (int client, struct roar_message * mes, char * data) {
 return -1;
}


int req_on_server_oinfo    (int client, struct roar_message * mes, char * data) {
 struct roar_stream s;
//ROAR_DIR_OUTPUT

 s.dir           = ROAR_DIR_OUTPUT;
 s.pos_rel_id    = -1;
 s.info.rate     = g_sa->rate;
 s.info.bits     = g_sa->bits;
 s.info.channels = g_sa->channels;
 s.info.codec    = g_sa->codec;

 if ( roar_stream_s2m(&s, mes) == -1 )
  return -1;

 mes->cmd = ROAR_CMD_OK;

 return 0;
}


int req_on_get_standby (int client, struct roar_message * mes, char * data) {
 mes->cmd = ROAR_CMD_OK;
 mes->datalen = 2;

 *((uint16_t*)mes->data) = ROAR_HOST2NET16((unsigned) g_standby);

 return 0;
}

int req_on_set_standby (int client, struct roar_message * mes, char * data) {
 if ( mes->datalen != 2 )
  return -1;

 g_standby = ROAR_NET2HOST16(*((uint16_t*)mes->data));

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_exit      (int client, struct roar_message * mes, char * data) {
 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 alive = 0;

 return 0;
}

int req_on_list_clients(int client, struct roar_message * mes, char * data) {
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
int req_on_list_streams(int client, struct roar_message * mes, char * data) {
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

int req_on_get_client  (int client, struct roar_message * mes, char * data) {
 struct roar_client * c;

 if ( mes->datalen != 1 )
  return -1;

 if ( clients_get(mes->data[0], &c) == -1 )
  return -1;

 mes->cmd = ROAR_CMD_OK;

 return roar_ctl_c2m(mes, c);
}

int req_on_get_stream  (int client, struct roar_message * mes, char * data) {
 struct roar_stream_server * s;

 if ( mes->datalen != 1 )
  return -1;

 if ( streams_get(mes->data[0], &s) == -1 )
  return -1;

 mes->cmd = ROAR_CMD_OK;

 return roar_stream_s2m(ROAR_STREAM(s), mes);
}

int req_on_kick (int client, struct roar_message * mes, char * data) {
 uint16_t * info = (uint16_t *) mes->data;

 if ( mes->datalen != 4 )
  return -1;

 if ( info[0] == ROAR_OT_CLIENT ) {
  clients_delete(info[1]);
 } else if ( info[0] == ROAR_OT_STREAM ) {
  streams_delete(info[1]);
 } else {
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_set_vol (int client, struct roar_message * mes, char * data) {
 uint16_t * info = (uint16_t *) mes->data;
 int stream;
 struct roar_stream_server * s;
 int i;
 int chans;

 ROAR_DBG("req_on_set_vol(*) = ?");
 ROAR_DBG("req_on_set_vol(*): mes->datalen=%i", mes->datalen);

 if ( mes->datalen < (4*2) )
  return -1;

 if ( info[0] != 0 ) // version
  return -1;

 stream = info[1];
 ROAR_DBG("req_on_set_vol(*): stream=%i", stream);

 // TODO: change this code.
 //       we should not directly change the stream object but use some stream_*()-func
 //       for that job.

 if ( stream < 0 || stream >= ROAR_STREAMS_MAX )
  return -1;

 s = g_streams[stream];

 if ( s == NULL )
  return -1;

 ROAR_DBG("req_on_set_vol(*): s=%p", s);

 if ( info[2] == ROAR_SET_VOL_ALL ) {
  chans = (mes->datalen/2) - 3;
  ROAR_DBG("req_on_set_vol(*): mode is ROAR_SET_VOL_ALL, channes=%i", chans);

  if ( chans >= ROAR_MAX_CHANNELS )
   return -1;

  ROAR_DBG("req_on_set_vol(*): mixer at %p", s->mixer.mixer);

  for (i = 0; i < chans; i++) {
   s->mixer.mixer[i] = info[i+3];
   ROAR_DBG("req_on_set_vol(*): channel %i: %i", i, info[i+3]);
  }

  ROAR_DBG("req_on_set_vol(*): mixer changed!");

 } else if ( info[2] == ROAR_SET_VOL_ONE ) {
  ROAR_DBG("req_on_set_vol(*): mode is ROAR_SET_VOL_ONE");
  if ( info[3] >= ROAR_MAX_CHANNELS )
   return -1;

  s->mixer.mixer[info[3]] = info[4];
 } else {
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

int req_on_get_vol (int client, struct roar_message * mes, char * data) {
 uint16_t * info = (uint16_t *) mes->data;
 int stream;
 struct roar_stream_server * s;
 int i;
 int chans;

 ROAR_DBG("req_on_get_vol(*) = ?");
 ROAR_DBG("req_on_get_vol(*): mes->datalen=%i", mes->datalen);

 if ( mes->datalen < (2*2) )
  return -1;

 if ( info[0] != 0 ) // version
  return -1;

 stream = info[1];
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

 info[0] = 0;
 info[1] = chans = ROAR_STREAM(s)->info.channels;

 for (i = 0; i < chans; i++)
  info[2+i] = s->mixer.mixer[i];

 mes->datalen = (2 + chans)*2;
 mes->cmd = ROAR_CMD_OK;

 return 0;
}

int req_on_add_data (int client, struct roar_message * mes, char * data) {
 struct roar_buffer * b;
 char               * buf;

 if ( roar_buffer_new(&b, mes->datalen) == -1 ) {
  ROAR_ERR("req_on_add_data(*): Can not alloc buffer space!");
  ROAR_DBG("req_on_add_data(*) = -1");
  return -1;
 }

 roar_buffer_get_data(b, (void **)&buf);

 if ( data == NULL ) {
  memcpy(buf, mes->data, mes->datalen);
 } else {
  memcpy(buf, data, mes->datalen);
 }

 if ( stream_add_buffer(mes->stream, b) == -1 ) {
  roar_buffer_free(b);
  return -1;
 }

 mes->cmd     = ROAR_CMD_OK;
 mes->datalen = 0;

 return 0;
}

//ll
