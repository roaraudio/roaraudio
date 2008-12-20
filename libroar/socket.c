//socket.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

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
/*
#ifdef SO_PEERCRED
 int opt = 1;
#endif
*/

 fh = socket(AF_UNIX, SOCK_STREAM, 0);

/*
#ifdef SO_PEERCRED
// setsockopt(fh, SOL_SOCKET, SO_PASSCRED, &opt, sizeof(int));
#endif
*/

 return fh;
}

int roar_socket_decnet_set_timeout (int fh, time_t sec, int usec) {
#ifdef ROAR_HAVE_LIBDNET
 struct timeval timeout = {sec, usec};

 return setsockopt(fh, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
#else
 return -1;
#endif
}

int roar_socket_recvbuf(int fh, int len) {
 if ( len < 256 ) len = 256;

 return setsockopt(fh, SOL_SOCKET, SO_RCVBUF, &len, sizeof(len));
}

int roar_socket_new_decnet_seqpacket (void) {
#ifdef ROAR_HAVE_LIBDNET
 int fh;

 fh = socket(AF_DECnet, SOCK_SEQPACKET, DNPROTO_NSP);

 roar_socket_decnet_set_timeout(fh, 300, 0);

 return fh;
#else
 return -1;
#endif
}


int roar_socket_new_decnet_stream (void) {
#ifdef ROAR_HAVE_LIBDNET
 int fh;

 fh = socket(AF_DECnet, SOCK_STREAM, DNPROTO_NSP);

 roar_socket_decnet_set_timeout(fh, 300, 0);

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


#define _SCMR_CONTROLLEN (sizeof(struct cmsghdr) + sizeof(int))
int roar_socket_send_fh (int sock, int fh, char * mes, size_t len) {
 struct iovec     iov[1];
 struct msghdr    msg;
 char             cmptr_buf[_SCMR_CONTROLLEN];
 struct cmsghdr * cmptr = (struct cmsghdr *) cmptr_buf;
 char             localmes[1] = {0};

 ROAR_DBG("roar_socket_send_fh(sock=%i, fh=%i, mes=%p, len=%u) = ?", sock, fh, mes, len);

 if ( sock < 0 || fh < 0 )
  return -1;

 if ( len == 0 ) {
  len = 1;
  mes = localmes;
 }

 memset(cmptr, 0, _SCMR_CONTROLLEN);

 iov[0].iov_base = mes;
 iov[0].iov_len  = len;
 msg.msg_iov     = iov;
 msg.msg_iovlen  = 1;
 msg.msg_name    = NULL;
 msg.msg_namelen = 0;

 cmptr->cmsg_level        = SOL_SOCKET;
 cmptr->cmsg_type         = SCM_RIGHTS;
 cmptr->cmsg_len          = _SCMR_CONTROLLEN;
 msg.msg_control          = (caddr_t) cmptr;
 msg.msg_controllen       = _SCMR_CONTROLLEN;
 *(int *)CMSG_DATA(cmptr) = fh;

 return sendmsg(sock, &msg, 0);
}

int roar_socket_recv_fh (int sock,         char * mes, size_t * len) {
 struct iovec     iov[1];
 struct msghdr    msg;
 char             cmptr_buf[_SCMR_CONTROLLEN];
 struct cmsghdr * cmptr = (struct cmsghdr *) cmptr_buf;
 char             localmes[1];
 size_t           locallen[1] = {1};

 if ( sock < 0 )
  return -1;

 if ( len == NULL ) {
  len = locallen;
  mes = localmes;
 }

 iov[0].iov_base = mes;
 iov[0].iov_len  = *len;
 msg.msg_iov     = iov;
 msg.msg_iovlen  = 1;
 msg.msg_name    = NULL;
 msg.msg_namelen = 0;

 msg.msg_control    = (caddr_t) cmptr;
 msg.msg_controllen = _SCMR_CONTROLLEN;

 if ( (*len = recvmsg(sock, &msg, 0)) == -1 )
  return -1;

 if ( msg.msg_controllen != _SCMR_CONTROLLEN )
  return -1;

 return *(int *)CMSG_DATA(cmptr);
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
#ifdef ROAR_HAVE_IPV6
  struct sockaddr_in6 in6;
#endif
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
 int    proxy_port = -1;
 char   proxy_host[ROAR_SOCKET_MAX_HOSTNAMELEN];
 char * proxy_addr = NULL;
 int    i;
 int    fh = -1;
 char * user = NULL, * pw = NULL, * opts = NULL;
 char * sep;
 int    no_fh = 0;
 char   proxy_addr_buf[1024];
 static struct passwd * passwd;
 int (* code)(int mode, int fh, char * host, int port, char * user, char * pw, char * opts) = NULL;

 if ( passwd == NULL ) {
  passwd = getpwuid(getuid());
 }

 if ( passwd != NULL )
  user = passwd->pw_name;

 if ( user == NULL )
  user = getenv("USER");

 // TODO: change this so we support listen() proxys (ssh -R)
 if ( mode != MODE_CONNECT )
  return -1;

 if ( !strncmp(proxy_type, "socks", 5) ) {
  proxy_addr = getenv("socks_proxy");

  proxy_port = 9050; // TOR's default port
 } else if ( !strcmp(proxy_type, "http") || !strcmp(proxy_type, "https") ) {
  proxy_port = 8080;

  if ( (proxy_addr = getenv("http_proxy")) == NULL )
   proxy_addr = getenv("https_proxy");

  if ( proxy_addr == NULL )
   return -1;

  if ( !strncmp(proxy_addr, "http://", 7) )
   proxy_addr += 7;
 } else if ( !strncmp(proxy_type, "ssh", 3) ) {
  proxy_port = 22;
  proxy_addr = getenv("ssh_proxy");
  no_fh      = 1;
 }

 proxy_addr_buf[1023] = 0;
 strncpy(proxy_addr_buf, proxy_addr, 1023);
 proxy_addr = proxy_addr_buf;

 if ( (sep = strstr(proxy_type, "/")) != NULL )
  opts = sep+1;

 if ( proxy_addr == NULL )
  return -1;

 if ( (sep = strstr(proxy_addr, "@")) != NULL ) {
  *sep = 0;
  user = proxy_addr;
  proxy_addr = sep+1;

  if ( (sep = strstr(user, ":")) != NULL ) {
   *sep = 0;
   pw = sep+1;
  }
 }

 ROAR_DBG("roar_socket_open_proxy(*): proxy_type='%s', opts='%s', user='%s', pw=(not shown), proxy_addr='%s'", proxy_type, opts, user, proxy_addr);

 for (i = 0; proxy_addr[i] != 0 && proxy_addr[i] != ':' && i < ROAR_SOCKET_MAX_HOSTNAMELEN; i++)
  proxy_host[i] = proxy_addr[i];
 proxy_host[i] = 0;

 if ( i == 0 ) // no hostname found
  return -1;

 if ( proxy_addr[i] == ':' )
  proxy_port = atoi(&proxy_addr[i+1]);

 if ( ! no_fh ) {
  if ( (fh = roar_socket_open(mode, type, proxy_host, proxy_port)) == -1) {
   return -1;
  }
 }

 if ( !strcmp(proxy_type, "socks4a") ) { // for TOR, the only supported type at the moment
  code = roar_socket_open_socks4a;
 } else if ( !strcmp(proxy_type, "socks4d") ) { // DECnet
  code = roar_socket_open_socks4d;
 } else if ( !strcmp(proxy_type, "socks4") ) { // good old SOCKS4
  code = roar_socket_open_socks4;
 } else if ( !strcmp(proxy_type, "http") ) { // HTTP CONNECT
  code = roar_socket_open_http;
 } else if ( !strncmp(proxy_type, "ssh", 3) ) { // SSH...
  code = roar_socket_open_ssh;
 } else {
  return -1; // unknown type
 }

 if ( code != NULL ) {
  if ( no_fh ) {
   fh = code(mode, fh, host, port, user, pw, opts);
  } else {
   if ( code(mode, fh, host, port, user, pw, opts) == -1 ) {
    close(fh);
    return -1;
   }
  }

  return fh;
 }

 close(fh);
 return -1;
}

// protocoll dependet proxy code:

int roar_socket_open_socks4 (int mode, int fh, char * host, int port, char * user, char * pw, char * opts) {
 struct hostent     * he;

 if ( (he = gethostbyname(host)) == NULL ) {
  ROAR_ERR("roar_socket_open_socks4(*): Can\'t resolve host name '%s'", host);
  return -1;
 }

 return roar_socket_open_socks4x(mode, fh, he->h_addr, port, NULL, 0, user);
}

int roar_socket_open_socks4a(int mode, int fh, char * host, int port, char * user, char * pw, char * opts) {
 return roar_socket_open_socks4x(mode, fh, "\0\0\0\1", port, host, strlen(host)+1, user);
}

int roar_socket_open_socks4d(int mode, int fh, char * host, int port, char * user, char * pw, char * opts) {
 size_t len = strlen(host)+1;
 char * dp;

 if ( port == 0 ) {
  if ( (dp = strstr(host, "::")) == NULL )
   return -1;

  len--;
  *dp = 0;
  memmove(dp+1, dp+2, len - (dp-host) - 1);
 }

 return roar_socket_open_socks4x(mode, fh, "\0\2\0\0", port, host, len, user);
}

int roar_socket_open_socks4x(int mode, int fh, char host[4], int port, char * app, size_t app_len, char * user) {
 char buf[9];
 int len;

 buf[0] = 0x04;
 buf[1] = mode == MODE_CONNECT ? 0x01 : 0x02;
 *((uint16_t*)&buf[2]) = htons(port);
 memcpy(buf+4, host, 4);

 if ( user == NULL ) {
  buf[8] = 0x00;
  len = 9;
 } else {
  len = 8;
 }

 if ( write(fh, buf, len) != len )
  return -1;

 if ( user != NULL ) {
  len = strlen(user) + 1;
  if ( write(fh, user, len) != len )
   return -1;
 }

 if ( app_len > 0 )
  if ( write(fh, app, app_len) != app_len )
   return -1;

 if ( read(fh, buf, 8) != 8 )
  return -1;

 if ( buf[1] != 0x5a )
  return -1;

 return 0;
}

int roar_socket_open_http   (int mode, int fh, char * host, int port, char * user, char * pw, char * opts) {
 char buf[1024];
 int len;

 if ( port == 0 || host == NULL )
  return -1;

 if ( *host == '/' ) // AF_UNIX
  return -1;

 if ( (len = snprintf(buf, 1024, "CONNECT %s:%i HTTP/1.0\r\nUser-Agent: libroar\r\n\r\n", host, port)) == -1 )
  return -1;

 if ( write(fh, buf, len) != len )
  return -1;

 while ( (len = read(fh, buf, 1024)) ) {
  if ( len == 1024 ) { // overlong lion
   return -1;
  } else if ( len == 2 && buf[0] == '\r' && buf[1] == '\n' ) {
   break;
  } else if ( len == 1 && (buf[0] == '\r' || buf[0] == '\n') ) { // bad proxy or devel trying to debug ;)
   break;
  } else if ( len >= 4 && buf[len-4] == '\r' && buf[len-3] == '\n' && buf[len-2] == '\r' && buf[len-1] == '\n' ) {
   break;
  }
 }

 return 0;
}


int roar_socket_open_ssh    (int mode, int fh, char * host, int port, char * user, char * pw, char * opts) {
 char * proxy_addr = getenv("ssh_proxy");
 char * sep;
 char   cmd[1024] = {0}, rcmd[1024] = {0};
 int    proxy_port = 22;
 int    use_socat = 0;
 int r;
 int socks[2];

 if ( host == NULL )
  return -1;

 if ( *host == '/' )
  use_socat = 1;

 if ( mode == MODE_LISTEN )
  return -1;

 if ( proxy_addr == NULL )
  return -1;

 if ( opts != NULL ) {
  if ( !strcmp(opts, "socat") ) {
   use_socat = 1;
  } else if ( !strcmp(opts, "netcat") ) {
   use_socat = 0;
  } else {
   return -1;
  }
 }

 ROAR_DBG("roar_socket_open_ssh(*): proxy_addr='%s'", proxy_addr);

 if ( (sep = strstr(proxy_addr, "@")) != NULL )
  proxy_addr = sep+1;

 if ( (sep = strstr(proxy_addr, ":")) != NULL ) {
  *sep = 0;
  proxy_port = atoi(sep+1);
 }


 if ( !strcmp(host, "+fork") ) {
  strcpy(rcmd, "roard --no-listen --client-fh 0");
 } else {
  if ( use_socat ) {
   if ( *host == '/' ) {
    snprintf(rcmd, 1023, "socat stdio unix-connect:\"%s\"", host);
   } else {
    snprintf(rcmd, 1023, "socat stdio tcp:\"%s\":%i", host, port);
   }
  } else {
   snprintf(rcmd, 1023, "$(which netcat nc 2> /dev/null | grep -v \" \" | head -n 1) \"%s\" %i", host, port);
  }

  rcmd[1023] = 0;
 }

 ROAR_DBG("roar_socket_open_ssh(*): proxy_port=%i, user='%s', proxy_addr='%s'", proxy_port, user, proxy_addr);
 snprintf(cmd, 1023, "ssh -p %i -l '%s' '%s' '%s'", proxy_port, user, proxy_addr, rcmd);
 cmd[1023] = 0;


 if ( socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1 ) {
  return -1;
 }

 r = fork();

 if ( r == -1 ) { // error!
  ROAR_ERR("roar_socket_open_ssh(*): Can not fork: %s", strerror(errno));
  close(socks[0]);
  close(socks[1]);
  return -1;
 } else if ( r == 0 ) { // we are the child
  close(socks[0]);

  close(ROAR_STDIN ); // we do not want roard to have any standard input
  close(ROAR_STDOUT); // STDOUT is also not needed, so we close it,
                      // but STDERR we keep open for error messages.

  dup2(socks[1], 0);
  dup2(socks[1], 1);

  execlp("sh", "sh", "-c", cmd, NULL);

  // we are still alive?
  ROAR_ERR("roar_socket_open_ssh(*): alive after exec(), that's bad!");
  _exit(1);
 } else { // we are the parent
  close(socks[1]);
  return socks[0];
 }
 return -1;
}

//ll
