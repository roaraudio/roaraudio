//config.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

struct roar_libroar_config * roar_libroar_get_config_ptr(void) {
 static struct roar_libroar_config config;
 static int inited = 0;

 if ( !inited ) {
  memset(&config, 0, sizeof(config));
  inited++;
 }

 return &config;
}

struct roar_libroar_config * roar_libroar_get_config(void) {
 struct roar_libroar_config * config = roar_libroar_get_config_ptr();
 static int inited = 0;
 char * k, * v, * next;

 if ( !inited ) {
  next = getenv("ROAR_OPTIONS");

  while (next != NULL) {
   k = next;
   next = strstr(next, " ");
   if ( next != NULL ) {
    *next = 0;
     next++;
   }

   if ( (v = strstr(k, ":")) != NULL ) {
    *v = 0;
     v++;
   }

   if ( !strcmp(k, "workaround") ) {
    if ( !strcmp(v, "use-execed") ) {
     config->workaround.workarounds |= ROAR_LIBROAR_CONFIG_WAS_USE_EXECED;
    } else {
     ROAR_WARN("roar_libroar_get_config(void): Unknown workaround option: %s", v);
    }
   } else {
    ROAR_WARN("roar_libroar_get_config(void): Unknown option: %s", k);
   }
  }

  inited++;
 }

 return config;
}


//ll
