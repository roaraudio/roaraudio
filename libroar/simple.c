//simple.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008, 2009
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
 int ret;

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

 if ( (ret = roar_get_connection_fh(&con)) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( dir == ROAR_DIR_PLAY ) {
  ROAR_SHUTDOWN(ret, SHUT_RD);
 } else if ( dir == ROAR_DIR_MONITOR || dir == ROAR_DIR_RECORD ) {
  ROAR_SHUTDOWN(ret, SHUT_WR);
 }

 return ret;
}

int roar_simple_new_stream_attachexeced_obj (struct roar_connection * con, struct roar_stream * s, int rate, int channels, int bits, int codec, int dir) {
 int fh;

 if ( (fh = roar_simple_stream_obj(s, rate, channels, bits, codec, NULL /* server, we hope this is ok here... */,
                                   dir, "libroar temp stream")) == -1 )
  return -1;

 if ( roar_stream_attach_simple(con, s, roar_get_clientid(con)) == -1 ) {
#ifdef ROAR_HAVE_IO_POSIX
  close(fh);
#endif /* no else as we return -1 anyway */
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
 struct roar_libroar_config * config = roar_libroar_get_config();
 char file[80] = {0};
 int fh = -1, listen = -1;
 static int count = 0;
 int    type = ROAR_SOCKET_TYPE_UNIX;
 int    port = 0;
#if defined(ROAR_HAVE_IPV4) || defined(ROAR_HAVE_LIBDNET)
 int    opt  = 1;
#endif
#ifdef ROAR_HAVE_IPV4
 struct sockaddr_in   socket_addr;
 socklen_t            len            = sizeof(struct sockaddr_in);
#else
 struct sockaddr      socket_addr;
 socklen_t            len            = sizeof(struct sockaddr);
#endif
#ifdef ROAR_HAVE_SELECT
 fd_set fds;
 struct timeval timeout = {10, 0};
 struct roar_message    mes;
#endif
#ifdef ROAR_HAVE_UNIX
 int socks[2]; // for socketpair()
#endif

 if ( config != NULL ) {
  if ( config->workaround.workarounds & ROAR_LIBROAR_CONFIG_WAS_USE_EXECED ) {
   return roar_simple_new_stream_attachexeced_obj(con, s, rate, channels, bits, codec, dir);
  }
 }

#ifdef ROAR_HAVE_BSDSOCKETS
 if ( getsockname(roar_get_connection_fh(con), (struct sockaddr *)&socket_addr, &len) == -1 ) {
  return -1;
 }
#else
 return -1;
#endif

 if ( len == 0 ) {
#ifdef ROAR_OS_OPENBSD
  ROAR_WARN("roar_simple_new_stream_obj(*): Unknown address family: guess AF_UNIX because OS is OpenBSD");
  ((struct sockaddr*)&socket_addr)->sa_family = AF_UNIX;
#else
  return -1;
#endif
 }

 switch (((struct sockaddr*)&socket_addr)->sa_family) {
#ifdef ROAR_HAVE_UNIX
  case AF_UNIX:   type = ROAR_SOCKET_TYPE_UNIX; break;
#endif
#ifdef ROAR_HAVE_IPV4
  case AF_INET:   type = ROAR_SOCKET_TYPE_INET; break;
#endif
#ifdef ROAR_HAVE_LIBDNET
  case AF_DECnet: type = ROAR_SOCKET_TYPE_DECNET; break;
#endif
  default:
    return -1;
   break;
 }

 if ( type == ROAR_SOCKET_TYPE_DECNET ) {
  if ( roar_socket_get_local_nodename() ) {
   snprintf(file, 24,"%s::roar$TMP%04x%02x", roar_socket_get_local_nodename(), getpid(), count++);
  } else {
   return -1;
  }
#ifdef ROAR_HAVE_IPV4
 } else {
  strncpy(file, inet_ntoa(socket_addr.sin_addr), 79);
#endif
 }

 if ( type != ROAR_SOCKET_TYPE_UNIX ) {
  if ( (listen = roar_socket_listen(type, file, port)) == -1 ) {
   return -1;
  }
 }

 if ( type == ROAR_SOCKET_TYPE_INET ) {
#ifdef ROAR_HAVE_IPV4
  len = sizeof(struct sockaddr_in);
  setsockopt(listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

  if ( getsockname(listen, (struct sockaddr *)&socket_addr, &len) == -1 ) {
   return -1;
  }
  port = ROAR_NET2HOST16(socket_addr.sin_port);
  ROAR_DBG("roar_simple_new_stream_obj(*): port=%i", port);
#else
  return -1;
#endif
 } else if ( type == ROAR_SOCKET_TYPE_DECNET ) {
#ifdef ROAR_HAVE_LIBDNET
  len = sizeof(struct sockaddr_in);
  setsockopt(listen, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
#else
  return -1;
#endif
 }

 if ( roar_stream_new(s, rate, channels, bits, codec) == -1 ) {
  return -1;
 }

 if ( roar_stream_connect(con, s, dir) == -1 ) {
  return -1;
 }

 if ( type != ROAR_SOCKET_TYPE_UNIX ) {
#ifdef ROAR_HAVE_SELECT
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
    /* errr, do we need we any error handling here? */
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
#else
  return -1;
#endif
 } else { // this is type == ROAR_SOCKET_TYPE_UNIX
#ifdef ROAR_HAVE_UNIX
  if ( socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1 ) {
   roar_kick(con, ROAR_OT_STREAM, s->id); // we do not need to check for errors
                                          // as we return -1 in both whys
   return -1;
  }

  if ( roar_stream_passfh(con, s, socks[0]) == -1 ) {
   roar_kick(con, ROAR_OT_STREAM, s->id); // we do not need to check for errors
                                          // as we return -1 in both whys
   return -1;
  }

  close(socks[0]);
  fh = socks[1];
#else
  roar_kick(con, ROAR_OT_STREAM, s->id);
  return -1;
#endif
 }

/*
 if ( type == ROAR_SOCKET_TYPE_UNIX ) {
  unlink(file);
 }
*/

 if ( dir == ROAR_DIR_PLAY ) {
  ROAR_SHUTDOWN(fh, SHUT_RD);
 } else if ( dir == ROAR_DIR_MONITOR || dir == ROAR_DIR_RECORD ) {
  ROAR_SHUTDOWN(fh, SHUT_WR);
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
 roar_debug_warn_sysio("roar_simple_play", "roar_vio_simple_stream", NULL);

 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_PLAY, name);
}

int roar_simple_monitor(int rate, int channels, int bits, int codec, char * server, char * name) {
 roar_debug_warn_sysio("roar_simple_monitor", "roar_vio_simple_stream", NULL);

 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_MONITOR, name);
}

int roar_simple_record(int rate, int channels, int bits, int codec, char * server, char * name) {
 roar_debug_warn_sysio("roar_simple_record", "roar_vio_simple_stream", NULL);

 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_RECORD, name);
}

int roar_simple_filter(int rate, int channels, int bits, int codec, char * server, char * name) {
 roar_debug_warn_sysio("roar_simple_filter", "roar_vio_simple_stream", NULL);

 return roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_FILTER, name);
}

