//serverinfo.c:

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

struct ie {
 int type;
 size_t len;
 char * buf;
};

struct roar_server_info * roar_server_info(struct roar_connection * con);

int roar_server_info_free(struct roar_server_info * info) {
 if ( info == NULL )
  return -1;

 roar_mm_free(info);

 return 0;
}

#define _add(t, m) do { if ( (sl = strlen(info->m)) != 0 ) { iebuf[idx].type = (t); iebuf[idx].len = sl; iebuf[idx].buf = (info->m); idx++; needlen += 4 + sl; } } while (0)

int roar_server_info_to_mes(struct roar_message * mes, struct roar_server_info * info) {
 size_t needlen = 4;
 size_t sl;
 int idx = 0;
 struct ie iebuf[sizeof(struct roar_server_info)/sizeof(char*)];
 uint16_t * d16;

 if ( mes == NULL || info == NULL )
  return -1;

 _add(ROAR_ITST_VERSION, version);
 _add(ROAR_ITST_LOCATION, location);
 _add(ROAR_ITST_DESCRIPTION, description);
 _add(ROAR_ITST_CONTACT, contact);
 _add(ROAR_ITST_SERIAL, serial);
 _add(ROAR_ITST_ADDRESS, address);
 _add(ROAR_ITST_UIURL, uiurl);
 _add(ROAR_ITST_UN_SYSNAME, un.sysname);
 _add(ROAR_ITST_UN_RELEASE, un.release);
 _add(ROAR_ITST_UN_NODENAME, un.nodename);
 _add(ROAR_ITST_UN_MACHINE, un.machine);

 if ( needlen > LIBROAR_BUFFER_MSGDATA ) {
  return -1;
 }

 memset(mes, 0, sizeof(struct roar_message));

 mes->datalen = needlen;

 d16 = (uint16_t*)mes->data;

 return -1;
}

struct roar_server_info * roar_server_info_from_mes(struct roar_message * mes);


//ll
