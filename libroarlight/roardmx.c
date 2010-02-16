//roardmx.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroarlight.h"

// base(ic) check
#define BCHK(x) if ( (x) == NULL ) return -1

int roar_roardmx_message_new (struct roar_roardmx_message * mes) {
 BCHK(mes);

 memset(mes, 0, sizeof(struct roar_roardmx_message));

 mes->version = ROAR_ROARDMX_VERSION;

 return 0;
}

// low level:
//int roar_roardmx_message_set_flag(struct roar_roardmx_message * mes, unsigned char   flag);
//int roar_roardmx_message_set_len (struct roar_roardmx_message * mes, size_t          type);
//int roar_roardmx_message_get_data(struct roar_roardmx_message * mes, unsigned char ** data);

// mdium level:
int roar_roardmx_message_set_type(struct roar_roardmx_message * mes, unsigned char   type) {
 BCHK(mes);

 if ( (type | ROAR_ROARDMX_MASK_TYPE) - ROAR_ROARDMX_MASK_TYPE )
  return -1;

 mes->type = type;

 return 0;
}

int roar_roardmx_message_get_flag(struct roar_roardmx_message * mes, unsigned char * flag) {
 BCHK(mes);

 *flag = mes->flags;

 return 0;
}

int roar_roardmx_message_get_type(struct roar_roardmx_message * mes, unsigned char * type) {
 BCHK(mes);

 *type = mes->type;

 return 0;
}

int roar_roardmx_message_get_len (struct roar_roardmx_message * mes, size_t        * length) {
 BCHK(mes);

 *length = mes->length;

 return 0;
}


// IO:
int roar_roardmx_message_send(struct roar_roardmx_message * mes, struct roar_vio_calls * vio) {
 BCHK(mes);
 BCHK(vio);

 if ( mes->length > ROAR_ROARDMX_DATA_LENGTH ) // this is very fatal!
  return -1;

 mes->data[0] =  mes->version;
 mes->data[1] = (mes->flags & ROAR_ROARDMX_MASK_FLAGS) |
                (mes->type  & ROAR_ROARDMX_MASK_TYPE ) ;

 mes->data[2] = mes->length;

 return roar_vio_write(vio, mes->data, mes->length + 3) == (mes->length + 3) ? 0 : -1;
}

int roar_roardmx_message_recv(struct roar_roardmx_message * mes, struct roar_vio_calls * vio) {
 BCHK(mes);
 BCHK(vio);

 if ( roar_roardmx_message_new(mes) == -1 )
  return -1;

 if ( roar_vio_read(vio, mes->data, 3) != 3 )
  return -1;

 mes->version = mes->data[0];

 if ( mes->version != 0 )
  return -1;

 mes->flags  = mes->data[1] & ROAR_ROARDMX_MASK_FLAGS;
 mes->type   = mes->data[1] & ROAR_ROARDMX_MASK_TYPE;

 mes->length = mes->data[2];

 if ( roar_vio_read(vio, &(mes->data[3]), mes->length) != mes->length )
  return -1;

 return 0;
}

// Data/high level:
// * SSET:
int roar_roardmx_message_new_sset   (struct roar_roardmx_message * mes) {
 if ( roar_roardmx_message_new(mes) == -1 )
  return -1;

 mes->type = ROAR_ROARDMX_TYPE_SSET;

 return 0;
}

int roar_roardmx_message_add_chanval(struct roar_roardmx_message * mes, uint16_t channel, unsigned char val) {
 register uint16_t * chan;

 BCHK(mes);

 if ( (mes->length + 3) > ROAR_ROARDMX_DATA_LENGTH ) // message would be to long
  return -1;

 chan = (uint16_t *) &(mes->data[mes->length + 3]);

 *chan = ROAR_HOST2NET16(channel);

 mes->data[mes->length + 2 + 3] = val;

 mes->length += 3;

 return 0;
}

int roar_roardmx_message_get_chanval(struct roar_roardmx_message * mes, uint16_t * channel, unsigned char * val, int index) {
 register uint16_t * chan;

 BCHK(mes);

 if ( index < 0 )
  return -1;

 if ( mes->version != 0 )
  return -1;

 switch (mes->type) {
  case ROAR_ROARDMX_TYPE_SSET:
  case ROAR_ROARDMX_TYPE_INC8S:
    if ( index >= (ROAR_ROARDMX_DATA_LENGTH/3) )
     return -1;

    *val     = mes->data[3 * index + 2 + 3];
    chan     = (uint16_t *) &(mes->data[3 + 3 * index]);
    *channel = ROAR_NET2HOST16(*chan);
    return 0;
   break;
 }

 return -1;
}

int roar_roardmx_message_numchannels(struct roar_roardmx_message * mes) {
 BCHK(mes);

 if ( mes->version != 0 )
  return -1;

 switch (mes->type) {
  case ROAR_ROARDMX_TYPE_SSET:
  case ROAR_ROARDMX_TYPE_INC8S:
    return mes->length /  3;
   break;
  case ROAR_ROARDMX_TYPE_IPO1:
    return mes->length /  6;
   break;
  case ROAR_ROARDMX_TYPE_IPO4:
    return mes->length / 12;
   break;
 }

 return -1;
}

//ll
