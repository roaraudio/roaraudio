//basic.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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
#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
 struct passwd * pwd;
#endif
#ifdef ROAR_HAVE_LIBDNET
 struct stat decnet_stat;
#endif
#ifdef ROAR_HAVE_LIBX11
 struct roar_x11_connection * x11con;
#endif

 roar_errno = ROAR_ERROR_UNKNOWN;

 // load config
 roar_libroar_get_config();

 if ( server == NULL )
  server = roar_libroar_get_server();

 if ( server == NULL && (roar_server = getenv("ROAR_SERVER")) != NULL )
  server = roar_server;

#ifdef ROAR_HAVE_LIBX11
 if ( server == NULL ) {
  if ( (x11con = roar_x11_connect(NULL)) != NULL ) {
   server = roar_x11_get_prop(x11con, "ROAR_SERVER");
   roar_x11_disconnect(x11con);
  }
 }
#endif

#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
 if ( server == NULL && (i = readlink("/etc/roarserver", user_sock, 79)) != -1 ) {
   user_sock[i] = 0;
   server = user_sock;
 }
#endif

 if ( server != NULL && !strcasecmp(server, "+slp") ) {
  server = roar_slp_find_roard(0);
  if ( server == NULL ) {
   return -1;
  }
 }

 if ( server == NULL || *server == 0 ) {
  /* connect via defaults */

#ifdef ROAR_HAVE_UNIX
#ifndef ROAR_TARGET_MICROCONTROLLER
  roar_server = getenv("HOME");
#else
  roar_server = NULL;
#endif

  if ( roar_server == NULL ) {
#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
   if ( (pwd = getpwuid(getuid())) == NULL ) {
    roar_server = "/NX-HOME-DIR";
   } else {
    roar_server = pwd->pw_dir;
   }
#else
   roar_server = "/WIN32-SUCKS";
#endif
  }

  snprintf(user_sock, 79, "%s/%s", roar_server, ROAR_DEFAULT_SOCK_USER);

  if ( (fh = roar_socket_connect(user_sock, 0)) != -1 )
   return fh;

  if ( (fh = roar_socket_connect(ROAR_DEFAULT_SOCK_GLOBAL, 0)) != -1 )
   return fh;
#endif

  if ( (fh = roar_socket_connect(ROAR_DEFAULT_HOST, ROAR_DEFAULT_PORT)) != -1 )
   return fh;

#ifdef ROAR_HAVE_LIBDNET
  if ( stat(ROAR_PROC_NET_DECNET, &decnet_stat) == 0 ) {
   if ( roar_socket_get_local_nodename() ) {
    snprintf(user_sock, 79, "%s::%s", roar_socket_get_local_nodename(), ROAR_DEFAULT_OBJECT);
    if ( (fh = roar_socket_connect(user_sock, ROAR_DEFAULT_NUM)) != -1 )
     return fh;
   }
  }
#endif

#ifdef ROAR_HAVE_LIBSLP
 if ( (server = roar_slp_find_roard(0)) != NULL )
  if ( (fh = roar_connect_raw(server)) != -1 )
   return fh;

 /* in case we can not connect to the server given this may be a cache problem,
    we do a new lookup with the cache disabled in this case                     */
 ROAR_WARN("roar_connect_raw(*): Can not connect to SLP located server, disabling cache");
 if ( (server = roar_slp_find_roard(1)) != NULL )
  if ( (fh = roar_connect_raw(server)) != -1 )
   return fh;
#endif

 return -1;

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
     strncat(user_sock, roar_socket_get_local_nodename(), 6);
   }

   strncat(user_sock, server, 79);
   server = user_sock;
   if ( *obj == 0 ) {
#ifdef DN_MAXOBJL
    strncat(user_sock, ROAR_DEFAULT_OBJECT, DN_MAXOBJL+2);
#else
    ROAR_ERR("roar_connect_raw(*): size of DECnet object unknown.");
#endif
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
 int fh;

 if ( con == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 roar_errno = ROAR_ERROR_UNKNOWN;
 fh = roar_connect_raw(server);

 if ( fh == -1 )
  return -1;

 return roar_connect_fh(con, fh);
}

int roar_connect_fh (struct roar_connection * con, int fh) {

 if ( con == NULL || fh == -1 ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 // specal hack to set an ilegal value used internaly in libroar:
 if ( fh == -2 )
  fh = -1;

 memset(con, 0, sizeof(struct roar_connection));

 con->flags = ROAR_CON_FLAGS_FH;
 con->__fh  = fh;

 if ( roar_vio_open_fh_socket(&(con->viocon), fh) != -1 )
  con->flags |= ROAR_CON_FLAGS_VIO;

 roar_errno = ROAR_ERROR_NONE;
 return 0;
}

int roar_get_connection_fh (struct roar_connection * con) {
 roar_debug_warn_sysio("roar_get_connection_fh", "roar_get_connection_vio2", NULL);

 if ( con == NULL )
  return -1;

 if ( !(con->flags & ROAR_CON_FLAGS_FH) )
  return -1;

 return con->__fh;
}

int roar_get_connection_vio (struct roar_connection * con, struct roar_vio_calls * vio) {
 roar_debug_warn_obsolete("roar_get_connection_vio", "roar_get_connection_vio2", NULL);

 if ( con == NULL || vio == NULL )
  return -1;

 if ( !(con->flags & ROAR_CON_FLAGS_FH) )
  return -1;

 return roar_vio_open_fh_socket(vio, con->__fh);
}

struct roar_vio_calls * roar_get_connection_vio2 (struct roar_connection * con) {
 if ( con == NULL )
  return NULL;

 if ( con->flags & ROAR_CON_FLAGS_VIO )
  return &(con->viocon);

// TODO: try to open the VIO.

 return NULL;
}

int roar_disconnect (struct roar_connection * con) {
 struct roar_vio_calls * vio;
 struct roar_message m;

 memset(&m, 0, sizeof(m));

 m.datalen = 0;
 m.stream  = 0;
 m.pos     = 0;
 m.cmd     = ROAR_CMD_QUIT;

 roar_req(con, &m, NULL);

 if ( (vio = roar_get_connection_vio2(con)) != NULL ) {
  roar_vio_close(vio);
 }

 roar_connect_fh(con, -2);

 roar_errno = ROAR_ERROR_NONE;

 return 0;
}

int roar_set_connection_callback(struct roar_connection * con,
                                 void (*cb)(struct roar_connection * con,
                                            struct roar_message    * mes,
                                            void                   * userdata),
                                 void * userdata) {
 if ( con == NULL )
  return -1;

 con->cb       = cb;
 con->userdata = userdata;

 return 0;
}


int roar_noop         (struct roar_connection * con) {
 struct roar_message mes;

 if ( con == NULL ) {
  return -1;
 }

 memset(&mes, 0, sizeof(mes));

 mes.cmd = ROAR_CMD_NOOP;

 return roar_req(con, &mes, NULL);
}

int roar_identify   (struct roar_connection * con, char * name) {
 struct roar_message mes;
 pid_t pid;
 int max_len;

 roar_errno = ROAR_ERROR_UNKNOWN;

 ROAR_DBG("roar_identify(*): try to identify myself...");

 memset(&mes, 0, sizeof(mes));

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
