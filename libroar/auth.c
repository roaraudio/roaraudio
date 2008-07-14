//auth.c:

#include "libroar.h"

int roar_auth   (struct roar_connection * con) {
 struct roar_message mes;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd     = ROAR_CMD_AUTH;
 mes.datalen = 0;

 return roar_req(con, &mes, NULL);
}

//ll
