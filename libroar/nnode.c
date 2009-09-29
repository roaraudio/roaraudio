//nnode.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#define _CHECK(x) if ( (x) == NULL ) return -1

int roar_nnode_new        (struct roar_nnode * nnode, int socktype) {
 _CHECK(nnode);

 memset(nnode, 0, sizeof(struct roar_nnode));

 nnode->socktype = socktype;

 return 0;
}

int roar_nnode_new_from_af(struct roar_nnode * nnode, int af) {
 int socktype;

 _CHECK(nnode);

 switch (af) {
#ifdef ROAR_HAVE_IPV4
  case AF_INET:   socktype = ROAR_SOCKET_TYPE_INET;   break;
#endif
#ifdef ROAR_HAVE_IPV6
  case AF_INET6:  socktype = ROAR_SOCKET_TYPE_INET6;  break;
#endif
#ifdef ROAR_HAVE_UNIX
  case AF_UNIX:   socktype = ROAR_SOCKET_TYPE_UNIX;   break;
#endif
#ifdef ROAR_HAVE_LIBDNET
  case AF_DECnet: socktype = ROAR_SOCKET_TYPE_DECNET; break;
#endif
#ifdef ROAR_HAVE_IPX
  case AF_IPX:    socktype = ROAR_SOCKET_TYPE_IPX;    break;
#endif
  default:
    return -1;
   break;
 }

 if ( roar_nnode_new(nnode, socktype) == -1 )
  return -1;

 return 0;
}

int roar_nnode_new_from_sockaddr(struct roar_nnode * nnode, struct sockaddr * addr, socklen_t len) {
 _CHECK(nnode);
 _CHECK(addr);

 ROAR_DBG("roar_nnode_new_from_sockaddr(nnode=%p, addr=%p, len=%lu) = ?", nnode, addr, (unsigned long)len);

 if ( len < sizeof(addr->sa_family) )
  return -1;

 ROAR_DBG("roar_nnode_new_from_sockaddr(nnode=%p, addr=%p, len=%lu) = ?", nnode, addr, (unsigned long)len);

 if ( roar_nnode_new_from_af(nnode, addr->sa_family) == -1 )
  return -1;

 ROAR_DBG("roar_nnode_new_from_sockaddr(nnode=%p, addr=%p, len=%lu) = ?", nnode, addr, (unsigned long)len);

 switch (addr->sa_family) {
#ifdef ROAR_HAVE_IPV4
  case AF_INET:
    if ( len < sizeof(struct sockaddr_in) )
     return -1;

    memcpy(nnode->addr.inet4, &(((struct sockaddr_in*)addr)->sin_addr), sizeof(nnode->addr.inet4));
   break;
#endif
#ifdef ROAR_HAVE_IPV6
  case AF_INET6:
    if ( len < sizeof(struct sockaddr_in6) )
     return -1;

    memcpy(nnode->addr.inet6, ((struct sockaddr_in6*)addr)->sin6_addr.s6_addr, sizeof(nnode->addr.inet6));
   break;
#endif
#ifdef ROAR_HAVE_UNIX
  case AF_UNIX:
    // nothing to do/save here.
   break;
#endif
#ifdef ROAR_HAVE_LIBDNET
  case AF_DECnet:
    if ( len < sizeof(struct sockaddr_dn) )
     return -1;

    nnode->addr.decnet.node  =  ((struct sockaddr_dn*)addr)->sdn_add.a_addr[0];
    nnode->addr.decnet.node |= (((struct sockaddr_dn*)addr)->sdn_add.a_addr[1] & 0x03) << 8;
    nnode->addr.decnet.area  =  ((struct sockaddr_dn*)addr)->sdn_add.a_addr[1] >> 2;
   break;
#endif
#ifdef ROAR_HAVE_IPX
  case AF_IPX:
    return -1;
   break;
#endif
  default:
    return -1;
   break;
 }

 ROAR_DBG("roar_nnode_new_from_sockaddr(nnode=%p, addr=%p, len=%lu) = 0", nnode, addr, (unsigned long)len);
 return 0;
}

int roar_nnode_free       (struct roar_nnode * nnode) {
 _CHECK(nnode);

 return 0;
}

int roar_nnode_get_socktype (struct roar_nnode * nnode) {
 _CHECK(nnode);

 return nnode->socktype;
}

