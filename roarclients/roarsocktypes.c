//roarsocktypes.c:

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

int main (void) {
 int i, fh;
 struct {
  char * name;
  int (*func)(void);
 } tests[] = {
  {"TCPv4"           , roar_socket_new_tcp },
  {"UDPv4"           , roar_socket_new_udp },
  {"TCPv6"           , roar_socket_new_tcp6},
  {"UDPv6"           , roar_socket_new_udp6},
  {"UNIX"            , roar_socket_new_unix},
  {"DECnet seqpacket", roar_socket_new_decnet_seqpacket},
  {"DECnet stream"   , roar_socket_new_decnet_stream},
  {"IPX"             , roar_socket_new_ipx},
  {"IPX/SPX"         , roar_socket_new_ipxspx},
  {NULL, NULL}
 };

 for (i = 0; tests[i].func; i++) {
  printf("Type %-16s ", tests[i].name);
  errno = 0;
  fh = tests[i].func();
  if ( fh == -1 ) {
   printf("not working: %s\n", strerror(errno));
  } else {
   close(fh);
   printf("working\n");
  }
 }

 return 0;
}

//ll
