//x11.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libpulse*. They are mostly copyrighted by:
 *  Lennart Poettering <poettering@users.sourceforge.net> and
 *  Pierre Ossman <drzeus@drzeus.cx>
 *
 *  This file is part of libroarpulse a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
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
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>

#ifndef ROAR_HAVE_LIBX11
typedef void Display;
#endif

void pa_x11_set_prop(Display *d, const char *name, const char *data) {
 struct roar_x11_connection * x11con = roar_x11_connect_display(d);

 if ( x11con == NULL )
  return;

 roar_x11_set_prop(x11con, name, data);

 roar_x11_disconnect(x11con);
}

void pa_x11_del_prop(Display *d, const char *name) {
 struct roar_x11_connection * x11con = roar_x11_connect_display(d);

 if ( x11con == NULL )
  return;

 roar_x11_delete_prop(x11con, name);

 roar_x11_disconnect(x11con);
}

char* pa_x11_get_prop(Display *d, const char *name, char *p, size_t l) {
 struct roar_x11_connection * x11con = roar_x11_connect_display(d);
 char * val;
 size_t len;

 if ( x11con == NULL )
  return NULL;

 val = roar_x11_get_prop(x11con, name);

 roar_x11_disconnect(x11con);

 if ( val == NULL )
  return NULL;

 len = strlen(val);

 if ( (l - 1) < len )
  len = l - 1;

 memcpy(p, val, len);
 p[len] = 0;

 return p;
}

//ll
