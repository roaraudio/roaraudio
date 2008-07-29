//ctl.c:

#include <libroaryiff.h>

void YShutdownServer(YConnection *con) {
 struct roar_connection rcon;

 if ( !con )
  return;

 rcon.fh = con->fd;

 roar_exit(&rcon);
}

//ll
