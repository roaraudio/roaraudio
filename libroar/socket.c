//socket.c:

#include "libroar.h"

#define MODE_LISTEN  ROAR_SOCKET_MODE_LISTEN
#define MODE_CONNECT ROAR_SOCKET_MODE_CONNECT

int roar_socket_new_tcp (void) {
 int fh;
 int opt = IPTOS_LOWDELAY;

 fh = socket(PF_INET, SOCK_STREAM, 0);

 setsockopt(fh, IPPROTO_IP, IP_TOS, &opt, sizeof(int));

 return fh;
}

int roar_socket_new_udp (void) {
 int fh;
 int opt = IPTOS_LOWDELAY;

 fh = socket(PF_INET, SOCK_DGRAM, 0);

 setsockopt(fh, IPPROTO_IP, IP_TOS, &opt, sizeof(int));

 return fh;
}

int roar_socket_new_tcp6 (void) {
#ifdef PF_INET6
 int fh;
 int opt = IPTOS_LOWDELAY;

 fh = socket(PF_INET6, SOCK_STREAM, 0);

 setsockopt(fh, IPPROTO_IP, IP_TOS, &opt, sizeof(int));

 return fh;
#else
 return -1;
#endif
}

int roar_socket_new_udp6 (void) {
#ifdef PF_INET6
 int fh;
 int opt = IPTOS_LOWDELAY;

 fh = socket(PF_INET6, SOCK_DGRAM, 0);

 setsockopt(fh, IPPROTO_IP, IP_TOS, &opt, sizeof(int));

 return fh;
#else
 return -1;
#endif
}

int roar_socket_new_unix (void) {
 int fh;
#ifdef SO_PEERCRED
 int opt = 1;
#endif

 fh = socket(AF_UNIX, SOCK_STREAM, 0);

#ifdef SO_PEERCRED
 setsockopt(fh, SOL_SOCKET, SO_PASSCRED, &opt, sizeof(int));
#endif

 return fh;
}

int roar_socket_new_decnet_seqpacket (void) {
#ifdef ROAR_HAVE_LIBDNET
 int fh;

 fh = socket(AF_DECnet, SOCK_SEQPACKET, DNPROTO_NSP);

 return fh;
#else
 return -1;
#endif
}


int roar_socket_new_decnet_stream (void) {
#ifdef ROAR_HAVE_LIBDNET
 int fh;

 fh = socket(AF_DECnet, SOCK_STREAM, DNPROTO_NSP);

 return fh;
#else
 return -1;
#endif
}

int roar_socket_new_ipxspx (void) {
 return -1;
}

