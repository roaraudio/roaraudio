//socket.c:

#include "libroar.h"

#define MODE_LISTEN  1
#define MODE_CONNECT 2

int roar_socket_new_tcp (void) {
 int fh;
// int extra_flags = TCP_NODELAY;

 fh = socket(PF_INET, SOCK_STREAM, 0);

 return fh;
}

int roar_socket_new_unix (void) {
 int fh;

 fh = socket(AF_UNIX, SOCK_STREAM, 0);

 return fh;
}

int roar_socket_nonblock(int fh, int state) {
 int flags;

 if ( (flags = fcntl(fh, F_GETFL, 0)) == -1 ) {
  ROAR_ERR("roar_socket_nonblock(fh=%i, state=%i): Can not read flags: %s", fh, state, strerror(errno));
  ROAR_DBG("roar_socket_nonblock(fh=%i, state=%i) = -1", fh, state);
  return -1;
 }

 flags |= O_NONBLOCK;

 if ( state == ROAR_SOCKET_BLOCK )
  flags -= O_NONBLOCK;

 if ( fcntl(fh, F_SETFL, flags) == -1 ) {
  ROAR_ERR("roar_socket_nonblock(fh=%i, state=%i): Can not set flags: %s", fh, state, strerror(errno));
  ROAR_DBG("roar_socket_nonblock(fh=%i, state=%i) = -1", fh, state);
  return -1;
 }

 ROAR_DBG("roar_socket_nonblock(fh=%i, state=%i) = 0", fh, state);
 return 0;
}

int roar_socket_listen  (int type, char * host, int port) {
 return roar_socket_open(MODE_LISTEN, type, host, port);
}

int roar_socket_connect (char * host, int port) {
 return roar_socket_open(MODE_CONNECT, ROAR_SOCKET_TYPE_UNKNOWN, host, port);
}

int roar_socket_open (int mode, int type, char * host, int port) {
// int type = ROAR_SOCKET_TYPE_INET;
 int fh;
 struct sockaddr_in   socket_addr;
 struct hostent     * he;
 //unsigned int host_div = 0;
 int (*mode_func)(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen) = connect; // default is to connect

 if ( mode == MODE_LISTEN )
  mode_func = bind;

 if ( type == ROAR_SOCKET_TYPE_UNKNOWN ) {
  type = ROAR_SOCKET_TYPE_INET;
  if ( *host == '/' )
   type = ROAR_SOCKET_TYPE_UNIX;
 }


 ROAR_DBG("roar_socket_open(*): type=%s, host='%s', port=%i",
             type == ROAR_SOCKET_TYPE_UNIX ? "UNIX" : "INET", host, port);

 memset(&socket_addr, 0, sizeof(socket_addr));
 memset(&he,          0, sizeof(he));


 if ( type == ROAR_SOCKET_TYPE_INET ) {

  if ( (he = gethostbyname(host)) == NULL ) {
   ROAR_ERR("roar_socket_open(*): Can\'t resolve host name '%s'",
                     host);
   return -1;
  }

  memcpy((struct in_addr *)&socket_addr.sin_addr, he->h_addr, sizeof(struct in_addr));

  /* set the connect information */
  socket_addr.sin_family = AF_INET;
  socket_addr.sin_port = htons( port );

  fh = roar_socket_new_tcp();

  if ( mode_func(fh, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr_in)) == -1 ) {
   ROAR_DBG("roar_socket_open(*): Can not connect/bind: %s", strerror(errno));
   close(fh);
   return -1;
  }
  // hey! we have a socket...
 } else {
  fh = roar_socket_new_unix();
  close(fh);
  return -1;
 }

 if ( mode == MODE_LISTEN )
  if ( listen(fh, ROAR_SOCKET_QUEUE_LEN) == -1 ) {
   close(fh);
   return -1;
  }

 return fh;
}

//ll
