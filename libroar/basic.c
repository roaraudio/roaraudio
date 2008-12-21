//basic.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

int roar_connect_raw (char * server) {
 char user_sock[80];
 char * roar_server;
 int i = 0;
 int port = 0;
 int fh = -1;
 int is_decnet = 0;
 char * obj = NULL;
 struct passwd * pwd;
#ifdef ROAR_HAVE_LIBDNET
 struct stat decnet_stat;
#endif

 roar_errno = ROAR_ERROR_UNKNOWN;

 if ( server == NULL && (roar_server = getenv("ROAR_SERVER")) != NULL )
  server = roar_server;

 if ( server == NULL && (i = readlink("/etc/roarserver", user_sock, 79)) != -1 ) {
   user_sock[i] = 0;
   server = user_sock;
 }

 if ( server == NULL || *server == 0 ) {
  /* connect via defaults */


  roar_server = getenv("HOME");

  if ( roar_server == NULL ) {
   if ( (pwd = getpwuid(getuid())) == NULL ) {
    roar_server = "/NX-HOME-DIR";
   } else {
    roar_server = pwd->pw_dir;
   }
  }

  snprintf(user_sock, 79, "%s/%s", roar_server, ROAR_DEFAULT_SOCK_USER);

  if ( (fh = roar_socket_connect(user_sock, 0)) != -1 )
   return fh;

  if ( (fh = roar_socket_connect(ROAR_DEFAULT_SOCK_GLOBAL, 0)) != -1 )
   return fh;

  if ( (fh = roar_socket_connect(ROAR_DEFAULT_HOST, ROAR_DEFAULT_PORT)) != -1 )
   return fh;

#ifdef ROAR_HAVE_LIBDNET
  if ( stat(ROAR_PROC_NET_DECNET, &decnet_stat) == 0 ) {
   if ( roar_socket_get_local_nodename() ) {
    snprintf(user_sock, 79, "%s::%s", roar_socket_get_local_nodename(), ROAR_DEFAULT_OBJECT);
    return roar_socket_connect(user_sock, ROAR_DEFAULT_NUM);
   }
  }
#endif

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

 if ( fh == -1 )
  roar_errno = ROAR_ERROR_CONNREFUSED;

 ROAR_DBG("roar_connect_raw(*) = %i", fh);

 return fh;
}

int roar_connect    (struct roar_connection * con, char * server) {
 roar_errno = ROAR_ERROR_UNKNOWN;
 con->fh = roar_connect_raw(server);

 if ( con->fh == -1 )
  return -1;

 roar_errno = ROAR_ERROR_NONE;

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

 roar_errno = ROAR_ERROR_NONE;

 return 0;
}

int roar_identify   (struct roar_connection * con, char * name) {
 struct roar_message mes;
 pid_t pid;
 int max_len;

 roar_errno = ROAR_ERROR_UNKNOWN;

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

 roar_errno = ROAR_ERROR_UNKNOWN;

 ROAR_DBG("roar_send_message(*): try to send an request...");

 buf[0] = _ROAR_MESSAGE_VERSION;
 buf[1] = (unsigned char) mes->cmd;
 *(uint16_t*)(buf+2) = ROAR_HOST2NET16(mes->stream);
 *(uint32_t*)(buf+4) = ROAR_HOST2NET32(mes->pos);
 *(uint16_t*)(buf+8) = ROAR_HOST2NET16(mes->datalen);

 if ( write(con->fh, buf, _ROAR_MESS_BUF_LEN) != _ROAR_MESS_BUF_LEN ) {
  roar_errno = ROAR_ERROR_PIPE;
  return -1;
 }

 if ( mes->datalen != 0 ) {
  if ( write(con->fh, data == NULL ? mes->data : data, mes->datalen) != mes->datalen ) {
   roar_errno = ROAR_ERROR_PIPE;
   return -1;
  }
 }

 roar_errno = ROAR_ERROR_NONE;

 ROAR_DBG("roar_send_message(*) = 0");
 return 0;
}

int roar_recv_message (struct roar_connection * con, struct roar_message * mes, char ** data) {
 char buf[_ROAR_MESS_BUF_LEN];

 roar_errno = ROAR_ERROR_UNKNOWN;

 ROAR_DBG("roar_recv_message(*): try to get a response form the server...");

 if ( data )
  *data = NULL;

 if ( read(con->fh, buf, _ROAR_MESS_BUF_LEN) != _ROAR_MESS_BUF_LEN ) {
  roar_errno = ROAR_ERROR_PROTO;
  return -1;
 }

 ROAR_DBG("roar_recv_message(*): Got a header");

 if ( buf[0] != _ROAR_MESSAGE_VERSION ) {
  roar_errno = ROAR_ERROR_PROTO;
  return -1;
 }

 mes->cmd     = (unsigned char)buf[1];
 mes->stream  = ROAR_NET2HOST16(*(uint16_t*)(buf+2));
 mes->pos     = ROAR_NET2HOST32(*(uint32_t*)(buf+4));
 mes->datalen = ROAR_NET2HOST16(*(uint16_t*)(buf+8));

 ROAR_DBG("roar_recv_message(*): command=%i(%s)", mes->cmd,
           mes->cmd == ROAR_CMD_OK ? "OK" : (mes->cmd == ROAR_CMD_ERROR ? "ERROR" : "UNKNOWN"));

 if ( mes->datalen == 0 ) {
  ROAR_DBG("roar_recv_message(*): no data in this pkg");
  ROAR_DBG("roar_recv_message(*) = 0");
  roar_errno = ROAR_ERROR_NONE;
  return 0;
 }

 if ( mes->datalen <= LIBROAR_BUFFER_MSGDATA ) {
  if ( read(con->fh, mes->data, mes->datalen) == mes->datalen ) {
   ROAR_DBG("roar_recv_message(*): Got data!");
   ROAR_DBG("roar_recv_message(*) = 0");
   roar_errno = ROAR_ERROR_NONE;
   return 0;
  }

  roar_errno = ROAR_ERROR_PIPE;
  return -1;
 } else {
  if ( data == NULL ) {
   roar_errno = ROAR_ERROR_MSGSIZE;
   return -1;
  }

  if ( (*data = malloc(mes->datalen)) == NULL ) {
   roar_errno = ROAR_ERROR_NOMEM;
   return -1;
  }

  if ( mes->datalen == 0 ) {
   roar_errno = ROAR_ERROR_NONE;
   return 0;
  }

  if ( read(con->fh, *data, mes->datalen) == mes->datalen ) {
   ROAR_DBG("roar_recv_message(*): Got data!");
   ROAR_DBG("roar_recv_message(*) = 0");
   roar_errno = ROAR_ERROR_NONE;
   return 0;
  }

  roar_errno = ROAR_ERROR_PIPE;
  return -1;
 }

 // what happened here?
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
