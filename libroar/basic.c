//basic.c:

#include "libroar.h"

int roar_connect_raw (char * server) {
 char user_sock[80];
 char * roar_server;
 int i;
 int port = 0;
 int fh = -1;
 int is_decnet = 0;
 char * obj = NULL;

 if ( server == NULL && (roar_server = getenv("ROAR_SERVER")) != NULL )
  server = roar_server;

 if ( server == NULL && (i = readlink("/etc/roarserver", user_sock, 79)) != -1 ) {
   user_sock[i] = 0;
   server = user_sock;
 }

 if ( server == NULL || *server == 0 ) {
  /* connect via defaults */


  snprintf(user_sock, 79, "%s/%s", getenv("HOME"), ROAR_DEFAULT_SOCK_USER);

  if ( (fh = roar_socket_connect(user_sock, 0)) != -1 )
   return fh;

  if ( (fh = roar_socket_connect(ROAR_DEFAULT_SOCK_GLOBAL, 0)) != -1 )
   return fh;

  if ( (fh = roar_socket_connect(ROAR_DEFAULT_HOST, ROAR_DEFAULT_PORT)) != -1 )
   return fh;

  if ( roar_socket_get_local_nodename() ) {
   snprintf(user_sock, 79, "%s::%s", roar_socket_get_local_nodename(), ROAR_DEFAULT_OBJECT);
   return roar_socket_connect(user_sock, ROAR_DEFAULT_NUM);
  }

 } else {
  /* connect via (char*)server */
  // find a port:
  if ( *server != '/' ) { // don't test AF_UNIX sockets for ports
   for (i = 0; server[i] != 0; i++) {
    if ( server[i] == ':' ) {
     if ( server[i+1] == ':' ) { // DECnet, leave unchanged
      is_decnet = 1;
      obj = &server[i+2];
      break;
     }

     port = atoi(server+i+1);
     server[i] = 0;
     break;
    }
   }
  }

  if ( is_decnet ) {
    *user_sock = 0;
   if ( *server == ':' ) {
    if ( roar_socket_get_local_nodename() )
     strcat(user_sock, roar_socket_get_local_nodename());
   }

   strcat(user_sock, server);
   server = user_sock;
   if ( *obj == 0 ) {
    strcat(user_sock, ROAR_DEFAULT_OBJECT);
   }
  }

  if ( port || is_decnet ) {
   fh = roar_socket_connect(server, port);
   // restore the original string
   server[i] = ':';
  } else {
   fh = roar_socket_connect(server, ROAR_DEFAULT_PORT);
  }
 }

 ROAR_DBG("roar_connect_raw(*) = %i", fh);

 return fh;
}

int roar_connect    (struct roar_connection * con, char * server) {
 con->fh = roar_connect_raw(server);

 if ( con->fh == -1 )
  return -1;

 return 0;
}

int roar_disconnect (struct roar_connection * con) {
 struct roar_message m;

 m.datalen = 0;
 m.stream  = 0;
 m.pos     = 0;
 m.cmd     = ROAR_CMD_QUIT;

 roar_req(con, &m, NULL);

 close(con->fh);

 con->fh = -1;

 return 0;
}

int roar_identify   (struct roar_connection * con, char * name) {
 struct roar_message mes;
 pid_t pid;
 int max_len;

 ROAR_DBG("roar_identify(*): try to identify myself...");

 mes.cmd    = ROAR_CMD_IDENTIFY;
 mes.stream = 0;
 mes.pos    = 0;

 ROAR_DBG("roar_identify(*): name=%p", name);

 if ( name == NULL )
  name = "libroar client";

 ROAR_DBG("roar_identify(*): name=%p", name);

 max_len = strlen(name);
 ROAR_DBG("roar_identify(*): strlen(name) = %i", max_len);

 if ( max_len > (LIBROAR_BUFFER_MSGDATA - 5) )
  max_len = LIBROAR_BUFFER_MSGDATA - 5;

 mes.datalen = 5 + max_len;
 mes.data[0] = 1;

 pid = getpid();
 *(uint32_t*)(mes.data+1) = ROAR_HOST2NET32(pid);
 ROAR_DBG("roar_identify(*): pid = %i", pid);

 strncpy(mes.data+5, name, max_len);

 return roar_req(con, &mes, NULL);
}

