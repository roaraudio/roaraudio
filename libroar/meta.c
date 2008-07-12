//meta.c:

#include "libroar.h"

int roar_stream_meta_set (struct roar_connection * con, struct roar_stream * s, int mode, struct roar_meta * meta) {
 struct roar_message m;
 int len;

 m.cmd     = ROAR_CMD_SET_META;
 m.stream  = s->id;
// m.datalen = len;

 m.data[0] = 0;
 m.data[1] = mode;
 m.data[2] = meta->type;

 m.data[3] = strlen(meta->key);
 m.data[4] = len = strlen(meta->value);

 if ( len > 255 )
  return -1;

 m.datalen = 5 + m.data[3] + m.data[4];
 if ( m.datalen > LIBROAR_BUFFER_MSGDATA )
  return -1;

 strncpy(&(m.data[5]), meta->key, ROAR_META_MAX_NAMELEN);
 strncpy(&(m.data[5+m.data[3]]), meta->value, len);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

//ll
