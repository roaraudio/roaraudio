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

 return con.fh;
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