#define _ROAR_MESS_BUF_LEN (1 /* version */ + 1 /* cmd */ + 2 /* stream */ + 4 /* pos */ + 2 /* datalen */)
int roar_send_message (struct roar_connection * con, struct roar_message * mes, char * data) {
 char buf[_ROAR_MESS_BUF_LEN];

 ROAR_DBG("roar_send_message(*): try to send an request...");

 buf[0] = _ROAR_MESSAGE_VERSION;
 buf[1] = (unsigned char) mes->cmd;
 *(uint16_t*)(buf+2) = ROAR_HOST2NET16(mes->stream);
 *(uint32_t*)(buf+4) = ROAR_HOST2NET32(mes->pos);
 *(uint16_t*)(buf+8) = ROAR_HOST2NET16(mes->datalen);

 if ( write(con->fh, buf, _ROAR_MESS_BUF_LEN) != _ROAR_MESS_BUF_LEN )
  return -1;

 if ( mes->datalen != 0 )
  if ( write(con->fh, data == NULL ? mes->data : data, mes->datalen) != mes->datalen )
   return -1;

 ROAR_DBG("roar_send_message(*) = 0");
 return 0;
}

int roar_recv_message (struct roar_connection * con, struct roar_message * mes, char ** data) {
 char buf[_ROAR_MESS_BUF_LEN];

 ROAR_DBG("roar_recv_message(*): try to get a response form the server...");

 if ( data )
  *data = NULL;

 if ( read(con->fh, buf, _ROAR_MESS_BUF_LEN) != _ROAR_MESS_BUF_LEN )
  return -1;

 ROAR_DBG("roar_recv_message(*): Got a header");

 if ( buf[0] != _ROAR_MESSAGE_VERSION )
  return -1;

 mes->cmd     = (unsigned char)buf[1];
 mes->stream  = ROAR_NET2HOST16(*(uint16_t*)(buf+2));
 mes->pos     = ROAR_NET2HOST32(*(uint32_t*)(buf+4));
 mes->datalen = ROAR_NET2HOST16(*(uint16_t*)(buf+8));

 ROAR_DBG("roar_recv_message(*): command=%i(%s)", mes->cmd,
           mes->cmd == ROAR_CMD_OK ? "OK" : (mes->cmd == ROAR_CMD_ERROR ? "ERROR" : "UNKNOWN"));

 if ( mes->datalen == 0 ) {
  ROAR_DBG("roar_recv_message(*): no data in this pkg");
  ROAR_DBG("roar_recv_message(*) = 0");
  return 0;
 }

 if ( mes->datalen <= LIBROAR_BUFFER_MSGDATA ) {
  if ( read(con->fh, mes->data, mes->datalen) == mes->datalen ) {
   ROAR_DBG("roar_recv_message(*): Got data!");
   ROAR_DBG("roar_recv_message(*) = 0");
   return 0;
  }
  return -1;
 } else {
  if ( data == NULL )
   return -1;

  if ( (*data = malloc(mes->datalen)) == NULL )
   return -1;

  if ( mes->datalen == 0 )
   return 0;

  if ( read(con->fh, *data, mes->datalen) == mes->datalen ) {
   ROAR_DBG("roar_recv_message(*): Got data!");
   ROAR_DBG("roar_recv_message(*) = 0");
   return 0;
  }
  return -1;
 }

 return -1;
}

int roar_req (struct roar_connection * con, struct roar_message * mes, char ** data) {
 if ( roar_send_message(con, mes, data ? *data : NULL) != 0 )
  return -1;

 if ( data )
  free(*data);

 return roar_recv_message(con, mes, data);
}

int roar_debug_message_print (struct roar_message * mes) {
 if ( mes == NULL )
  return -1;

 ROAR_DBG("roar_debug_message_print(*): Command: %i", mes->cmd);
 ROAR_DBG("roar_debug_message_print(*): Stream : %u", mes->stream);
 ROAR_DBG("roar_debug_message_print(*): Pos    : %u", mes->pos);
 ROAR_DBG("roar_debug_message_print(*): Datalen: %i", mes->datalen);

 ROAR_DBG("roar_debug_message_print(*) = 0");
 return 0;
}

int roar_debug_audio_info_print (struct roar_audio_info * info) {
 if ( info == NULL )
  return -1;

 ROAR_DBG("roar_debug_audio_info_print(*): Rate    : %i", info->rate);
 ROAR_DBG("roar_debug_audio_info_print(*): Channels: %i", info->channels);
 ROAR_DBG("roar_debug_audio_info_print(*): Bits    : %i", info->bits);
 ROAR_DBG("roar_debug_audio_info_print(*): Codec   : %i", info->codec);

 ROAR_DBG("roar_debug_audio_info_print(*) = 0");
 return 0;
}

//ll
