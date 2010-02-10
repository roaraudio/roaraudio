//roarx11.h:

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

#ifndef _LIBROARX11_H_
#define _LIBROARX11_H_

#include "libroar.h"

struct roar_x11_connection {
#ifdef ROAR_HAVE_LIBX11
 Display * display;
#else
 char dummy[8];
#endif
};

struct roar_x11_connection * roar_x11_connect(char * display);
int    roar_x11_disconnect(struct roar_x11_connection * con);

#define roar_x11_get_display(con) ((con) == NULL ? NULL : (con)->display)

int    roar_x11_set_prop(struct roar_x11_connection * con, const char * key, const char * val);
int    roar_x11_delete_prop(struct roar_x11_connection * con, const char * key);
char * roar_x11_get_prop(struct roar_x11_connection * con, const char * key);

#endif

//ll
