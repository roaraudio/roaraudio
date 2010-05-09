//vio_socket.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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

#ifdef ROAR_HAVE_BSDSOCKETS
#define _CAN_OPERATE
#endif

int     roar_vio_open_def_socket          (struct roar_vio_calls * calls, struct roar_vio_defaults * def, char * opts) {
#ifdef _CAN_OPERATE
 int       fh  = -1;
 socklen_t len =  0;
 int       listening  = 0;
 int       one_client = 0;
 int       client;

 if ( calls == NULL || def == NULL )
  return -1;

 if ( opts != NULL ) {
  if ( strstr(opts, "listen") != NULL ) {
   listening  = 1;
   one_client = 1;
  }
 }

 if ( def->type != ROAR_VIO_DEF_TYPE_SOCKET )
  return -1;

 switch (def->d.socket.domain) {
#ifdef ROAR_HAVE_IPV4
  case AF_INET:
    len = sizeof(struct sockaddr_in);

    if ( roar_vio_socket_init_inet4host_def(def) == -1 )
     return -1;

    switch (def->d.socket.type) {
     case SOCK_STREAM:
       fh = roar_socket_new_tcp();
      break;
     case SOCK_DGRAM:
       fh = roar_socket_new_udp();
      break;
     default:
       return -1;
    }
   break;
#endif
#ifdef ROAR_HAVE_UNIX
  case AF_UNIX:
    len = sizeof(struct sockaddr_un);

    switch (def->d.socket.type) {
     case SOCK_STREAM:
       fh = roar_socket_new_unix();
      break;
     case SOCK_DGRAM:
       return -1;
      break;
     default:
       return -1;
    }
   break;
#endif
#ifdef ROAR_HAVE_LIBDNET
  case AF_DECnet:
    len = sizeof(struct sockaddr_dn);

    if ( roar_vio_socket_init_decnetnode_def(def) == -1 )
     return -1;

    switch (def->d.socket.type) {
     case SOCK_STREAM:
       fh = roar_socket_new_decnet_stream();
      break;
     default:
       return -1;
    }
   break;
#endif
#ifdef ROAR_HAVE_IPV6
  case AF_INET6:
    len = sizeof(struct sockaddr_in6);

    switch (def->d.socket.type) {
     case SOCK_STREAM:
       fh = roar_socket_new_tcp6();
      break;
     case SOCK_DGRAM:
       fh = roar_socket_new_udp6();
      break;
     default:
       return -1;
    }
   break;
#endif
#ifdef ROAR_HAVE_IPX
  case AF_IPX:
    len = sizeof(struct sockaddr_ipx);

    return -1;
   break;
#endif
  default:
    return -1;
 }

 if ( fh == -1 )
  return -1;

 if ( listening ) {
  if ( bind(fh, &(def->d.socket.sa.sa), len) == -1 ) {
   close(fh);
   return -1;
  }

  if ( listen(fh, one_client ? 1 : 16) == -1 ) {
   close(fh);
   return -1;
  }

  if ( one_client ) {
   client = accept(fh, NULL, NULL);
   close(fh);

   if ( client == -1 ) {
    return -1;
   }

   fh = client;
  }
 } else {
  if ( connect(fh, &(def->d.socket.sa.sa), len) == -1 ) {
   close(fh);
   return -1;
  }
 }

 if ( roar_vio_open_fh_socket(calls, fh) == -1 ) {
  close(fh);
  return -1;
 }

 // there is no problem if the shutdown()s fail.
 // some socket domains don't support unidirectional connections
 // this just free()s some kernel buffers :)
 switch (def->o_flags & (O_RDONLY|O_WRONLY|O_RDWR)) {
  case O_RDONLY:
    ROAR_SHUTDOWN(fh, SHUT_WR);
   break;
  case O_WRONLY:
    ROAR_SHUTDOWN(fh, SHUT_RD);
   break;
 }

 return 0;
#else
 return -1;
#endif
}

int     roar_vio_socket_init_socket_def   (struct roar_vio_defaults * def, int domain, int type) {
#ifdef _CAN_OPERATE
 if ( def == NULL || domain == -1 || type == -1 )
  return -1;

 // we do not memset(def, 0, sizeof(...)) here
 // because this is allready done in roar_vio_dstr_init_defaults()
 // if we would be would override o_flags/o_mode and maybe others

 memset(&(def->d.socket.sa), 0, sizeof(def->d.socket.sa));

 def->type                     = ROAR_VIO_DEF_TYPE_SOCKET;
 def->d.socket.domain          = domain;
 def->d.socket.type            = type;
 def->d.socket.sa.sa.sa_family = domain;

 return 0;
#else
 return -1;
#endif
}

