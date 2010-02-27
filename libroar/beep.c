//beep.c:

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

int roar_beep(struct roar_connection * con, const struct roar_beep * beep) {
 struct roar_message m;
 int16_t * data = (int16_t*)m.data;

 if ( con == NULL )
  return -1;

 memset(&m, 0, sizeof(m));

 m.cmd = ROAR_CMD_BEEP;

 if ( beep == NULL ) {
  m.datalen = 0;
 } else {
  m.datalen = 8*2;
  data[0]   = ROAR_HOST2NET16(0); // version
  data[1]   = ROAR_HOST2NET16(beep->vol);
  data[2]   = ROAR_HOST2NET16(beep->time);
  data[3]   = ROAR_HOST2NET16(beep->freq);
  data[4]   = ROAR_HOST2NET16(beep->type);
  data[5]   = ROAR_HOST2NET16(beep->x);
  data[6]   = ROAR_HOST2NET16(beep->y);
  data[7]   = ROAR_HOST2NET16(beep->z);
 }

 if ( roar_req(con, &m, NULL) != 0 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 return m.stream;
}

//ll
