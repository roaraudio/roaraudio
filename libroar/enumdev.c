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

static int _test_server(struct roar_server * c, int flags) {
 struct roar_connection con;
 if ( c->server == NULL )
  return -1;

 if ( flags & ROAR_ENUM_FLAG_NONBLOCK )
  return 0;

 if ( roar_connect(&con, (char*)c->server) == -1 )
  return -1;

 roar_disconnect(&con);

 return 0;
}

#define _add(x) if ( (x) != NULL ) servers[ret++] = roar_mm_strdup((x))
static ssize_t _esl_defaults(int flags, int dir, int socktype, char ** servers, size_t maxlen) {
#ifdef ROAR_HAVE_LIBX11
 struct roar_x11_connection * x11con;
#endif
 ssize_t ret = 0;
 const char * new;
 char buf[1024];
 int i;

 if ( maxlen < 10 )
  return -1;

 new = roar_libroar_get_server();
 _add(new);

 new = getenv("ROAR_SERVER");
 _add(new);

#ifdef ROAR_HAVE_LIBX11
 if ( (x11con = roar_x11_connect(NULL)) != NULL ) {
  new = roar_x11_get_prop(x11con, "ROAR_SERVER");
  _add(new);
  roar_x11_disconnect(x11con);
 }
#endif

#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
 if ( (i = readlink("/etc/roarserver", buf, sizeof(buf)-1)) != -1 ) {
   buf[i] = 0;
   _add(buf);
 }
#endif

 if ( (new = roar_env_get_home(0)) != NULL ) {
  snprintf(buf, sizeof(buf)-1, "%s/%s", new, ROAR_DEFAULT_SOCK_USER);
  buf[sizeof(buf)-1] = 0;
  _add(buf);
 }

 servers[ret++] = roar_mm_strdup(ROAR_DEFAULT_SOCK_GLOBAL);
 servers[ret++] = roar_mm_strdup(ROAR_DEFAULT_HOST);
 servers[ret++] = roar_mm_strdup("::" ROAR_DEFAULT_OBJECT);
 servers[ret++] = roar_mm_strdup("+abstract");
 servers[ret++] = roar_mm_strdup("/tmp/muroard");

 return ret;
}

struct locmed {
 int supflags;
 ssize_t (*func)(int flags, int dir, int socktype, char ** servers, size_t maxlen);
};

static struct locmed _libroar_locmod[] = {
 {ROAR_ENUM_FLAG_NONBLOCK|ROAR_ENUM_FLAG_HARDNONBLOCK, _esl_defaults}
};

struct roar_server * roar_enum_servers(int flags, int dir, int socktype) {
 struct roar_server * ret = NULL;
 struct roar_server * c;
 char * servers[64];
 size_t have = 1;
 size_t i, cp, unic;
 ssize_t r;
 int testflags = flags;
 int is_uniq;

 // load config:
 roar_libroar_get_config();

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

 for (i = cp = 0; i < have; i++) {
  c = &(ret[cp]);
  c->server = servers[i];
  c->description = NULL;
  c->location = NULL;

  // uniq test:
  is_uniq = 1;
  for (unic = 0; unic < cp; unic++)
   if ( !strcmp(ret[unic].server, servers[i]) )
    is_uniq = 0;

  if ( is_uniq && _test_server(c, flags) == 0 ) {
   cp++;
  } else {
   roar_mm_free(servers[i]);
  }
 }

 ret[cp].server = NULL;
 ret[cp].description = roar_mm_strdup("Default server");
 ret[cp].location = NULL;

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
