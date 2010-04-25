//client.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

int roar_client_new      (struct roar_client * client) {
 int i;

 if ( client == NULL )
  return -1;

 memset(client, 0, sizeof(struct roar_client));

 client->fh          = -1;
 client->pid         = -1;
 client->uid         = -1;
 client->gid         = -1;
 client->proto       = ROAR_PROTO_ROARAUDIO;
 client->byteorder   = ROAR_BYTEORDER_NETWORK;

 for (i = 0; i < ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT; i++)
  client->streams[i] = -1;

 return 0;
}

int roar_client_set_fh   (struct roar_client * client, int fh) {
 if ( client == NULL )
  return -1;

 client->fh = fh;

 return 0;
}

int roar_client_set_proto(struct roar_client * client, int proto, int byteorder) {
 if ( client == NULL )
  return -1;

 client->proto = proto;

 if ( byteorder != -1 ) {
  client->byteorder = byteorder;
 } else {
  switch (proto) {
   case ROAR_PROTO_ROARAUDIO:
     client->byteorder = ROAR_BYTEORDER_NETWORK;
    break;
   default:
     return -1;
    break;
  }
 }

 return 0;
}

int roar_client_pass     (struct roar_connection * con, struct roar_client * client, uint16_t flags) {
 struct roar_message m;
 int16_t * d = (int16_t *)m.data;
 int confh;
 int i;

 m.cmd     = ROAR_CMD_PASSFH;
 m.stream  =  -1; // client (non-stream) passs
 m.pos     =   0;
 m.datalen = 4*2;

 d[0] = 0; // version
 d[1] = flags;
 d[2] = client->proto;
 d[3] = client->byteorder;

 for (i = 0; i < 4; i++)
  d[i] = ROAR_HOST2NET16(d[i]);

 if ( (confh = roar_get_connection_fh(con)) == -1 )
  return -1;

 if ( roar_send_message(con, &m, NULL) == -1 ) {
  return -1;
 }

 if ( roar_socket_send_fh(confh, client->fh, NULL, 0) == -1 )
  return -1;

 if ( roar_recv_message(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;

 return -1;
}

//ll