int roar_socket_new_ipx    (void) {
#ifdef ROAR_HAVE_IPX
 return socket(AF_IPX, SOCK_DGRAM, AF_IPX);
#else
 return -1;
#endif
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

int roar_socket_dup_udp_local_end (int fh) {
 int                  n              = -1;
 int                  flags          = -1;
 struct sockaddr_in   socket_addr;
 socklen_t            len            = sizeof(struct sockaddr_in);

 if ( (flags = fcntl(fh, F_GETFL, 0)) == -1 ) {
  ROAR_WARN("roar_socket_dup_udp_local_end(fh=%i): Can not read flags: %s", fh, strerror(errno));
 }

 if ( getsockname(fh, (struct sockaddr *)&socket_addr, &len) == -1 ) {
  return -1;
 }

 if ( socket_addr.sin_family != AF_INET ) {
  return -1;
 }

 n = roar_socket_new_udp();

 if ( n == -1 )
  return -1;

//  if ( mode_func(fh, (struct sockaddr *)&socket_addr, sizeof(struct sockaddr_in)) == -1 ) {
 if ( bind(n, (struct sockaddr *)&socket_addr, len) == -1 ) {
  close(n);
  return -1;
 }

 if ( flags != -1 ) {
  if ( fcntl(fh, F_SETFL, flags) == -1 ) {
   ROAR_WARN("roar_socket_dup_udp_local_end(fh=%i): Can not set flags: %s", fh, strerror(errno));
   return -1;
  }
 }


 return n;
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


int roar_socket_listen_decnet (char * object, int num) {
#ifdef ROAR_HAVE_LIBDNET
 int fh = roar_socket_new_decnet_stream();
 struct sockaddr_dn bind_sockaddr;

 if ( fh == -1 )
  return -1;

 if ( !*object )
  object = NULL;

 if ( (object && num) || (!*object && !num) ) {
  ROAR_WARN("roar_socket_listen_decnet(object='%s', num=%i): illegal address!", object, num);
  close(fh);
  return -1;
 }

 memset((void*)&bind_sockaddr, 0, sizeof(struct sockaddr_dn));

 bind_sockaddr.sdn_family    = AF_DECnet;
 bind_sockaddr.sdn_flags     = 0;
 bind_sockaddr.sdn_objnum    = num;

 if ( num ) {
  bind_sockaddr.sdn_objnamel = 0;
 } else {
  bind_sockaddr.sdn_objnamel  = ROAR_dn_htons(strlen(object));
  strcpy((char*)bind_sockaddr.sdn_objname, object); // FIXME: shouldn't we use strncpy()?
 }

 if ( bind(fh, (struct sockaddr *) &bind_sockaddr, sizeof(bind_sockaddr)) == -1 ) {
  close(fh);
  return -1;
 }

 if ( listen(fh, 8) == -1 ) {
  close(fh);
  return -1;
 }

 return fh;
#else
 return -1;
#endif
}

char * roar_socket_get_local_nodename(void) {
#ifdef ROAR_HAVE_LIBDNET
 static char node[16] = {0};
 struct dn_naddr      *binaddr;
 struct nodeent       *dp;

 if ( !node[0] ) {
  if ( (binaddr=getnodeadd()) == NULL)
   return NULL;

  if ( (dp=getnodebyaddr((char*)binaddr->a_addr, binaddr->a_len, PF_DECnet)) == NULL )
   return NULL;

  strncpy(node, dp->n_name, 15);
  node[15] = 0;
 }

 return node;
#else
 return NULL;
#endif
}

int roar_socket_open (int mode, int type, char * host, int port) {
// int type = ROAR_SOCKET_TYPE_INET;
 int fh;
#ifdef ROAR_HAVE_IPX
#define _NEED_OBJ
 int i;
 int ret;
#endif
 union {
  struct sockaddr_in  in;
  struct sockaddr_un  un;
  struct sockaddr_in6 in6;
#ifdef ROAR_HAVE_IPX
  struct sockaddr_ipx ipx;
#endif
 } socket_addr;
 struct hostent     * he;
 //unsigned int host_div = 0;
 int (*mode_func)(int sockfd, const struct sockaddr *serv_addr, socklen_t addrlen) = connect; // default is to connect
#ifdef ROAR_HAVE_LIBDNET
#define _NEED_OBJ
#endif
#ifdef _NEED_OBJ
 char obj[80];
 char * del;
#endif

 if ( mode == MODE_LISTEN )
  mode_func = bind;

 if ( type == ROAR_SOCKET_TYPE_UNKNOWN ) {
  type = ROAR_SOCKET_TYPE_INET;
  if ( *host == '/' ) {
   type = ROAR_SOCKET_TYPE_UNIX;
  } else if ( strcmp(host, "+fork") == 0 ) {
   type = ROAR_SOCKET_TYPE_FORK;
  } else if ( strstr(host, "::") != NULL ) {
   type = ROAR_SOCKET_TYPE_DECNET;
  } else if ( host[strlen(host)-1] == ')' ) {
   type = ROAR_SOCKET_TYPE_IPX;
  }
 }


 ROAR_DBG("roar_socket_open(*): type=%s, host='%s', port=%i",
             type == ROAR_SOCKET_TYPE_UNIX ? "UNIX" : "INET", host, port);

 if ( type == ROAR_SOCKET_TYPE_DECNET ) {
#ifdef ROAR_HAVE_LIBDNET
   ROAR_DBG("roar_socket_open(*): hostname for DECnet: host(%p)=%s", host, host);
   del = strstr(host, "::");
   ROAR_DBG("roar_socket_open(*): hostname for DECnet: del(%p)=%s", del, del);

   if ( del == NULL ) {
    ROAR_WARN("roar_socket_open(*): invalid hostname for DECnet: %s", host);
    return -1;
   }

   *del = 0;

   if ( *(del+2) == '#' ) { // assume we have node::#num
    port = atoi(del+2);
   }

   if ( port ) {
    sprintf(obj, "%i", port); // no need for snprintf() as dec(port) is smaller than obj[]
   } else {
    *obj = 0;
    strncat(obj, del+2, 79);
   }

  if ( mode == MODE_LISTEN ) {
   fh = roar_socket_listen_decnet(obj, port);
   *del = ':';
   return fh;
//   return -1; // listen sockets on DECnet are not supportet at the moment
  } else {
   // There is nothing wrong in this case to use dnet_conn() so we do.
   fh = dnet_conn(host, obj, SOCK_STREAM, 0 ,0 ,0 , 0);
   *del = ':';
   return fh;
  }
#else
  return -1; // no decnet support
#endif
 }

 memset(&socket_addr,    0, sizeof(socket_addr));
 memset(&he,             0, sizeof(he));               // FIXME: we have a valid pointer in here????


 if ( type == ROAR_SOCKET_TYPE_INET || type == ROAR_SOCKET_TYPE_INET6 ) {

  if ( (he = gethostbyname(host)) == NULL ) {
   ROAR_ERR("roar_socket_open(*): Can\'t resolve host name '%s'",
                     host);
   return -1;
  }

  if ( he->h_addrtype == AF_INET ) {
   if ( type != ROAR_SOCKET_TYPE_INET )
    return -1;

   memcpy((struct in_addr *)&socket_addr.in.sin_addr, he->h_addr, sizeof(struct in_addr));

   /* set the connect information */
   socket_addr.in.sin_family = AF_INET;
   socket_addr.in.sin_port   = ROAR_HOST2NET16(port);

   fh = roar_socket_new_tcp();

   if ( mode_func(fh, (struct sockaddr *)&socket_addr.in, sizeof(struct sockaddr_in)) == -1 ) {
    ROAR_DBG("roar_socket_open(*): Can not connect/bind: %s", strerror(errno));
    close(fh);
    return -1;
   }
  } if ( he->h_addrtype == AF_INET6 ) {
   if ( type != ROAR_SOCKET_TYPE_INET6 )
    return -1;

   memcpy((struct in6_addr *)&socket_addr.in6.sin6_addr, he->h_addr, sizeof(struct in6_addr));

   /* set the connect information */
   socket_addr.in6.sin6_family = AF_INET6;
   socket_addr.in6.sin6_port   = ROAR_HOST2NET16(port);

   fh = roar_socket_new_tcp6();

   if ( mode_func(fh, (struct sockaddr *)&socket_addr.in6, sizeof(struct sockaddr_in6)) == -1 ) {
    ROAR_DBG("roar_socket_open(*): Can not connect/bind: %s", strerror(errno));
    close(fh);
    return -1;
   }
  } else {
   return -1;
  }
  // hey! we have a socket...
 } else if ( type == ROAR_SOCKET_TYPE_UNIX ) {
  socket_addr.un.sun_family = AF_UNIX;
  strncpy(socket_addr.un.sun_path, host, sizeof(socket_addr.un.sun_path) - 1);

  fh = roar_socket_new_unix();

  if ( mode_func(fh, (struct sockaddr *)&socket_addr.un, sizeof(struct sockaddr_un)) == -1 ) {
   ROAR_DBG("roar_socket_open(*): Can not connect/bind: %s", strerror(errno));
   close(fh);
   return -1;
  }
 } else if ( type == ROAR_SOCKET_TYPE_IPX ) {
#ifdef ROAR_HAVE_IPX
  socket_addr.ipx.sipx_family = AF_IPX;

  obj[0] = 0;

  if ( (ret = sscanf(host, "%8x.%12s(%x)", &socket_addr.ipx.sipx_network, obj,
                               (unsigned int *)&socket_addr.ipx.sipx_port)) < 2 ) {
   return -1;
  } else if ( ret == 2 ) {
   socket_addr.ipx.sipx_port = port; // Network Byte Order?
  }

  memset(socket_addr.ipx.sipx_node, 0, IPX_NODE_LEN);
  ret = strlen(obj);

  if ( ret % 2 )  // needs to be even at the moment
   return -1;

  fh = roar_socket_new_ipx();

  close(fh);
  return -1;
#else
  return -1;
#endif
 } else if ( type == ROAR_SOCKET_TYPE_FORK ) {
  return roar_socket_open_fork(mode, host, port);
 } else if ( type == ROAR_SOCKET_TYPE_FILE ) {
  return roar_socket_open_file(mode, host, port);
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

  execlp("roard", "roard", "--terminate", "--no-listen", "--client-fh", fhstr, NULL);

  // we are still alive?
  ROAR_ERR("roar_socket_open_fork(*): alive after exec(), that's bad!");
  _exit(1);
 } else { // we are the parent
  close(socks[1]);
  return socks[0];
 }

 return -1;
}

int roar_socket_open_file  (int mode, char * host, int port) {
 int fh;

 if ( mode == MODE_LISTEN )
  return -1;

 if ( (fh = open(host, O_RDONLY, 0644)) == -1 ) {
  ROAR_ERR("roar_socket_open_file(*): Can not open file %s: %s", host, strerror(errno));
 }

 return fh;
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
