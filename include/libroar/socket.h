//libroar.h:

#ifndef _LIBROARSOCKET_H_
#define _LIBROARSOCKET_H_

#include "libroar.h"

#define ROAR_SOCKET_MODE_LISTEN  1
#define ROAR_SOCKET_MODE_CONNECT 2

#define ROAR_SOCKET_TYPE_NONE 0
#define ROAR_SOCKET_TYPE_UNKNOWN ROAR_SOCKET_TYPE_NONE
#define ROAR_SOCKET_TYPE_INET 1
#define ROAR_SOCKET_TYPE_TCP  ROAR_SOCKET_TYPE_INET
#define ROAR_SOCKET_TYPE_UNIX 2
#define ROAR_SOCKET_TYPE_FORK 3
#define ROAR_SOCKET_TYPE_PIPE ROAR_SOCKET_TYPE_FORK
#define ROAR_SOCKET_TYPE_FILE 4
#define ROAR_SOCKET_TYPE_UDP  5
#define ROAR_SOCKET_TYPE_GENSTR 6 /* generic stream: TCP or UNIX */
#define ROAR_SOCKET_TYPE_DECNET 7 /* DECnet */

#define ROAR_SOCKET_TYPE_MAX  4

#define ROAR_SOCKET_QUEUE_LEN 8

#define ROAR_SOCKET_BLOCK     1
#define ROAR_SOCKET_NONBLOCK  2

#define ROAR_SOCKET_MAX_HOSTNAMELEN 64

int roar_socket_listen  (int type, char * host, int port);
int roar_socket_connect (char * host, int port);

int roar_socket_new_tcp    (void);
int roar_socket_new_udp    (void);
int roar_socket_new_unix   (void);
int roar_socket_new_decnet_seqpacket (void);
int roar_socket_new_decnet_stream (void);

int roar_socket_open       (int mode, int type, char * host, int port);
int roar_socket_open_fork  (int mode, char * host, int port);
int roar_socket_open_file  (int mode, char * host, int port);
int roar_socket_open_proxy (int mode, int type, char * host, int port, char * proxy_type);

int roar_socket_listen_decnet (char * object, int num);

int roar_socket_nonblock(int fh, int state);

int roar_socket_dup_udp_local_end (int fh);

int roar_socket_open_socks4a(int mode, int fh, char * host, int port);

#endif

//ll
