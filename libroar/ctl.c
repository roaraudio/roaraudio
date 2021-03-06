//ctl.c:

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

int roar_get_clientid  (struct roar_connection * con) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd     = ROAR_CMD_WHOAMI;
 mes.datalen = 0;

 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 if ( mes.datalen != 1 )
  return -1;

 return mes.data[0];
}

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
 return roar_terminate(con, 0);
}

int roar_terminate (struct roar_connection * con, int terminate) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd     = ROAR_CMD_EXIT;
 mes.datalen = 1;
 mes.data[0] = terminate;

 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

int roar_list_filtered(struct roar_connection * con, int * items,   int max, int cmd, unsigned char filter, unsigned char cmp, uint32_t id) {
 struct roar_message m;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 roar_ctl_f2m(&m, filter, cmp, id);
 m.cmd = cmd;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 return roar_ctl_m2ia(&m, items, max);
}

int roar_list         (struct roar_connection * con, int * items,   int max, int cmd) {
 return roar_list_filtered(con, items, max, cmd, ROAR_CTL_FILTER_ANY, ROAR_CTL_CMP_ANY, ROAR_CTL_FILTER_ANY);
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

 roar_errno = ROAR_ERROR_UNKNOWN;

 if ( roar_req(con, &m, NULL) == -1 ) {
  roar_errno = ROAR_ERROR_PROTO;
  return -1;
 }

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
 m.stream  = id;
 info[0] = ROAR_HOST2NET16(1);
 info[1] = ROAR_HOST2NET16(mixer->scale);
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
 m.stream  = id;

 info[0]   = ROAR_HOST2NET16(1);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 if ( ROAR_NET2HOST16(info[0]) != 1 )
  return -1;

 info[1] = ROAR_NET2HOST16(info[1]);

 if ( channels != NULL )
  *channels = info[1];

 if ( info[1] > ROAR_MAX_CHANNELS )
  return -1;

 mixer->scale   = ROAR_NET2HOST16(info[2]);
 mixer->rpg_mul = ROAR_NET2HOST16(info[3]);
 mixer->rpg_div = ROAR_NET2HOST16(info[4]);

 for (i = 0; i < info[1]; i++)
  mixer->mixer[i] = ROAR_NET2HOST16(info[i+5]);

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
  ROAR_ERR("roar_ctl_m2f(*): version %i not supported!", (int)m->data[0]);
  return -1;
 }

 *filter = m->data[1];
 *cmp    = m->data[2];

 *id = ROAR_NET2HOST32(*((uint32_t*)&(m->data[3])));

 return 0;
}

int roar_filter_match (const unsigned cmp, const uint32_t a, const uint32_t b) {
 switch (cmp) {
  case ROAR_CTL_CMP_ANY:
    return 1;
   break;
  case ROAR_CTL_CMP_EQ:
    return a == b;
   break;
  case ROAR_CTL_CMP_NE:
    return a != b;
   break;
  default:
    return -1;
 }
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
 size_t len_rest;

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

 pid = ROAR_HOST2NET32(c->proto);
 memcpy(&(m->data[cur]), &pid, 4);
 cur += 4;

 pid = ROAR_HOST2NET32(c->byteorder);
 memcpy(&(m->data[cur]), &pid, 4);
 cur += 4;

 len_rest = sizeof(m->data) - cur;
 if ( roar_nnode_to_blob(&(c->nnode), &(m->data[cur]), &len_rest) == 0 ) {
  cur += len_rest;
 }

 m->datalen = cur;

 return 0;
}

int roar_ctl_m2c      (struct roar_message * m, struct roar_client * c) {
 int i;
 int cur;
 uint32_t pid;
 size_t len;

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

 if ( m->datalen >= cur+4 ) {
  memcpy(&pid, &(m->data[cur]), 4);
  c->proto = ROAR_NET2HOST32(pid);
  cur += 4;
 } else {
  c->proto = ROAR_PROTO_NONE;
 }

 if ( m->datalen >= cur+4 ) {
  memcpy(&pid, &(m->data[cur]), 4);
  c->byteorder = ROAR_NET2HOST32(pid);
  cur += 4;
 } else {
  c->byteorder = ROAR_BYTEORDER_UNKNOWN;
 }

 if ( m->datalen > cur ) {
  len = m->datalen - cur;
  if ( roar_nnode_from_blob(&(c->nnode), &(m->data[cur]), &len) == 0 ) {
   cur += len;
  } else {
   if ( roar_nnode_new(&(c->nnode), ROAR_SOCKET_TYPE_UNKNOWN) == -1 )
    return -1;
  }
 } else {
  if ( roar_nnode_new(&(c->nnode), ROAR_SOCKET_TYPE_UNKNOWN) == -1 )
   return -1;
 }

 return 0;
}

