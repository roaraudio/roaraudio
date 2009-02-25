//pinentry.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

int roar_pinentry_open (struct roar_pinentry * pe, int flags, char * display, char * tty, char * term) {
 if ( pe == NULL )
  return -1;

 memset(pe, 0, sizeof(struct roar_pinentry));
 pe->in  = -1;
 pe->out = -1;

#ifdef ROAR_HAVE_BIN_PINENTRY
 return 0;
#else
 return -1;
#endif
}

int roar_pinentry_simple_open(struct roar_pinentry * pe) {
 return roar_pinentry_open(pe, 0, NULL, NULL, NULL);
}

int roar_pinentry_close(struct roar_pinentry * pe) {
 if ( pe == NULL )
  return -1;

 if ( pe->opened == 0 )
  return 0;

 if ( pe->out != -1 )
  close(pe->out);

 if ( pe->in  != -1 )
  close(pe->in);

 return 0;
}

int roar_pinentry_send (struct roar_pinentry * pe, char * cmd,  char * args) {
 size_t len;

 if ( pe == NULL )
  return -1;

 if ( cmd == NULL )
  return -1;

 len = strlen(cmd);

 if ( write(pe->out, cmd, len) != len )
  return -1;

 if ( args != NULL ) {
  len = strlen(args);

  if ( write(pe->out, args, len) != len )
   return -1;
 }

 if ( write(pe->out, "\n", 1) != 1 )
  return -1;

 return 0;
}

int roar_pinentry_recv (struct roar_pinentry * pe, char * line, char * opts) {
 return -1;
}

int roar_pinentry_req  (struct roar_pinentry * pe, char * cmd,  char * args, char * line, char * opts) {
 if ( pe == NULL )
  return -1;

 if ( roar_pinentry_send(pe, cmd, args) != 0 )
  return -1;

 return roar_pinentry_recv(pe, line, opts);
}

int roar_pinentry_set_desc (struct roar_pinentry * pe, char * desc) {
 return roar_pinentry_set(pe, "DESC", desc);
}

int roar_pinentry_set      (struct roar_pinentry * pe, char * obj, char * text) {
 char req[80] = "SET";

 if ( pe == NULL )
  return -1;

 if ( obj == NULL )
  return -1;

 if ( strlen(obj) > (80-1 /* \0 */ + 3 /* "SET" */ + 1 /* " " */) )
  return -1;

 strncat(req, obj, 80-5);
 strncat(req, " ", 2);

 return roar_pinentry_req(pe, req, text, NULL, NULL);
}

//ll
