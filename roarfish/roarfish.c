//roarfish.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarfish a part of RoarAudio,
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roarfish.h"

void usage (void) {
 printf("Usage: roarfish COMMAND FILE [OPTIONS]\n");
 printf("\nCOMMANDS:\n");
 printf("play          - plays a file\n"
       );
}

int main (int argc, char * argv[]) {
// int i;
 char * command, * file;

 if ( argc < 3 ) {
  usage();
  return 1;
 }

 command = argv[1];
 file    = argv[2];

 if ( !strcmp(command, "play") ) {
  play(file, NULL);
 } else {
  usage();
  return 1;
 }

 return 0;
}

//ll