int roar_nnode_to_str     (struct roar_nnode * nnode, char * str, size_t len) {
 // Format: PROTO: nodename[nodeaddr]
 char proto[7]    = "???";
 char nodename[4] = "";
 char nodeaddr[48 /* IPv6 */] = "???";

 _CHECK(nnode);

 if ( len == 0 )
  return 0;

 _CHECK(str);

 if ( len == 1 ) {
  *str = 0;
  return 0;
 }

 switch (nnode->socktype) {
  case ROAR_SOCKET_TYPE_UNIX:
    strcpy(proto,     "UNIX");
    strcpy(nodeaddr, "LOCAL");
   break;
  case ROAR_SOCKET_TYPE_INET:
    strcpy(proto,     "IPv4");
    sprintf(nodeaddr, "%i.%i.%i.%i",
                 nnode->addr.inet4[0], nnode->addr.inet4[1],
                 nnode->addr.inet4[2], nnode->addr.inet4[3]
           );
   break;
  case ROAR_SOCKET_TYPE_DECNET:
    strcpy(proto,     "DECnet");
    sprintf(nodeaddr, "%i.%i", nnode->addr.decnet.area, nnode->addr.decnet.node);
   break;
  case ROAR_SOCKET_TYPE_INET6:
    strcpy(proto,     "IPv6");
   break;
  case ROAR_SOCKET_TYPE_IPX:
    strcpy(proto,     "IPX");
   break;
  default:
    sprintf(proto, "P#%i", nnode->socktype);
   break;
 }

 if ( *nodename ) {
  snprintf(str, len, "%s: %s[%s]", proto, nodename, nodeaddr);
 } else {
  snprintf(str, len, "%s: %s", proto, nodeaddr);
 }

 str[len-1] = 0;

 return 0;
}

int roar_nnode_from_blob  (struct roar_nnode * nnode, void * blob, size_t * len) {
 uint16_t socktype;

 _CHECK(nnode);
 _CHECK(blob);
 _CHECK(len);

 if ( *len < 2 )
  return -1;

 memcpy(&socktype, blob, 2);

 socktype = ROAR_NET2HOST16(socktype);

 if ( roar_nnode_new(nnode, socktype) == -1 )
  return -1;

 switch (socktype) {
  case ROAR_SOCKET_TYPE_UNIX:
    *len = 2;
   break;
  case ROAR_SOCKET_TYPE_INET:
    if ( *len < 6 )
     return -1;

    memcpy(nnode->addr.inet4, blob+2, 4);

    *len = 6;
   break;
  case ROAR_SOCKET_TYPE_DECNET:
    if ( *len < 4 )
     return -1;

    nnode->addr.decnet.node   =  ((unsigned char*)blob)[2];
    nnode->addr.decnet.node   = (((unsigned char*)blob)[3] & 0x03) << 8;
    nnode->addr.decnet.area   =  ((unsigned char*)blob)[3] >> 2;

    *len = 4;
   break;
  case ROAR_SOCKET_TYPE_INET6:
    if ( *len < 18 )
     return -1;

    memcpy(nnode->addr.inet4, blob+2, 16);

    *len = 18;
   break;
  case ROAR_SOCKET_TYPE_IPX:
  default:
    return -1;
   break;
 }

 return 0;
}

int roar_nnode_to_blob    (struct roar_nnode * nnode, void * blob, size_t * len) {
 uint16_t socktype_net;

 _CHECK(nnode);
 _CHECK(blob);
 _CHECK(len);

 if ( *len < 2 )
  return -1;

 socktype_net = ROAR_HOST2NET16(nnode->socktype);

 memcpy(blob, &socktype_net, 2);

 switch (nnode->socktype) {
  case ROAR_SOCKET_TYPE_UNIX:
    // all UNIX sockets allways match.
    *len = 2;
   break;
  case ROAR_SOCKET_TYPE_INET:
    if ( *len < 6 )
     return -1;

    memcpy(blob+2, nnode->addr.inet4, 4);
    *len = 6;

   break;
  case ROAR_SOCKET_TYPE_DECNET:
    if ( *len < 4 )
     return -1;

    ((unsigned char*)blob)[2] =   nnode->addr.decnet.node & 0x00FF;
    ((unsigned char*)blob)[3] = ((nnode->addr.decnet.node & 0x0300) >> 8) | (nnode->addr.decnet.area << 2);
    *len = 4;

    return -1;
   break;
  case ROAR_SOCKET_TYPE_INET6:
    if ( *len < 18 )
     return -1;

    memcpy(blob+2, nnode->addr.inet4, 16);
    *len = 18;

   break;
  case ROAR_SOCKET_TYPE_IPX:
  default:
    return -1;
   break;
 }

 return 0;
}

int roar_nnode_cmp        (struct roar_nnode * n0, struct roar_nnode * n1) {
 _CHECK(n0);
 _CHECK(n1);

 if ( n0->socktype != n1->socktype )
  return 1;

 switch (n0->socktype) {
  case ROAR_SOCKET_TYPE_UNIX:
    // all UNIX sockets allways match.
   break;
  case ROAR_SOCKET_TYPE_INET:
    if ( memcmp(n0->addr.inet4, n1->addr.inet4, sizeof(n0->addr.inet4)) != 0 )
     return 1;
   break;
  case ROAR_SOCKET_TYPE_DECNET:
    if ( n0->addr.decnet.area != n1->addr.decnet.area ||
         n0->addr.decnet.node != n1->addr.decnet.node   )
     return 1;
   break;
  case ROAR_SOCKET_TYPE_INET6:
    if ( memcmp(n0->addr.inet6, n1->addr.inet6, sizeof(n0->addr.inet6)) != 0 )
     return 1;
   break;
  case ROAR_SOCKET_TYPE_IPX:
  default:
    return -1;
   break;
 }

 return 0;
}

//ll