int roar_simple_connect_virtual(struct roar_connection * con, struct roar_stream * s, int parent, int dir) {
 struct roar_stream parent_stream;

 if ( con == NULL || s == NULL || parent < 0 )
  return -1;

 if ( dir == -1 ) {
  if ( roar_get_stream(con, &parent_stream, parent) == -1 )
   return -1;

  if ( (dir = roar_stream_get_dir(&parent_stream)) == -1 )
   return -1;
 }

 if ( roar_stream_set_rel_id(s, parent) == -1 )
  return -1;

 if ( roar_stream_connect(con, s, dir) == -1 )
  return -1;

 if ( roar_stream_set_flags(con, s, ROAR_FLAG_VIRTUAL, 0) == -1 ) {
  roar_kick(con, ROAR_OT_STREAM, roar_stream_get_id(s));
  return -1;
 }

 return 0;
}

int roar_simple_close(int fh) {
 roar_debug_warn_sysio("roar_simple_close", "roar_vio_close", NULL);

#ifdef ROAR_TARGET_WIN32
 closesocket(fh);
 return 0;
#else

#ifdef ROAR_HAVE_IO_POSIX
 return close(fh);
#else
 return -1;
#endif

#endif
}

int roar_simple_get_standby (int fh) {
 struct roar_connection con;

 roar_debug_warn_sysio("roar_simple_get_standby", NULL, NULL);

 if ( roar_connect_fh(&con, fh) == -1 )
  return -1;

 return roar_get_standby(&con);
}

//ll
