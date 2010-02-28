//network.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_SUPPORT_LISTEN

#ifdef ROAR_BROKEN_PEERCRED
#undef SO_PEERCRED
#endif

#ifdef ROAR_HAVE_SELECT
#define _CAN_OPERATE
#endif

int net_check_listen  (void) {
#ifdef _CAN_OPERATE
 int r;
 fd_set sl;
 struct timeval tv;
 int i;
 int max_fh = -1;

 FD_ZERO(&sl);

 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  if ( g_listen[i].socket != -1 ) {
   if ( g_listen[i].socket > max_fh )
    max_fh = g_listen[i].socket;

   FD_SET(g_listen[i].socket, &sl);
  }
 }

 if ( max_fh == -1 )
  return 0;

 tv.tv_sec  = 0;
 tv.tv_usec = 1;

 if ((r = select(max_fh + 1, &sl, NULL, NULL, &tv)) > 0) {
  ROAR_DBG("net_check_listen(void): We have a connection!");
  for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
   if ( g_listen[i].socket != -1 ) {
    if ( FD_ISSET(g_listen[i].socket, &sl) ) {
     if ( net_get_new_client(&(g_listen[i])) == -1 )
      return -1;
    }
   }
  }
 }

 return r;
#else
 return -1;
#endif
}

#ifdef _CAN_OPERATE
int net_get_new_client (struct roard_listen * lsock) {
 int fh;
 int client;
 struct roar_client * c;
#ifdef SO_PEERCRED
 struct ucred cred;
 socklen_t cred_len = sizeof(cred);
#endif
 struct roar_vio_calls    vio;
 struct sockaddr_storage  addr;
 socklen_t                addrlen = sizeof(addr);

 fh = accept(lsock->socket, (struct sockaddr*)&addr, &addrlen);

 ROAR_DBG("net_get_new_client(void): fh = %i", fh);

 client = clients_new();

 if ( clients_set_fh(client, fh) == -1 ) {
  ROAR_ERR("net_get_new_client(void): Can not set client's fh");

  clients_delete(client);
  close(fh);

  ROAR_DBG("net_get_new_client(void) = -1");
  return -1;
 }

 if ( clients_get(client, &c) != -1 ) {
#ifdef SO_PEERCRED
  if (getsockopt(fh, SOL_SOCKET, SO_PEERCRED, &cred, &cred_len) != -1) {
   if ( cred.pid != 0 ) {
    c->pid = cred.pid;
    c->uid = cred.uid;
    c->gid = cred.gid;
   }
  } else {
   ROAR_DBG("req_on_identify(): Can't get creds via SO_PEERCRED: %s", strerror(errno));
  }
#elif defined(ROAR_HAVE_GETPEEREID)
  if (getpeereid(fh, &(c->uid), &(c->gid)) == -1) {
   ROAR_DBG("req_on_identify(): Can't get creds via getpeereid(): %s", strerror(errno));
  }
#endif

  if ( roar_nnode_free(&(c->nnode)) == -1 )
   return -1;

  if ( roar_nnode_new_from_sockaddr(&(c->nnode), (struct sockaddr*)&addr, addrlen) == -1 )
   return -1;
 }

 ROAR_DBG("net_get_new_client(*): proto=0x%.4x", lsock->proto);

 if ( clients_set_proto(client, lsock->proto) == -1 ) {
  ROAR_WARN("net_get_new_client(*): Setting proto(0x%.4x) of client %i failed.", lsock->proto, client);
  return -1;
 }

 switch (lsock->proto) {
  case ROAR_PROTO_ROARAUDIO:
    // nothing needed to be done here
   break;
#ifndef ROAR_WITHOUT_DCOMP_EMUL_ESD
#ifdef ROAR_HAVE_H_ESD
  case ROAR_PROTO_ESOUND:
    ROAR_DBG("net_get_new_client(*): execing ESD CONNECT command");

    if ( roar_vio_open_fh_socket(&vio, fh) == -1 )
     return -1;

    ROAR_DBG("net_get_new_client(*): creating VIO OK");

    if ( emul_esd_exec_command(client, ESD_PROTO_CONNECT, &vio) == -1 )
     return -1;

    ROAR_DBG("net_get_new_client(*): CONNECT execed sucessfully");
   break;
#endif
#endif
#ifndef ROAR_WITHOUT_DCOMP_EMUL_SIMPLE
  case ROAR_PROTO_SIMPLE:
    if ( emul_simple_on_connect(client, lsock) == -1 )
     return -1;
   break;
#endif
  default:
    // OS independiend code to close the socket:
    if ( roar_vio_open_fh_socket(&vio, fh) == -1 )
     return -1;
    roar_vio_close(&vio);
    return -1;
   break;
 }

// close(fh);

 return 0;
}
#endif

#endif

//ll
