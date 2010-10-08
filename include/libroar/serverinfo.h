//serverinfo.h:

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

#ifndef _LIBROARSERVERINFO_H_
#define _LIBROARSERVERINFO_H_

#include "libroar.h"

struct roar_server_info {
 char * version;
 char * location;
 char * description;
 char * contact;
 char * serial;
 char * address;
 char * uiurl;
 struct {
  char * sysname;
  char * release;
  char * nodename;
  char * machine;
 } un;
};

struct roar_server_info * roar_server_info(struct roar_connection * con);
int roar_server_info_free(struct roar_server_info * info);

int roar_server_info_to_mes(struct roar_message * mes, struct roar_server_info * info);
struct roar_server_info * roar_server_info_from_mes(struct roar_message * mes);

#endif

//ll
