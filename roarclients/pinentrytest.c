//pinentrytest.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
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
 */

#include <roaraudio.h>

#define BUF_MAX 1024

int main (void) {
 char * pw;
 struct roar_pinentry pe[1];

 roar_pinentry_simple_open(pe);
 printf("roar_pinentry_getpin(%p, ['%s'], 'desc', 'prompt:') = %i\n", pe, pw, roar_pinentry_getpin(pe, &pw, "desc", "prompt:"));
 printf("roar_pinentry_confirm(%p, 'desc', 'Jo!', 'ne...') = %i\n", pe, roar_pinentry_confirm(pe, "desc", "Jo!", "ne..."));
 roar_pinentry_close(pe);

 return 0;
}

//ll
