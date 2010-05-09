//roardmx.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

#ifndef _LIBROARLIGHT_ROARDMX_H_
#define _LIBROARLIGHT_ROARDMX_H_

#include "libroarlight.h"

#define ROAR_ROARDMX_VERSION        0x00

#define ROAR_ROARDMX_MASK_FLAGS     0xF0
#define ROAR_ROARDMX_MASK_TYPE      0x0F

#define ROAR_ROARDMX_DATA_LENGTH    ((1<<(sizeof(char)*8))-1) /* 255 */

// we have a offset of one nibble, see ROAR_ROARDMX_MASK_FLAGS
#define ROAR_ROARDMX_FLAG_PACKED    0x10

#define ROAR_ROARDMX_TYPE_SSET      0x00 /* simple set */
#define ROAR_ROARDMX_TYPE_IPO1      0x01 /* linear interploation */
#define ROAR_ROARDMX_TYPE_IPOLIN    ROAR_ROARDMX_TYPE_IPO1
#define ROAR_ROARDMX_TYPE_IPO4      0x02 /* poly5 interpolation (a*t^4 + b*t^3 + c*t^2 + d*t + e) */
#define ROAR_ROARDMX_TYPE_INC8S     0x03 /* signed 8 bit increment */

struct roar_roardmx_message {
 unsigned char version;
 unsigned char flags;
 unsigned char type;
 size_t        length;
 unsigned char data[3 /* header */ + ROAR_ROARDMX_DATA_LENGTH /* data */];
};

// generic things:
int roar_roardmx_message_new (struct roar_roardmx_message * mes);

// low level:
//int roar_roardmx_message_set_flag(struct roar_roardmx_message * mes, unsigned char   flag);
//int roar_roardmx_message_set_len (struct roar_roardmx_message * mes, size_t          type);
//int roar_roardmx_message_get_data(struct roar_roardmx_message * mes, unsigned char ** data);

// mdium level:
int roar_roardmx_message_set_type(struct roar_roardmx_message * mes, unsigned char   type);
int roar_roardmx_message_get_flag(struct roar_roardmx_message * mes, unsigned char * flag);
int roar_roardmx_message_get_type(struct roar_roardmx_message * mes, unsigned char * length);
int roar_roardmx_message_get_len (struct roar_roardmx_message * mes, size_t        * type);


// IO:
int roar_roardmx_message_send(struct roar_roardmx_message * mes, struct roar_vio_calls * vio);
int roar_roardmx_message_recv(struct roar_roardmx_message * mes, struct roar_vio_calls * vio);

// Data/high level:
// * SSET:
int roar_roardmx_message_new_sset   (struct roar_roardmx_message * mes);
int roar_roardmx_message_add_chanval(struct roar_roardmx_message * mes, uint16_t   channel, unsigned char   val);
int roar_roardmx_message_get_chanval(struct roar_roardmx_message * mes, uint16_t * channel, unsigned char * val, int index);
int roar_roardmx_message_numchannels(struct roar_roardmx_message * mes);


#endif

//ll
