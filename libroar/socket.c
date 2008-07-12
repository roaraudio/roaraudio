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
 char * proxy_type = getenv("ROAR_PROXY");

 if ( proxy_type == NULL || strcmp(proxy_type, "") == 0 ) {
  return roar_socket_open(MODE_CONNECT, ROAR_SOCKET_TYPE_UNKNOWN, host, port);
 } else {
  return roar_socket_open_proxy(MODE_CONNECT, ROAR_SOCKET_TYPE_UNKNOWN, host, port, proxy_type);
 }
}

int roar_socket_open (int mode, int type, char * host, int port) {
// int type = ROAR_SOCKET_TYPE_INET;
 int fh;
 struct sockaddr_in   socket_addr;
 struct sockaddr_un   socket_addr_un;
 struct hostent     * he;
 //unsigned int host_div = 0;
 int (*mode_func)(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen) = connect; // default is to connect

 if ( mode == MODE_LISTEN )
  mode_func = bind;

 if ( type == ROAR_SOCKET_TYPE_UNKNOWN ) {
  type = ROAR_SOCKET_TYPE_INET;
  if ( *host == '/' ) {
   type = ROAR_SOCKET_TYPE_UNIX;
  } else if ( strcmp(host, "+fork") == 0 ) {
   type = ROAR_SOCKET_TYPE_FORK;
  }
 }


 ROAR_DBG("roar_socket_open(*): type=%s, host='%s', port=%i",
             type == ROAR_SOCKET_TYPE_UNIX ? "UNIX" : "INET", host, port);

 memset(&socket_addr   , 0, sizeof(socket_addr));
 memset(&socket_addr_un, 0, sizeof(socket_addr_un));
 memset(&he,             0, sizeof(he));               // FIXME: we have a valid pointer in here????


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
 } else if ( type == ROAR_SOCKET_TYPE_UNIX ) {
  socket_addr_un.sun_family = AF_UNIX;
  strncpy(socket_addr_un.sun_path, host, sizeof(socket_addr_un.sun_path) - 1);

  fh = roar_socket_new_unix();

  if ( mode_func(fh, (struct sockaddr *)&socket_addr_un, sizeof(struct sockaddr_un)) == -1 ) {
   ROAR_DBG("roar_socket_open(*): Can not connect/bind: %s", strerror(errno));
   close(fh);
   return -1;
  }
 } else if ( type == ROAR_SOCKET_TYPE_FORK ) {
  return roar_socket_open_fork(mode, host, port);
 } else {
  return -1;
 }

 if ( mode == MODE_LISTEN )
  if ( listen(fh, ROAR_SOCKET_QUEUE_LEN) == -1 ) {
   close(fh);
   return -1;
  }

 return fh;
}

int roar_socket_open_fork  (int mode, char * host, int port) {
 int socks[2];
 int r;
 char fhstr[8];

 if ( mode == MODE_LISTEN )
  return -1;

 if ( socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1 ) {
  return -1;
 }

 r = fork();

 if ( r == -1 ) { // error!
  ROAR_ERR("roar_socket_open_fork(*): Can not fork: %s", strerror(errno));
  close(socks[0]);
  close(socks[1]);
  return -1;
 } else if ( r == 0 ) { // we are the child
  close(socks[0]);

  close(ROAR_STDIN ); // we do not want roard to have any standard input
  close(ROAR_STDOUT); // STDOUT is also not needed, so we close it,
                      // but STDERR we keep open for error messages.

  snprintf(fhstr, 7, "%i", socks[1]);

  execlp("roard", "roard", "--no-listen", "--client-fh", fhstr, NULL);

  // we are still alive?
  ROAR_ERR("roar_socket_open_fork(*): alive after exec(), that's bad!");
  _exit(1);
 } else { // we are the parent
  close(socks[1]);
  return socks[0];
 }

 return -1;
}

// --- [ PROXY CODE ] ---

// generic proxy code:

int roar_socket_open_proxy (int mode, int type, char * host, int port, char * proxy_type) {
 int    proxy_port;
 char   proxy_host[ROAR_SOCKET_MAX_HOSTNAMELEN];
 char * proxy_addr;
 int    i;
 int    fh;

 // TODO: change this so we support listen() proxys (ssh -R)
 if ( mode != MODE_CONNECT )
  return -1;

 if ( !strcmp(proxy_type, "socks4a") ) { // for TOR, the only supported type at the moment
  proxy_addr = getenv("socks_proxy");

  proxy_port = 9050; // TOR's default port

  if ( proxy_addr == NULL )
   return -1;

  for (i = 0; proxy_addr[i] != 0 && proxy_addr[i] != ':' && i < ROAR_SOCKET_MAX_HOSTNAMELEN; i++)
   proxy_host[i] = proxy_addr[i];
  proxy_host[i] = 0;

  if ( i == 0 ) // no hostname found
   return -1;

  if ( proxy_addr[i] == ':' )
   proxy_port = atoi(&proxy_addr[i+1]);

  if ( (fh = roar_socket_open(mode, type, proxy_host, proxy_port)) == -1) {
   return -1;
  }

  if ( roar_socket_open_socks4a(mode, fh, host, port) == -1 ) {
   close(fh);
   return -1;
  }

  return fh;
 } else {
  return -1; // unknown type
 }
}

// protocoll dependet proxy code:

int roar_socket_open_socks4a(int mode, int fh, char * host, int port) {
 char buf[9];
 int  len;

 buf[0] = 0x04;
 buf[1] = mode == MODE_CONNECT ? 0x01 : 0x02;
 *((uint16_t*)&buf[2]) = htons(port);
 buf[4] = 0x00;
 buf[5] = 0x00;
 buf[6] = 0x00;
 buf[7] = 0x01;
 buf[8] = 0x00;

 if ( write(fh, buf, 9) != 9 )
  return -1;

 len = strlen(host);

 if ( write(fh, host, len) != len )
  return -1;

 if ( write(fh, "\0", 1) != 1 )
  return -1;

 if ( read(fh, buf, 8) != 8 )
  return -1;

 if ( buf[1] != 0x5a )
  return -1;

 return 0;
}

//ll
