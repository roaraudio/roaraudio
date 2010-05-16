//basic.h:

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

#ifndef _LIBROARBASIC_H_
#define _LIBROARBASIC_H_

#include "libroar.h"

#define LIBROAR_BUFFER_SMALL   80
#define LIBROAR_BUFFER_MSGDATA LIBROAR_BUFFER_SMALL
#define _ROAR_MESSAGE_VERSION 0

#if ROAR_MAX_CHANNELS > (LIBROAR_BUFFER_SMALL - 10)
#error ROAR_MAX_CHANNELS too large change ROAR_MAX_CHANNELS or LIBROAR_BUFFER_SMALL
#endif

struct roar_message {
 int cmd;
 unsigned int stream;
 uint32_t pos;
 int datalen;
 char data[LIBROAR_BUFFER_MSGDATA];
 int16_t seq;
 int32_t flags;
 int32_t crc;
};

struct roar_connection {
 int __fh;
 int32_t flags;
 struct roar_vio_calls viocon;
 struct roar_buffer * msgqueue;
};


int roar_connect_raw  (char * server);

int roar_connect      (struct roar_connection * con, char * server);
int roar_connect_fh   (struct roar_connection * con, int fh);
int roar_get_connection_fh  (struct roar_connection * con);
int roar_get_connection_vio (struct roar_connection * con, struct roar_vio_calls * vio);
int roar_disconnect   (struct roar_connection * con);

int roar_noop         (struct roar_connection * con);

int roar_identify     (struct roar_connection * con, char * name);

int roar_send_message (struct roar_connection * con, struct roar_message * mes, char *  data);
int roar_recv_message (struct roar_connection * con, struct roar_message * mes, char ** data);
int roar_req          (struct roar_connection * con, struct roar_message * mes, char ** data);

int roar_vsend_message(struct roar_vio_calls  * vio, struct roar_message * mes, char *  data);
int roar_vrecv_message(struct roar_vio_calls  * vio, struct roar_message * mes, char ** data);
int roar_vreq         (struct roar_vio_calls  * vio, struct roar_message * mes, char ** data);

int roar_debug_message_print    (struct roar_message * mes);
int roar_debug_audio_info_print (struct roar_audio_info * info);

#endif

//ll