int    roar_str2proto (char * proto) {
 if ( !strcasecmp(proto, "roar") ) {
  return ROAR_PROTO_ROARAUDIO;
 } else if ( !strcasecmp(proto, "roaraudio") ) {
  return ROAR_PROTO_ROARAUDIO;
 } else if ( !strcasecmp(proto, "esd") ) {
  return ROAR_PROTO_ESOUND;
 } else if ( !strcasecmp(proto, "esound") ) {
  return ROAR_PROTO_ESOUND;
 } else if ( !strcasecmp(proto, "auto") ) {
  return ROAR_PROTO_AUTO;
 } else if ( !strcasecmp(proto, "(auto)") ) {
  return ROAR_PROTO_AUTO;
 } else if ( !strcasecmp(proto, "http") ) {
  return ROAR_PROTO_HTTP;
 } else if ( !strcasecmp(proto, "gopher") ) {
  return ROAR_PROTO_GOPHER;
 } else if ( !strcasecmp(proto, "icy") ) {
  return ROAR_PROTO_ICY;
 } else if ( !strcasecmp(proto, "simple") ) {
  return ROAR_PROTO_SIMPLE;
 } else if ( !strcasecmp(proto, "rsound") ) {
  return ROAR_PROTO_RSOUND;
 } else if ( !strcasecmp(proto, "rplay") ) {
  return ROAR_PROTO_RPLAY;
 }

 return -1;
}

char * roar_proto2str (int    proto) {
 switch (proto) {
  case ROAR_PROTO_ROARAUDIO: return "RoarAudio"; break;
  case ROAR_PROTO_ESOUND:    return "EsounD";    break;
  case ROAR_PROTO_AUTO:      return "(auto)";    break;
  case ROAR_PROTO_HTTP:      return "http";      break;
  case ROAR_PROTO_GOPHER:    return "gopher";    break;
  case ROAR_PROTO_ICY:       return "ICY";       break;
  case ROAR_PROTO_SIMPLE:    return "Simple";    break;
  case ROAR_PROTO_RSOUND:    return "RSound";    break;
  case ROAR_PROTO_RPLAY:     return "RPlay";     break;
  default:
    return "(unknown)";
 }
}

int    roar_str2byteorder (char * byteorder) {
 if (        !strcasecmp(byteorder, "le")            || !strcasecmp(byteorder, "little") ||
             !strcasecmp(byteorder, "little endian") || !strcasecmp(byteorder, "1234")   ) {
  return ROAR_BYTEORDER_LE;
 } else if ( !strcasecmp(byteorder, "be")            || !strcasecmp(byteorder, "big")    ||
             !strcasecmp(byteorder, "big endian")    || !strcasecmp(byteorder, "4321")   ) {
  return ROAR_BYTEORDER_BE;
 } else if ( !strcasecmp(byteorder, "pdp")           ||
             !strcasecmp(byteorder, "pdp endian") ) {
  return ROAR_BYTEORDER_PDP;
 } else if ( !strcasecmp(byteorder, "network")       ||
             !strcasecmp(byteorder, "network byteorder") ) {
  return ROAR_BYTEORDER_NETWORK;
 }

 return -1;
}

char * roar_byteorder2str (int    byteorder) {
 switch (byteorder) {
  case ROAR_BYTEORDER_LE:      return "little endian"; break;
  case ROAR_BYTEORDER_BE:      return "big endian";    break;
  case ROAR_BYTEORDER_PDP:     return "pdp endian";    break;
//  case ROAR_BYTEORDER_NETWORK: return "network";       break;
  default:
    return "(unknown)";
 }
}

// grep '^#define ROAR_OT_' roaraudio/proto.h | cut -d' ' -f2 | sed 's/^\(ROAR_OT_\)\(.*\)$/ {\1\2, "\2"},/'
static struct {
 const int ot;
 const char * name;
} _libroar_ot[] = {
 {ROAR_OT_CLIENT,   "client"},
 {ROAR_OT_STREAM,   "stream"},
 {ROAR_OT_SOURCE,   "source"},
 {ROAR_OT_SAMPLE,   "sample"},
 {ROAR_OT_OUTPUT,   "output"},
 {ROAR_OT_MIXER,    "mixer"},
 {ROAR_OT_BRIDGE,   "bridge"},
 {ROAR_OT_LISTEN,   "listen"},
 {ROAR_OT_ACTION,   "action"},
 {ROAR_OT_MSGQUEUE, "msgqueue"},
 {ROAR_OT_MSGBUS,   "msgbus"},
 {-1, NULL}
};

int    roar_str2ot        (const char * ot) {
 int i;

 for (i = 0; _libroar_ot[i].ot != -1; i++)
  if ( !strcasecmp(ot, _libroar_ot[i].name) )
   return _libroar_ot[i].ot;

 return -1;
}

const char * roar_ot2str  (const int    ot) {
 int i;

 for (i = 0; _libroar_ot[i].ot != -1; i++)
  if ( _libroar_ot[i].ot == ot )
   return _libroar_ot[i].name;

 return NULL;
}

