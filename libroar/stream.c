//stream.c:

#include "libroar.h"

int roar_stream_connect (struct roar_connection * con, struct roar_stream * s, int dir) {
 struct roar_message m;

 s->dir = dir;

 m.cmd     = ROAR_CMD_NEW_STREAM;
 m.stream  = -1;
 m.pos     = 0;

 roar_stream_s2m(s, &m);

 if ( roar_req(con, &m, NULL) != 0 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK ) {
  s->id = m.stream;

  ROAR_DBG("roar_stream_connect(*) = 0");
  return 0;
 }

 ROAR_ERR("roar_stream_connect(*): Connecting new stream faild!");
 ROAR_DBG("roar_stream_connect(*) = -1");
 return -1;
}

int roar_stream_new (struct roar_stream * s, unsigned int rate,
                     unsigned int channels, unsigned int bits, unsigned int codec) {


 s->fh         = -1;
 s->id         = -1;
 s->pos        = 0;
 s->pos_rel_id = -1;

 s->dir        = ROAR_DIR_DEFAULT;

 s->datalen    = 0;
 s->offset     = 0;

 s->database   = NULL;
 s->dataoff    = NULL;

 s->info.rate     = rate;
 s->info.channels = channels;
 s->info.bits     = bits;
 s->info.codec    = codec;

 if ( bits > ROAR_BITS_MAX )
  return -1;

 return 0;
}

int roar_stream_exec    (struct roar_connection * con, struct roar_stream * s) {
 struct roar_message m;

 m.cmd     = ROAR_CMD_EXEC_STREAM;
 m.stream  = s->id;
 m.datalen = 0;
 m.pos     = 0;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_connect_to (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port) {
 struct roar_message m;

 if ( roar_stream_connect_to_ask(con, s, type, host, port) == -1 )
  return -1;

 if ( roar_recv_message(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_connect_to_ask (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port) {
 struct roar_message m;
 int len = 0;

 if ( host == NULL )
  return -1;

 m.cmd     = ROAR_CMD_CON_STREAM;
 m.stream  = s->id;
 m.pos     = 0;

 m.data[0] = 0;
 m.data[1] = type;
 ((uint16_t*)&(m.data))[1] = ROAR_HOST2NET16(port);

 len = strlen(host);

 if ( len > 76 )
  return -1;

 strncpy(&(m.data[4]), host, len);

 m.datalen = len + 4;

 if ( roar_send_message(con, &m, NULL) == -1 )
  return -1;

 return 0;
}

int roar_stream_add_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len) {
 struct roar_message m;

 m.cmd     = ROAR_CMD_ADD_DATA;
 m.stream  = s->id;
 m.datalen = len;

// if ( roar_req(con, &m, (void**)&data) == -1 )
//  return -1;
 if ( roar_send_message(con, &m, data) != 0 )
  return -1;

 if ( roar_recv_message(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}


#define _ROAR_STREAM_MESSAGE_LEN ((5+1)*4)

int roar_stream_s2m     (struct roar_stream * s, struct roar_message * m) {
 uint32_t * data;
 int i;

 if ( !(s && m) )
  return -1;

 m->datalen = _ROAR_STREAM_MESSAGE_LEN;
 data = (uint32_t*) m->data;

 data[0] = s->dir;
 data[1] = s->pos_rel_id;
 data[2] = s->info.rate;
 data[3] = s->info.bits;
 data[4] = s->info.channels;
 data[5] = s->info.codec;

 for (i = 0; i < _ROAR_STREAM_MESSAGE_LEN/4; i++)
  data[i] = ROAR_HOST2NET32(data[i]);

 ROAR_DBG("roar_stream_s2m(*): s->info:");
 roar_debug_audio_info_print(&s->info);

 return 0;
}
int roar_stream_m2s     (struct roar_stream * s, struct roar_message * m) {
 uint32_t * data;
 int i;

 if ( !(s && m) )
  return -1;

 if ( m->datalen != _ROAR_STREAM_MESSAGE_LEN )
  return -1;

 data = (uint32_t*) m->data;

 for (i = 0; i < _ROAR_STREAM_MESSAGE_LEN/4; i++)
  data[i] = ROAR_NET2HOST32(data[i]);

 s->dir           = data[0];
 s->pos_rel_id    = data[1];
 s->info.rate     = data[2];
 s->info.bits     = data[3];
 s->info.channels = data[4];
 s->info.codec    = data[5];

 ROAR_DBG("roar_stream_m2s(*): s->info:");
 roar_debug_audio_info_print(&s->info);

 return 0;
}

//ll
