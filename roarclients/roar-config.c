//roar-config.c:

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

int main (int argc, char * argv[]) {
 int i;

 if ( argc == 1 ) {
  printf("Usage: roar-config [--version] [--libs] [--cflags]\n");
  return 0;
 }

 for (i = 1; i < argc; i++) {
  if ( !strcmp(argv[i], "--version") ) {
   printf("unknown\n");
  } else if ( !strcmp(argv[i], "--libs") ) {
   printf("%s\n", ROAR_LIBS);
  } else if ( !strcmp(argv[i], "--cflags") ) {
   printf("%s\n", ROAR_CFLAGS);
  } else {
   fprintf(stderr, "Unknown option: %s\n", argv[i]);
   return 1;
  }
 }

 return 0;
}

//ll
