//connection.c:

#include <libroaryiff.h>

YConnection *YOpenConnection (const char *start_arg, const char *con_arg) {
 YConnection * ycon = malloc(sizeof(YConnection));
 struct roar_connection con;
 char * server = (char *)con_arg;
 char * name   = "libroaryiff client";

 memset(ycon, 0, sizeof(YConnection));

 // there is no symbolic value for default con_arg
 // in the heder files. So we use a hard coded value from the docs

 if ( strcmp(server, "127.0.0.1:9433") == 0 ||
      strcmp(server, "127.0.0.1")      == 0 ||   // \\                                         //
      strcmp(server, "localhost")      == 0 ||   //   => Don't know if this is valid for libY  //
      strcmp(server, "localhost:9433") == 0 ) {  // //   but we support it.                    //

  server = NULL; // try default locations
 }

 if (roar_simple_connect(&con, server, name) == -1) {
  // Handle start_arg here!
  free(ycon);
  return NULL;
 }

 ycon->fd = con.fh;

 return ycon;
}

void YCloseConnection (YConnection *connection, Boolean no_shutdown) {
 if ( !connection )
  return;

 roar_simple_close(connection->fd);  // roard will clean up all other sockets if the close the main one.


 free(connection);
}

//ll
