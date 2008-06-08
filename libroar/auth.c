//auth.c:

#include "libroar.h"

int roar_auth   (struct roar_connection * con) {
 struct roar_message mes;

 mes.cmd     = ROAR_CMD_AUTH;
 mes.datalen = 0;

 return roar_req(con, &mes, NULL);
}

//ll
