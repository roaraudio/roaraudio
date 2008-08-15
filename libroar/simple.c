//simple.c:

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
 struct roar_connection con;
 struct roar_stream     s;

 if ( roar_simple_connect(&con, server, name) == -1 ) {
  ROAR_DBG("roar_simple_play(*): roar_simple_connect() faild!");
  return -1;
 }

 if ( roar_stream_new(&s, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( roar_stream_connect(&con, &s, dir) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( roar_stream_exec(&con, &s) == -1 ) {
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
  sprintf(file, "/tmp/.libroar-simple-stream.%i-%i", getpid(), count++);
 } else if ( type == ROAR_SOCKET_TYPE_DECNET ) {
  if ( roar_socket_get_local_nodename() ) {
   sprintf(file, "%s::roar$TMP%04x%02x", roar_socket_get_local_nodename(), getpid(), count++);
  } else {
   return -1;
  }
 } else {
  strcpy(file, inet_ntoa(socket_addr.sin_addr));
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
 }

 if ( type == ROAR_SOCKET_TYPE_UNIX ) {
  chmod(file, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP);

  grp = getgrnam(ROAR_DEFAULT_SOCKGRP);

  if ( grp )
   chown(file, -1, grp->gr_gid);
 }

 if ( roar_stream_new(s, rate, channels, bits, codec) == -1 ) {
  return -1;
 }

 if ( roar_stream_connect(con, s, dir) == -1 ) {
  return -1;
 }

 if ( roar_stream_connect_to_ask(con, s, type, file, port) != -1 ) {

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
