//error.c:

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

int    roar_err_int(struct roar_error_frame * frame) {
 if ( frame == NULL )
  return -1;

 memset(frame, 0, sizeof(struct roar_error_frame));

 frame->cmd         = -1;
 frame->ra_errno    = -1;
 frame->ra_suberrno = -1;
 frame->p_errno     = -1;

 return 0;
}

void * roar_err_buildmsg(struct roar_message * mes, struct roar_error_frame * frame) {
 int16_t * d;

 if ( mes == NULL || frame == NULL )
  return NULL;

 memset(mes,  0, sizeof(struct roar_message));

 d = (int16_t*)mes->data;

 mes->datalen = 8 + frame->datalen;
 frame->data  = &(mes->data[8]);

 mes->data[0]    = 0; // version.
 mes->data[1]    = frame->cmd;
 mes->data[2]    = frame->ra_errno;
 mes->data[3]    = frame->ra_suberrno;
 d[2]            = ROAR_HOST2NET16(frame->p_errno);
 d[3]            = ROAR_HOST2NET16(frame->flags);

 return frame->data;
}

int    roar_err_parsemsg(struct roar_message * mes, struct roar_error_frame * frame) {
 int16_t * d;

 if ( mes == NULL || frame == NULL )
  return -1;

 d = (int16_t*)mes->data;

 if ( mes->datalen < 8 )
  return -1;

 if ( mes->data[0] != 0 )
  return -1;

 frame->cmd         = mes->data[1];
 frame->ra_errno    = mes->data[2];
 frame->ra_suberrno = mes->data[3];
 frame->p_errno     = ROAR_NET2HOST16(d[2]);
 frame->flags       = ROAR_NET2HOST16(d[3]);

 frame->datalen     = mes->datalen - 8;
 frame->data        = &(mes->data[8]);

 return 0;
}

//ll