int roar_conv_volume (struct roar_mixer_settings * dst, struct roar_mixer_settings * src, int dstchans, int srcchans) {
 struct roar_mixer_settings lsrc;
 int i;
 uint_least32_t s;

 if ( dst == NULL || src == NULL || dstchans < 0 || srcchans < 0 )
  return -1;

 if ( dstchans == srcchans ) {
  if ( dst == src )
   return 0;

  memcpy(dst, src, sizeof(struct roar_mixer_settings));
  return 0;
 }

 if ( dst == src ) {
  memcpy(&lsrc, src, sizeof(lsrc));
  src = &lsrc;
 }

 // sepcal handling for N->1:
 if ( dstchans == 1 ) {
  s = 0;

  for (i = 0; i < srcchans; i++)
   s += src->mixer[i];

  dst->mixer[0] = s / srcchans;
 } else {
  switch (srcchans) {
   case  1:
     for (i = 0; i < dstchans; i++)
      dst->mixer[i] = src->mixer[0];
    break;
   case 2:
     switch (dstchans) {
      case 3:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = (src->mixer[0] + src->mixer[1]) / 2;
       break;
      case 4:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[0];
        dst->mixer[3] = src->mixer[1];
       break;
      case 5:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = (src->mixer[0] + src->mixer[1]) / 2;
        dst->mixer[3] = src->mixer[0];
        dst->mixer[4] = src->mixer[1];
       break;
      case 6:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = (src->mixer[0] + src->mixer[1]) / 2;
        dst->mixer[3] = dst->mixer[2];
        dst->mixer[4] = src->mixer[0];
        dst->mixer[5] = src->mixer[1];
       break;
      default:
        return -1;
       break;
     }
    break;
   case 3:
     switch (dstchans) {
      case 2:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
       break;
      case 4:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[0];
        dst->mixer[3] = src->mixer[1];
       break;
      case 5:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[2];
        dst->mixer[3] = src->mixer[0];
        dst->mixer[4] = src->mixer[1];
       break;
      case 6:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[2];
        dst->mixer[3] = src->mixer[2];
        dst->mixer[4] = src->mixer[0];
        dst->mixer[5] = src->mixer[1];
       break;
      default:
        return -1;
       break;
     }
    break;
   case 4:
     switch (dstchans) {
      case 2:
        dst->mixer[0] = (src->mixer[0] + src->mixer[2]) / 2;
        dst->mixer[1] = (src->mixer[1] + src->mixer[3]) / 2;
       break;
      case 3:
        dst->mixer[0] = (src->mixer[0] + src->mixer[2]) / 2;
        dst->mixer[1] = (src->mixer[1] + src->mixer[3]) / 2;
        dst->mixer[2] = (dst->mixer[0] + dst->mixer[1]) / 2;
       break;
      case 5:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = (src->mixer[0] + src->mixer[2] + src->mixer[1] + src->mixer[3]) / 4;
        dst->mixer[3] = src->mixer[2];
        dst->mixer[4] = src->mixer[3];
       break;
      case 6:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = (src->mixer[0] + src->mixer[2] + src->mixer[1] + src->mixer[3]) / 4;
        dst->mixer[3] = dst->mixer[2];
        dst->mixer[4] = src->mixer[2];
        dst->mixer[5] = src->mixer[3];
       break;
      default:
        return -1;
       break;
     }
    break;
   case 5:
     switch (dstchans) {
      case 2:
        dst->mixer[0] = (src->mixer[0] + src->mixer[3]) / 2;
        dst->mixer[1] = (src->mixer[1] + src->mixer[4]) / 2;
       break;
      case 3:
        dst->mixer[0] = (src->mixer[0] + src->mixer[3]) / 2;
        dst->mixer[1] = (src->mixer[1] + src->mixer[4]) / 2;
        dst->mixer[2] = src->mixer[2];
       break;
      case 4:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[3];
        dst->mixer[3] = src->mixer[4];
       break;
      case 6:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[2];
        dst->mixer[3] = src->mixer[2];
        dst->mixer[4] = src->mixer[3];
        dst->mixer[5] = src->mixer[4];
       break;
      default:
        return -1;
       break;
     }
    break;
   case 6:
     switch (dstchans) {
      case 2:
        dst->mixer[0] = (src->mixer[0] + src->mixer[4]) / 2;
        dst->mixer[1] = (src->mixer[1] + src->mixer[5]) / 2;
       break;
      case 3:
        dst->mixer[0] = (src->mixer[0] + src->mixer[4]) / 2;
        dst->mixer[1] = (src->mixer[1] + src->mixer[5]) / 2;
        dst->mixer[2] = src->mixer[2];
       break;
      case 4:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[4];
        dst->mixer[3] = src->mixer[5];
       break;
      case 5:
        dst->mixer[0] = src->mixer[0];
        dst->mixer[1] = src->mixer[1];
        dst->mixer[2] = src->mixer[2];
        dst->mixer[3] = src->mixer[4];
        dst->mixer[4] = src->mixer[5];
       break;
      default:
        return -1;
       break;
     }
    break;
   default:
     return -1;
    break;
  }
 }

 dst->scale   = src->scale;
 dst->rpg_mul = src->rpg_mul;
 dst->rpg_div = src->rpg_div;

 return 0;
}

//ll
