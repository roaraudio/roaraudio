//connection.c:

#include <libroaryiff.h>

YConnection *YOpenConnection (const char *start_arg, const char *con_arg) {
 YConnection * con = malloc(sizeof(YConnection));

 memset(con, 0, sizeof(YConnection));

 return con;
}

void YCloseConnection (YConnection *connection, Boolean no_shutdown) {
 if ( !connection )
  return;

}

//ll
