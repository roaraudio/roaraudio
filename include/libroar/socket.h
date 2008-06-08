//libroar.h:

#ifndef _LIBROARSOCKET_H_
#define _LIBROARSOCKET_H_

#include "libroar.h"

#define ROAR_SOCKET_TYPE_NONE 0
#define ROAR_SOCKET_TYPE_UNKNOWN ROAR_SOCKET_TYPE_NONE
#define ROAR_SOCKET_TYPE_INET 1
#define ROAR_SOCKET_TYPE_UNIX 2

#define ROAR_SOCKET_QUEUE_LEN 8

#define ROAR_SOCKET_BLOCK     1
#define ROAR_SOCKET_NONBLOCK  2

int roar_socket_listen  (int type, char * host, int port);
int roar_socket_connect (char * host, int port);

int roar_socket_new_tcp (void);
int roar_socket_new_unix (void);
int roar_socket_open (int mode, int type, char * host, int port);

int roar_socket_nonblock(int fh, int state);

#endif

//ll
