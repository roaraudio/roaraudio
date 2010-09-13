//asyncctl.c:

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

int roar_wait (struct roar_connection * con, struct roar_event * triggered, struct roar_event * events, size_t num) {
 struct roar_message m;
 uint16_t * u16 = (uint16_t *) m.data;
 void * vp = m.data;
 size_t i, len_left, len_have, tmp;

 if ( con == NULL || triggered == NULL )
  return -1;

 if ( num == 0 )
  return 0;

 if ( events == NULL )
  return -1;

 memset(&m, 0, sizeof(m));

 m.cmd = ROAR_CMD_WAIT;
 m.stream = -1;

 u16[0] = ROAR_HOST2NET16(0); // version
 u16[1] = ROAR_HOST2NET16(0); // flags

 vp += 4; // we have 4 byte header (version + flags)

 len_left = sizeof(m.data) - 4;
 len_have = 4;

 for (i = 0; i < num; i++) {
  tmp = len_left;
  if ( roar_event_to_blob(&(events[i]), vp, &tmp) == -1 )
   return -1;

  len_left -= tmp;
  len_have += tmp;
  vp       += tmp;
 }

 m.datalen = len_have;

 if ( roar_req(con, &m, NULL) != 0 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 u16[0] = ROAR_NET2HOST16(u16[0]);
 u16[1] = ROAR_NET2HOST16(u16[1]);

 // check if we have a complet header:
 if ( m.datalen < 2 )
  return -1;

 // check if we have a unsupported version or flags:
 if ( u16[0] != 0 || u16[1] != 0 )
  return -1;

 vp  = (void*)m.data;
 vp += 4;
 len_have = tmp = m.datalen - 4;

 memset(triggered, 0, sizeof(struct roar_event));

 if ( roar_event_from_blob(triggered, vp, &tmp) != 0 )
  return -1;

 // test if we have a length match:
 if ( len_have != tmp )
  return -1;

 // we now have an event.

 // we maybe have some data, need to reset this as it is of a local buffer:
 triggered->arg2 = NULL;
 triggered->arg2_len = 0;

 return 0;
}

//ll
