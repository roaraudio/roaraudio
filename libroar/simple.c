//simple.c:

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

int roar_simple_connect (struct roar_connection * con, char * server, char * name) {

 ROAR_DBG("roar_simple_connect(*): trying to connect...");

 if ( roar_connect(con, server) == -1 ) {
  ROAR_DBG("roar_simple_connect(*): roar_connect() faild!");
  return -1;
 }

 if ( roar_identify(con, name) == -1 ) {
  ROAR_DBG("roar_simple_connect(*): roar_identify() faild!");
  return -1;
 }

 if ( roar_auth(con) == -1 ) {
  ROAR_DBG("roar_simple_connect(*): roar_auth() faild!");
  return -1;
 }

 return 0;
}

int roar_simple_stream(int rate, int channels, int bits, int codec, char * server, int dir, char * name) {
 struct roar_stream     s;

 return roar_simple_stream_obj(&s, rate, channels, bits, codec, server, dir, name);
}

int roar_simple_stream_obj  (struct roar_stream * s, int rate, int channels, int bits, int codec, char * server, int dir, char * name) {
 struct roar_connection con;

 if ( roar_simple_connect(&con, server, name) == -1 ) {
  ROAR_DBG("roar_simple_play(*): roar_simple_connect() faild!");
  ROAR_ERR("roar_simple_stream(*): Can not connect to server");
  return -1;
 }

 if ( roar_stream_new(s, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( roar_stream_connect(&con, s, dir) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( roar_stream_exec(&con, s) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( dir == ROAR_DIR_PLAY ) {
  shutdown(con.fh, SHUT_RD);
 } else if ( dir == ROAR_DIR_MONITOR || dir == ROAR_DIR_RECORD ) {
  shutdown(con.fh, SHUT_WR);
 }

 return con.fh;
}

int roar_simple_new_stream_attachexeced_obj (struct roar_connection * con, struct roar_stream * s, int rate, int channels, int bits, int codec, int dir) {
 int fh;

 if ( (fh = roar_simple_stream_obj(s, rate, channels, bits, codec, NULL /* server, we hope this goes ok here... */,
                                   dir, "libroar temp stream")) == -1 )
  return -1;

 if ( roar_stream_attach_simple(con, s, roar_get_clientid(con)) == -1 ) {
  close(fh);
  return -1;
 }

 s->fh = fh;

 return fh;
}

int roar_simple_new_stream (struct roar_connection * con, int rate, int channels, int bits, int codec, int dir) {
 struct roar_stream     s;
 return roar_simple_new_stream_obj(con, &s, rate, channels, bits, codec, dir);
}

int roar_simple_new_stream_obj (struct roar_connection * con, struct roar_stream * s, int rate, int channels, int bits, int codec, int dir) {
 struct roar_message    mes;
 char file[80];
 int fh = -1, listen;
 static int count = 0;
 struct group   * grp  = NULL;
 int    type = ROAR_SOCKET_TYPE_UNIX;
 int    port = 0;
 int    opt  = 1;
 struct sockaddr_in   socket_addr;
 socklen_t            len            = sizeof(struct sockaddr_in);
 fd_set fds;
 struct timeval timeout = {10, 0};

 if ( getsockname(con->fh, (struct sockaddr *)&socket_addr, &len) == -1 ) {
  return -1;
 }

 if ( socket_addr.sin_family == AF_INET ) {
  type = ROAR_SOCKET_TYPE_INET;
 } else if ( socket_addr.sin_family == AF_UNIX ) {
  type = ROAR_SOCKET_TYPE_UNIX;
 } else if ( socket_addr.sin_family == AF_DECnet ) {
  type = ROAR_SOCKET_TYPE_DECNET;
 } else {
  return -1;
 }

 if ( type == ROAR_SOCKET_TYPE_UNIX ) {
  snprintf(file, 79, "/tmp/.libroar-simple-stream.%i-%i", getpid(), count++);
 } else if ( type == ROAR_SOCKET_TYPE_DECNET ) {
  if ( roar_socket_get_local_nodename() ) {
   snprintf(file, 24,"%s::roar$TMP%04x%02x", roar_socket_get_local_nodename(), getpid(), count++);
  } else {
   return -1;
  }
 } else {
  strncpy(file, inet_ntoa(socket_addr.sin_addr), 79);
 }

 if ( (listen = roar_socket_listen(type, file, port)) == -1 ) {
  return -1;
 }

 if ( type == ROAR_SOCKET_TYPE_INET ) {
  len = sizeof(struct sockaddr_in);
  setsockopt(listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

  if ( getsockname(listen, (struct sockaddr *)&socket_addr, &len) == -1 ) {
   return -1;
  }
  port = ROAR_NET2HOST16(socket_addr.sin_port);
  ROAR_DBG("roar_simple_new_stream_obj(*): port=%i", port);
 } else if ( type == ROAR_SOCKET_TYPE_UNIX ) {
#ifndef ROAR_TARGET_WIN32
  chmod(file, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

  grp = getgrnam(ROAR_DEFAULT_SOCKGRP);

  if ( grp )
   chown(file, -1, grp->gr_gid);
#else
  ROAR_ERR("roar_simple_new_stream_obj(*): There is no UNIX Domain Socket support in win32, download a real OS.");
#endif
 } else if ( type == ROAR_SOCKET_TYPE_DECNET ) {
  len = sizeof(struct sockaddr_in);
  setsockopt(listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
 }

 if ( roar_stream_new(s, rate, channels, bits, codec) == -1 ) {
  return -1;
 }

 if ( roar_stream_connect(con, s, dir) == -1 ) {
  return -1;
 }

 if ( roar_stream_connect_to_ask(con, s, type, file, port) != -1 ) {

  FD_ZERO(&fds);
  FD_SET(listen, &fds);

  if ( select(listen + 1, &fds, &fds, &fds, &timeout) < 1 ) {
   close(listen);

   // we don't need to check the content as we know it failed...
   if ( roar_recv_message(con, &mes, NULL) == -1 )
    return -1;

   if ( roar_kick(con, ROAR_OT_STREAM, s->id) == -1 )
    return -1;

   return roar_simple_new_stream_attachexeced_obj(con, s, rate, channels, bits, codec, dir);
  }

  if ( (fh = accept(listen, NULL, NULL)) != -1 ) {
   if ( dir == ROAR_DIR_PLAY ) {
    shutdown(fh, SHUT_RD);
   } else if ( dir == ROAR_DIR_MONITOR || dir == ROAR_DIR_RECORD ) {
    shutdown(fh, SHUT_WR);
   }
  }
   if ( roar_recv_message(con, &mes, NULL) == -1 ) {
    close(fh);
    fh = -1;
   } else if ( mes.cmd != ROAR_CMD_OK ) {
    close(fh);
    fh = -1;
   }
 }

 close(listen);

 if ( type == ROAR_SOCKET_TYPE_UNIX ) {
  unlink(file);
 }

 s->fh = fh;

 return fh;
}


int roar_simple_play_file(char * file, char * server, char * name) {
 struct roar_connection con;

 if ( roar_simple_connect(&con, server, name) == -1 ) {
  return -1;
 }

 return roar_file_play(&con, file, 1); // con is closed by this as this stream will be an execed one.
}

int roar_simple_play(int rate, int channels, int bits, int codec, char * server, char * name) {
 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_PLAY, name);
}

int roar_simple_monitor(int rate, int channels, int bits, int codec, char * server, char * name) {
 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_MONITOR, name);
}

int roar_simple_record(int rate, int channels, int bits, int codec, char * server, char * name) {
 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_RECORD, name);
}

int roar_simple_filter(int rate, int channels, int bits, int codec, char * server, char * name) {
 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_FILTER, name);
}


int roar_simple_close(int fh) {
 return close(fh);
}

int roar_simple_get_standby (int fh) {
 struct roar_connection con;

 con.fh = fh;

 return roar_get_standby(&con);
}

//ll
