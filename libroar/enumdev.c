//enumdev.c:

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

// TODO: we should put all the data in one big alloced block.

struct locmed {
 int supflags;
 ssize_t (*func)(int flags, int dir, int socktype, char ** servers, size_t maxlen);
};

static struct locmed _libroar_locmod[] = {
};

struct roar_server * roar_enum_servers(int flags, int dir, int socktype) {
 struct roar_server * ret = NULL;
 struct roar_server * c;
 char * servers[64];
 size_t have = 1;
 size_t i;
 ssize_t r;
 int testflags = flags;

 if ( testflags & ROAR_ENUM_FLAG_DESC )
  testflags -= ROAR_ENUM_FLAG_DESC;
 if ( testflags & ROAR_ENUM_FLAG_LOCATION )
  testflags -= ROAR_ENUM_FLAG_LOCATION;

 for (i = 0; i < sizeof(_libroar_locmod)/sizeof(*_libroar_locmod); i++) {
  if ( (_libroar_locmod[i].supflags & testflags) == testflags ) {
   r = _libroar_locmod[i].func(flags, dir, socktype, &(servers[have-1]), (sizeof(servers)/sizeof(*servers)) - have);
   if ( r > 0 )
    have += r;
  }
 }

 ret = roar_mm_malloc(have*sizeof(struct roar_server));

 if (ret == NULL)
  return NULL;

 have--;

 for (i = 0; i < have; i++) {
  c = &(ret[i]);
  c->server = servers[i];
  c->description = NULL;
  c->location = NULL;
 }

 ret[have].server = NULL;
 ret[have].description = roar_mm_strdup("Default server");
 ret[have].location = NULL;

 return ret;
}

int roar_enum_servers_free(struct roar_server * servs) {
 struct roar_server * c;
 int i;

 if ( servs == NULL )
  return -1;

 for (i = 0; (c = &(servs[i]))->server != NULL; i++) {
  roar_mm_free((void*)c->server);
  if ( c->description != NULL )
   roar_mm_free((void*)c->description);
  if ( c->location != NULL )
   roar_mm_free((void*)c->location);
 }

 if ( c->description != NULL )
  roar_mm_free((void*)c->description);
 if ( c->location != NULL )
  roar_mm_free((void*)c->location);

 roar_mm_free(servs);

 return 0;
}

ssize_t roar_enum_servers_num(struct roar_server * servs) {
 size_t ret;

 if ( servs == NULL )
  return -1;

 for (ret = 0; servs[ret].server != NULL; ret++);

 return ret;
}

//ll
