//roarx11.c:

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

#include "libroar.h"

struct roar_x11_connection * roar_x11_connect(char * display) {
#ifdef ROAR_HAVE_LIBX11
 struct roar_libroar_config * config = roar_libroar_get_config();
 struct roar_x11_connection * con;

 if ( display == NULL )
  display = config->x11.display;

 if ( (con = roar_mm_malloc(sizeof(struct roar_x11_connection))) == NULL )
  return NULL;

 if ( (con->display = XOpenDisplay(display)) == NULL ) {
  roar_mm_free(con);
  return NULL;
 }

 return con;
#else
 return NULL;
#endif
}

int roar_x11_disconnect(struct roar_x11_connection * con) {
#ifdef ROAR_HAVE_LIBX11
 int ret;

 if ( con == NULL )
  return -1;

 ret = XCloseDisplay(con->display);

 roar_mm_free(con);

 return ret;
#else
 return -1;
#endif
}

int roar_x11_set_prop(struct roar_x11_connection * con, const char * key, const char * val) {
#ifdef ROAR_HAVE_LIBX11
 Atom a;

 if ( con == NULL )
  return -1;

 a = XInternAtom(con->display, key, False);

 XChangeProperty(con->display, RootWindow(con->display, 0), a, XA_STRING, 8, PropModeReplace, (const unsigned char*) val, strlen(val)+1);

 return 0;
#else
 return -1;
#endif
}

int roar_x11_delete_prop(struct roar_x11_connection * con, const char * key) {
#ifdef ROAR_HAVE_LIBX11
 Atom a;
 if ( con == NULL )
  return -1;

 a = XInternAtom(con->display, key, False);
 XDeleteProperty(con->display, RootWindow(con->display, 0), a);

 return 0;
#else
 return -1;
#endif
}

char * roar_x11_get_prop(struct roar_x11_connection * con, const char * key) {
 unsigned long   nitems;
 unsigned long   nbytes_after;
 unsigned char * prop = NULL;
 char          * ret = NULL;
 int             actual_format;
 Atom            actual_type;
 Atom            a;

 a = XInternAtom(con->display, key, False);

 if ( XGetWindowProperty(con->display, RootWindow(con->display, 0), a,
                         0, 256, False, XA_STRING, &actual_type,
                         &actual_format, &nitems, &nbytes_after, &prop) != Success ) {
  if ( prop != NULL )
   XFree(prop);

  return NULL;
 }

 if ( prop == NULL )
  return NULL;

 if (actual_type != XA_STRING) {
  XFree(prop);

  return NULL;
 }

 ret = roar_mm_malloc(nitems+1);

 memcpy(ret, prop, nitems);
 ret[nitems] = 0;

 XFree(prop);

 return ret;
}

//ll
