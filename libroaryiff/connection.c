//connection.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroaryiff a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroaryiff is distributed in the hope that it will be useful,
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

#include <libroaryiff.h>

YConnection *YOpenConnection (const char *start_arg, const char *con_arg) {
 YConnection * ycon = malloc(sizeof(YConnection));
 struct roar_connection con;
 char * server = (char *)con_arg;
 char * name   = "libroaryiff client";

 memset(ycon, 0, sizeof(YConnection));

 // there is no symbolic value for default con_arg
 // in the heder files. So we use a hard coded value from the docs

 if ( server != NULL ) {
  if ( strcmp(server, "127.0.0.1:9433") == 0 ||
       strcmp(server, "127.0.0.1")      == 0 ||   // \\                                         //
       strcmp(server, "localhost")      == 0 ||   //   => Don't know if this is valid for libY  //
       strcmp(server, "localhost:9433") == 0 ) {  // //   but we support it.                    //

   server = NULL; // try default locations
  }
 }

 if (roar_simple_connect(&con, server, name) == -1) {
  // Handle start_arg here!
  free(ycon);
  return NULL;
 }

 if ( (ycon->fd = roar_get_connection_fh(&con)) == -1 ) {
  roar_disconnect(&con);
  free(ycon);
  return NULL;
 }

 return ycon;
}

void YCloseConnection (YConnection *connection, Boolean no_shutdown) {
 if ( !connection )
  return;

 roar_simple_close(connection->fd);  // roard will clean up all other sockets if the close the main one.


 free(connection);
}

//ll
