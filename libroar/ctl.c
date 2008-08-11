//ctl.c:

#include "libroar.h"

int roar_server_oinfo   (struct roar_connection * con, struct roar_stream * sa) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd     = ROAR_CMD_SERVER_OINFO;
 mes.datalen = 0;

 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 if ( roar_stream_m2s(sa, &mes) == -1 )
  return -1;

 return 0;
}

int roar_get_standby   (struct roar_connection * con) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd = ROAR_CMD_GET_STANDBY;
 mes.datalen = 0;

 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 return ROAR_NET2HOST16(*((uint16_t*)mes.data));
}

int roar_set_standby   (struct roar_connection * con, int state) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd = ROAR_CMD_SET_STANDBY;
 mes.datalen = 2;

 *((uint16_t*)mes.data) = ROAR_HOST2NET16((unsigned) state);

 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

int roar_exit   (struct roar_connection * con) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd = ROAR_CMD_EXIT;
 mes.datalen = 0;
 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

int roar_list         (struct roar_connection * con, int * items,   int max, int cmd) {
 struct roar_message m;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 roar_ctl_f2m_any(&m);
 m.cmd = cmd;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 return roar_ctl_m2ia(&m, items, max);
}

int roar_get_client   (struct roar_connection * con, struct roar_client * client, int id) {
 struct roar_message m;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_GET_CLIENT;
 m.datalen = 1;
 m.data[0] = id;

 ROAR_DBG("roar_get_client(*): id = %i", id);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 ROAR_DBG("roar_get_client(*): got ok");

 return roar_ctl_m2c(&m, client);
}

int roar_get_stream   (struct roar_connection * con, struct roar_stream * stream, int id) {
 struct roar_message m;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_GET_STREAM;
 m.datalen = 1;
 m.data[0] = id;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 return roar_stream_m2s(stream, &m);
}

int roar_kick         (struct roar_connection * con, int type, int id) {
 struct roar_message m;
 uint16_t * info = (uint16_t *) m.data;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_KICK;
 m.datalen = 4;
 info[0] = ROAR_HOST2NET16(type);
 info[1] = ROAR_HOST2NET16(id);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

int roar_set_vol      (struct roar_connection * con, int id, struct roar_mixer_settings * mixer, int channels) {
 struct roar_message m;
 uint16_t * info = (uint16_t *) m.data;
 int i;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_SET_VOL;
 m.datalen = (3 + channels) * 2;
 info[0] = 0;
 info[1] = ROAR_HOST2NET16(id);
 info[2] = ROAR_HOST2NET16(ROAR_SET_VOL_ALL);

 for (i = 0; i < channels; i++)
  info[i+3] = ROAR_HOST2NET16(mixer->mixer[i]);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

int roar_get_vol      (struct roar_connection * con, int id, struct roar_mixer_settings * mixer, int * channels) {
 struct roar_message m;
 uint16_t * info = (uint16_t *) m.data;
 int i;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_GET_VOL;
 m.datalen = 2*2;
 info[0] = 0;
 info[1] = ROAR_HOST2NET16(id);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 if ( info[0] != 0 )
  return -1;

 info[1] = ROAR_NET2HOST16(info[1]);

 if ( channels != NULL )
  *channels = info[1];

 if ( info[1] > ROAR_MAX_CHANNELS )
  return -1;

 for (i = 0; i < info[1]; i++)
  mixer->mixer[i] = ROAR_NET2HOST16(info[i+2]);

 return 0;
}

// converts: *_m2*, *_*2m

int roar_ctl_f2m      (struct roar_message * m, unsigned char   filter, unsigned char   cmp, uint32_t   id) {

 m->datalen = 7;

 m->data[0] = 0;
 m->data[1] = filter;
 m->data[2] = cmp;
 *((uint32_t*)&(m->data[3])) = ROAR_HOST2NET32(id);

 return 0;
}
int roar_ctl_m2f      (struct roar_message * m, unsigned char * filter, unsigned char * cmp, uint32_t * id) {

 if ( m->datalen != 7 )
  return -1;

 if ( m->data[0] != 0 ) {
  ROAR_ERR("roar_ctl_m2f(*): version %i not supported!", m->data[0]);
  return -1;
 }

 *filter = m->data[1];
 *cmp    = m->data[2];

 *id = ROAR_NET2HOST32(*((uint32_t*)&(m->data[3])));

 return 0;
}

int roar_ctl_ia2m     (struct roar_message * m, int * data, int len) {
 int i;

 if ( len > LIBROAR_BUFFER_MSGDATA )
  return -1;

 m->datalen = len;

 for (i = 0; i < len; i++)
  m->data[i] = data[i];

 return 0;
}
int roar_ctl_m2ia     (struct roar_message * m, int * data, int len) {
 int i;

 if ( m->datalen > len )
  return -1;

 for (i = 0; i < m->datalen; i++)
  data[i] = m->data[i];

 return m->datalen;
}

int roar_ctl_c2m      (struct roar_message * m, struct roar_client * c) {
 int cur = 0;
 int h;
 int i;
 int max_len;
 uint32_t pid;

 if ( c == NULL )
  return -1;

 m->data[cur++] = 0;                       // 0: Version
 m->data[cur++] = c->execed;               // 1: execed

 h = 0;
 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++) {
  if ( c->streams[i] != -1 )
   m->data[cur+1+h++] = c->streams[i];
 }

 m->data[cur++] = h;                       // 2: num of streams
 cur += h;

 max_len = strlen(c->name);

 // TODO: add some code to check if this fits in the pkg
 // NOTE: add this code after we are sure how long this pkg will be
 //       and fully decieded about this function.

 m->data[cur++] = max_len;

 strncpy((m->data)+cur, c->name, max_len);

 cur += max_len;

 pid = ROAR_HOST2NET32(c->pid);
 memcpy(&(m->data[cur]), &pid, 4);
 cur += 4;

 pid = ROAR_HOST2NET32(c->uid);
 memcpy(&(m->data[cur]), &pid, 4);
 cur += 4;

 pid = ROAR_HOST2NET32(c->gid);
 memcpy(&(m->data[cur]), &pid, 4);
 cur += 4;

 m->datalen = cur;

 return 0;
}

int roar_ctl_m2c      (struct roar_message * m, struct roar_client * c) {
 int i;
 int cur;
 uint32_t pid;

 if ( m == NULL || c == NULL )
  return -1;

 if ( m->datalen == 0 )
  return -1;

 ROAR_DBG("roar_ctl_m2c(*): got data!, len = %i", m->datalen);

 if ( m->data[0] != 0 ) {
  ROAR_DBG("roar_ctl_m2c(*): wrong version!");
  return -1;
 }

 if ( m->datalen < 3 )
  return -1;

 ROAR_DBG("roar_ctl_m2c(*): have usable data!");

 c->execed = m->data[1];

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++)
  c->streams[i] = -1;

 for (i = 0; i < m->data[2]; i++)
  c->streams[i] = m->data[3+i];

 cur = 3 + m->data[2];

 strncpy(c->name, (m->data)+cur+1, m->data[cur]);
 c->name[(int)m->data[cur]] = 0;

 cur += m->data[cur] + 1;

 memcpy(&pid, &(m->data[cur]), 4);
 c->pid = ROAR_NET2HOST32(pid);
 cur += 4;

 memcpy(&pid, &(m->data[cur]), 4);
 c->uid = ROAR_NET2HOST32(pid);
 cur += 4;

 memcpy(&pid, &(m->data[cur]), 4);
 c->gid = ROAR_NET2HOST32(pid);
 cur += 4;

 return 0;
}

//ll