int     roar_vio_socket_init_dstr_def     (struct roar_vio_defaults * def, char * dstr, int hint, int type,
                                           struct roar_vio_defaults * odef) {
#ifdef _CAN_OPERATE
 char * host;
#if defined(ROAR_HAVE_IPV4) || defined(ROAR_HAVE_IPV6)
 int    port;
#endif
#if defined(ROAR_HAVE_IPV4)
 int ret;
#endif

 if ( def == NULL )
  return -1;

 if ( dstr == NULL && odef == NULL )
  return -1;

 if ( dstr == NULL )
  dstr = "";

 ROAR_DBG("roar_vio_socket_init_dstr_def(def=%p, dstr='%s', hint=%i, type=%i, odef=%p) = ?", def, dstr, hint, type, odef);

 if ( hint == -1 ) {
  if ( 0 ) { // this is needed to keep the syntx ok, compiler will throw it away
#ifdef ROAR_HAVE_IPV6
  } else if ( strstr(dstr, "[") != NULL ) { // [ip]:service
   hint = AF_INET6;
#endif
#ifdef ROAR_HAVE_LIBDNET
  } else if ( strstr(dstr, "::") != NULL ) { // node::object
   hint = AF_DECnet;
#endif
#ifdef ROAR_HAVE_IPX
  } else if ( strstr(dstr, "(") != NULL ) { // net:mac(service)
   hint = AF_IPX;
#endif
#ifdef ROAR_HAVE_UNIX
  } else if ( strstr(dstr, "/") != NULL ) { // /path/to/sock
   hint = AF_UNIX;
#endif
#ifdef ROAR_HAVE_IPV4
  } else if ( strstr(dstr, ":") != NULL ) { // host:port
   hint = AF_INET;
#endif
  }
 }

 if ( hint == -1 && odef != NULL ) { // if we still don't know what this is we try
                                     // to use the parent objects request
  ROAR_DBG("roar_vio_socket_init_dstr_def(*): hint=-1 && odef!=NULL");
  if ( odef->type == ROAR_VIO_DEF_TYPE_SOCKET ) {
   ROAR_DBG("roar_vio_socket_init_dstr_def(*): hint=-1 && odef!=NULL, using hint from odef");
   hint = odef->d.socket.domain;
  }
 }

 if ( hint == -1 ) /* we really have no glue what this is... */
  return -1;

#ifdef ROAR_HAVE_UNIX
 if ( hint == AF_UNIX ) {
  if ( *dstr != 0 && strcmp(dstr, "//") != 0 ) {
   return roar_vio_socket_init_unix_def(def, dstr);
  } else {
   if ( roar_vio_socket_conv_def(odef, AF_UNIX) == -1 )
    return -1;

   return roar_vio_socket_init_unix_def(def, odef->d.socket.sa.un.sun_path);
  }
 }
#endif

 ROAR_DBG("roar_vio_socket_init_dstr_def(*) = ?");

 if ( *dstr == 0 ) {
  if ( roar_vio_socket_conv_def(odef, hint) == -1 )
   return -1;

  if ( odef->d.socket.type != type )
   return -1;

  if ( def != odef )
   memcpy(def, odef, sizeof(struct roar_vio_defaults));

  return 0;
 }

 for (; *dstr == '/'; dstr++);

 ROAR_DBG("roar_vio_socket_init_dstr_def(*) = ?");

 switch (hint) {
#ifdef ROAR_HAVE_IPV4
  case AF_INET:
    host = dstr;
    for (; *dstr != 0 && *dstr != ':'; dstr++);

    if ( *dstr == ':' ) { // we have a port :)
     *dstr++ = 0;
     if ( (port = roar_vio_socket_get_port(dstr, AF_INET, type)) == -1 )
      return -1;

     ret = roar_vio_socket_init_inet4_def(def, host, port, type);

     *(dstr-1) = ':';

     return ret;
    } else {
     if ( roar_vio_socket_conv_def(odef, AF_INET) == -1 )
      return -1;

     return roar_vio_socket_init_inet4_def(def, host, ROAR_NET2HOST16(odef->d.socket.sa.in.sin_port), type);
    }
   break;
#endif
#ifdef ROAR_HAVE_LIBDNET
  case AF_DECnet:
    ROAR_DBG("roar_vio_socket_init_dstr_def(*) = ?");
    host = dstr;

    if ( type != SOCK_STREAM )
     return -1;

    if ( (dstr = strstr(dstr, "::")) == NULL ) {
     if ( roar_vio_socket_conv_def(odef, AF_DECnet) == -1 )
      return -1;

     return -1;
//     return roar_vio_socket_init_decnet_def(def, host, -1, dstr);
    } else {
     *dstr  = 0;
      dstr += 2;
     return roar_vio_socket_init_decnet_def(def, host, -1, dstr);
    }
   break;
#endif
#ifdef ROAR_HAVE_IPV6
  case AF_INET6:
    return -1;
   break;
#endif
#ifdef ROAR_HAVE_IPX
  case AF_IPX:
    return -1;
   break;
#endif
  default:
    return -1;
 }

 return 0;
#else
 return -1;
#endif
}

