//roarsockconnect.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include <roaraudio.h>
#include <string.h>
#include <stdio.h>

void usage (void) {
 fprintf(stderr, "Usage: roarsockconnect HOST PORT\n");

 exit(1);
}

int main (int argc, char * argv[]) {
 char buf[1024];
 int fh;
 int port;
 int len;

 if ( argc != 3 )
  usage();

 port     = atoi(argv[2]);

 if ( (fh = roar_socket_connect(argv[1], port)) == -1 ) {
  ROAR_ERR("Error: can not connect!");
  return 1;
 }

 while ((len = read(ROAR_STDIN, buf, 1024)))
  write(fh, buf, len);

 while ((len = read(fh, buf, 1024)))
  write(ROAR_STDOUT, buf, len);

 return 0;
}

//ll
