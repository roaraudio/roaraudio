//roarclientpass.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

void usage (void) {
 printf("roarclientpass [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server    SERVER    - Set server hostname\n"
        "  --stdin               - Client is on stdin\n"
        "  --stdout              - Client is on stdout\n"
        "  --stdio               - Same as --stdin --stdout\n"
        "  --stderr              - Client is on stderr\n"
        "  --client-fh FH        - Client is on FH\n"
        "  --proto PROTO         - Client uses protocol PROTO (default: RoarAudio)\n"
        "  --byteorder BO        - Client uses byteorder BO (default: network)\n"
        "  --listen              - This is a listen mode connection\n"
        "  --help                - Show this help\n"
       );

}

#define _BV(x) (1<<(x))
#define F_STDIN  _BV(ROAR_STDIN)
#define F_STDOUT _BV(ROAR_STDOUT)
#define F_STDERR _BV(ROAR_STDERR)

int main (int argc, char * argv[]) {
 struct roar_connection    con;
 struct roar_client        client;
 char * server    = NULL;
 char * k;
 int    i;
 int    clientfh  = -1;
 int    cflags    = 0;
 int    flags     = 0;
 int    proto     = ROAR_PROTO_ROARAUDIO;
 int    byteorder = ROAR_BYTEORDER_NETWORK;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( !strcmp(k, "--server") || !strcmp(k, "-s") ) {
   server = argv[++i];
  } else if ( !strcmp(k, "--stdin") ) {
   cflags |= F_STDIN;
  } else if ( !strcmp(k, "--stdout") ) {
   cflags |= F_STDOUT;
  } else if ( !strcmp(k, "--stderr") ) {
   cflags |= F_STDERR;
  } else if ( !strcmp(k, "--stdio") ) {
   cflags |= F_STDIN|F_STDOUT;
  } else if ( !strcmp(k, "--client-fh") ) {
   clientfh = atoi(argv[++i]);
  } else if ( !strcmp(k, "--proto") ) {
   proto = roar_str2proto(argv[++i]);
  } else if ( !strcmp(k, "--byteorder") ) {
   byteorder = roar_str2byteorder(argv[++i]);
  } else if ( !strcmp(k, "--listen") ) {
   flags |= ROAR_CLIENTPASS_FLAG_LISTEN;
  } else {
   ROAR_ERR("unknown argument: %s", k);
   usage();
   return 1;
  }
 }

 if ( cflags & F_STDERR ) {
#ifdef ROAR_HAVE_SYSLOG
  roar_debug_set_stderr_mode(ROAR_DEBUG_MODE_SYSLOG);
#else
  roar_debug_set_stderr_fh(-1);
#endif
 } else {
  roar_debug_set_stderr_fh(ROAR_STDERR);
 }

 if ( clientfh == -1 ) {
  if ( cflags & F_STDIN ) {
   clientfh = ROAR_STDIN;
  } else if ( cflags & F_STDOUT ) {
   clientfh = ROAR_STDOUT;
  } else if ( cflags & F_STDERR ) {
   clientfh = ROAR_STDERR;
  } else {
   ROAR_ERR("No client socket given");
  }
 }

 roar_client_new(&client);
 roar_client_set_fh(&client, clientfh);
 roar_client_set_proto(&client, proto, byteorder);

 if ( roar_simple_connect(&con, server, "roarclientpass") == -1 ) {
  ROAR_ERR("Can not connect to server");
  return 10;
 }

 if ( roar_client_pass(&con, &client, flags) == -1 ) {
  ROAR_ERR("Can not pass client fh to server");
  roar_disconnect(&con);
  return 20;
 }

 roar_disconnect(&con);

 return 0;
}

//ll