#ifdef _CAN_OPERATE
int     roar_vio_socket_conv_def          (struct roar_vio_defaults * def, int domain) {
 if ( def == NULL || domain == -1 )
  return -1;

#ifdef ROAR_HAVE_UNIX
 if ( domain == AF_UNIX ) {
  if ( def->type == ROAR_VIO_DEF_TYPE_SOCKET ) {
   if ( def->d.socket.domain == AF_UNIX )
    return 0;

   return -1;
  } else {
   if ( def->type == ROAR_VIO_DEF_TYPE_FILE )
    return roar_vio_socket_init_unix_def(def, def->d.file);

   return -1;
  }
 }
#endif

 if ( def->type != ROAR_VIO_DEF_TYPE_SOCKET )
  return -1;

 if ( def->d.socket.domain == domain )
  return 0;

 // we sould add support to convert IPv4 <-> IPv6 here

 return -1;
}

int     roar_vio_socket_get_port          (char * service, int domain, int type) {
#ifdef ROAR_HAVE_GETSERVBYNAME
 struct servent * serv  = NULL;
#endif
 char           * proto = NULL;
 int              port;
 char           * ts;

 if ( service == NULL || domain == -1 || type == -1 )
  return -1;

 if ( (ts = strstr(service, "/")) != NULL )
  *ts = 0;

 if ( sscanf(service, "%i", &port) == 1 )
  return port;

 if ( ts != NULL )
  *ts = '/';

 switch (domain) {
#ifdef ROAR_HAVE_IPV6
  case AF_INET6:
#endif
#ifdef ROAR_HAVE_IPV4
  case AF_INET:
#endif
#if defined(ROAR_HAVE_IPV6) || defined(ROAR_HAVE_IPV4)
    switch (type) {
     case SOCK_STREAM: proto = "tcp"; break;
     case SOCK_DGRAM:  proto = "udp"; break;
     default:
      return -1;
    }
   break;
#endif
#ifdef ROAR_HAVE_LIBDNET
  case AF_DECnet:
#ifdef ROAR_HAVE_GETOBJECTBYNAME
    return getobjectbyname(service);
#else
    if ( !strcmp(service, "roar") )
     return 0;

    return -1;
#endif
   break;
#endif
  default:
    return -1;
 }

#ifdef ROAR_HAVE_GETSERVBYNAME
 if ( ts != NULL )
  *ts = 0;

 if ( (serv = getservbyname(service, proto)) == NULL ) {
  ROAR_ERR("roar_vio_socket_get_port(*): Unknown service: %s/%s: %s", service, proto, strerror(errno));

  if ( ts != NULL )
   *ts = '/';

  return -1;
 }

 if ( ts != NULL )
  *ts = '/';

 return ROAR_NET2HOST16(serv->s_port);
#endif

 return -1;
}
#endif

// AF_UNIX:
int     roar_vio_socket_init_unix_def     (struct roar_vio_defaults * def, char * path) {
#if defined(ROAR_HAVE_UNIX) && defined(_CAN_OPERATE)
 if ( def == NULL || path == NULL )
  return -1;

 if ( roar_vio_socket_init_socket_def(def, AF_UNIX, SOCK_STREAM) == -1 )
  return -1;

 strncpy(def->d.socket.sa.un.sun_path, path, sizeof(def->d.socket.sa.un.sun_path) - 1);

 return 0;
#else
 return -1;
#endif
}

// AF_DECnet:
int     roar_vio_socket_init_decnetnode_def(struct roar_vio_defaults * def) {
#if defined(ROAR_HAVE_LIBDNET) && defined(_CAN_OPERATE)
 char               * node;
 char               * ed;
 struct nodeent     * ne;

 if ( def == NULL )
  return -1;

 if ( (node = def->d.socket.host) == NULL )
  return -1;

 if ( (ed = strstr(node, "/")) != NULL )
  *ed = 0;

 if ( (ne = getnodebyname(node)) == NULL ) {
  ROAR_ERR("roar_vio_socket_init_decnetnode_def(*): Can\'t resolve node name '%s'", node);
  if ( ed != NULL ) *ed = '/';
  return -1;
 }

 memcpy(&(def->d.socket.sa.dn.sdn_add.a_addr), ne->n_addr, 2);

 if ( ed != NULL ) *ed = '/';


 return 0;
#else
 return -1;
#endif
}

int     roar_vio_socket_init_decnet_def   (struct roar_vio_defaults * def, char * node, int object, char * objname) {
#if defined(ROAR_HAVE_LIBDNET) && defined(_CAN_OPERATE)
 struct sockaddr_dn * dn;

 if ( def == NULL )
  return -1;

 if ( object < 1 && objname == NULL )
  return -1;

 if ( object == -1 )
  object = roar_vio_socket_get_port(objname, AF_DECnet, SOCK_STREAM);

 if ( object == -1 ) {
  if ( objname == NULL ) {
   return -1;
  } else {
   object = 0;
  }
 }

 if ( roar_vio_socket_init_socket_def(def, AF_DECnet, SOCK_STREAM) == -1 )
  return -1;

 def->d.socket.host = node;
 dn                 = &(def->d.socket.sa.dn);
 dn->sdn_flags      = 0;
 dn->sdn_objnum     = object;
 dn->sdn_nodeaddrl  = 2;

 if ( objname == NULL ) {
  dn->sdn_objnamel   = 0;
 } else {
  dn->sdn_objnamel   = strlen(objname);
  if ( dn->sdn_objnamel > DN_MAXOBJL )
   dn->sdn_objnamel  = DN_MAXOBJL;

  memcpy(&(dn->sdn_objname), objname, dn->sdn_objnamel);
 }

 return 0;
#else
 return -1;
#endif
}


// AF_INET:
int     roar_vio_socket_init_inet4host_def(struct roar_vio_defaults * def) {
#if defined(ROAR_HAVE_IPV4) && defined(_CAN_OPERATE)
 struct hostent     * he;
 char               * ed, * pd;

 if ( def == NULL )
  return -1;

 if ( def->d.socket.host == NULL )
  return -1;

 if ( (ed = strstr(def->d.socket.host, "/")) != NULL )
  *ed = 0;

 if ( (pd = strstr(def->d.socket.host, ":")) != NULL )
  *pd = 0;

 if ( (he = gethostbyname(def->d.socket.host)) == NULL ) {
  ROAR_ERR("roar_vio_socket_init_inet4host_def(*): Can\'t resolve host name '%s'",
                    def->d.socket.host);
  if ( ed != NULL ) *ed = '/';
  return -1;
 }

 if ( pd != NULL ) *pd = ':';

 if ( ed != NULL ) *ed = '/';

 memcpy((struct in_addr *)&def->d.socket.sa.in.sin_addr, he->h_addr, sizeof(struct in_addr));

 return 0;
#else
 return -1;
#endif
}

int     roar_vio_socket_init_inet4_def    (struct roar_vio_defaults * def, char * host, int port, int type) {
#if defined(ROAR_HAVE_IPV4) && defined(_CAN_OPERATE)
 if ( roar_vio_socket_init_socket_def(def, AF_INET, type) == -1 )
  return -1;

 def->d.socket.host             = host;

 def->d.socket.sa.in.sin_port   = ROAR_HOST2NET16(port);

 return 0;
#else
 return -1;
#endif
}

int     roar_vio_socket_init_tcp4_def     (struct roar_vio_defaults * def, char * host, int port) {
 return roar_vio_socket_init_inet4_def(def, host, port, SOCK_STREAM);
}

int     roar_vio_socket_init_udp4_def     (struct roar_vio_defaults * def, char * host, int port) {
 return roar_vio_socket_init_inet4_def(def, host, port, SOCK_DGRAM);
}


// AF_INET6:
int     roar_vio_socket_init_inet6host_def(struct roar_vio_defaults * def);
int     roar_vio_socket_init_inet6_def    (struct roar_vio_defaults * def, char * host, int port, int type) {
 return -1;
}

int     roar_vio_socket_init_tcp6_def     (struct roar_vio_defaults * def, char * host, int port) {
 return roar_vio_socket_init_inet6_def(def, host, port, SOCK_STREAM);
}

int     roar_vio_socket_init_udp6_def     (struct roar_vio_defaults * def, char * host, int port) {
 return roar_vio_socket_init_inet6_def(def, host, port, SOCK_DGRAM);
}

//ll
