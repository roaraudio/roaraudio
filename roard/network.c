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
 struct roar_vio_selecttv rtv;
 struct roar_vio_select   sv[ROAR_MAX_LISTEN_SOCKETS];
 size_t num = 0;
 ssize_t ret;
 int i;

 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  if ( g_listen[i].used ) {
   ROAR_VIO_SELECT_SETVIO(&(sv[num]), &(g_listen[i].sock), ROAR_VIO_SELECT_READ);
   sv[num].ud.si = i;
   num++;
  }
 }

 if ( num == 0 )
  return 0;

 rtv.sec  = 0;
 rtv.nsec = 1000;

 if ( (ret = roar_vio_select(sv, num, &rtv, NULL)) > 0 ) {
  for (i = 0; i < num; i++) {
   if ( sv[i].eventsa & ROAR_VIO_SELECT_READ ) {
    if ( net_get_new_client(&(g_listen[sv[i].ud.si])) == -1 )
     return -1;
   }
  }
 }

 return ret;
}

int net_get_new_client (struct roard_listen * lsock) {
 int fh;
 int client;
 int socket;
 struct roar_client * c;
 struct roar_vio_calls    vio;
 struct sockaddr_storage  addr;
 socklen_t                addrlen = sizeof(addr);

 if ( roar_vio_ctl(&(lsock->sock), ROAR_VIO_CTL_GET_FH, &socket) == -1 ) {
  // next is needed for winsock:
  if ( roar_vio_ctl(&(lsock->sock), ROAR_VIO_CTL_GET_SELECT_FH, &socket) == -1 ) {
   ROAR_DBG("net_get_new_client(void) = -1 // can not find any acceptable socket to accept() on");
   return -1;
  }
 }

 fh = accept(socket, (struct sockaddr*)&addr, &addrlen);

 ROAR_DBG("net_get_new_client(void): fh = %i", fh);

 if ( fh == -1 )
  return -1;

#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND
 if ( lsock->proto == ROAR_PROTO_RSOUND ) {
  client = emul_rsound_on_connect(fh, lsock);
  switch (client) {
   case -1: return -1; break;
   case -2: return  0; break;
   default: // TODO: write error handling
     clients_get(client, &c);
     fh = c->fh;
    break;
  }
 } else {
#endif

 client = clients_new();

 if ( client == -1 ) {
  ROAR_DBG("net_get_new_client(void) = -1 // can not create new client");
  close(fh);
  return -1;
 }

 if ( clients_set_fh(client, fh) == -1 ) {
  ROAR_ERR("net_get_new_client(void): Can not set client's fh");

  clients_delete(client);
  close(fh);

  ROAR_DBG("net_get_new_client(void) = -1");
  return -1;
 }
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND
 }
#endif

 if ( clients_get(client, &c) != -1 ) {
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
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND
  case ROAR_PROTO_RSOUND: // nothing to do here.
   break;
#endif
#ifndef ROAR_WITHOUT_DCOMP_EMUL_RPLAY
  case ROAR_PROTO_RPLAY: // nothing to do here.

    if ( roar_vio_open_fh_socket(&vio, fh) == -1 )
     return -1;

    if ( emul_rplay_on_status(client, NULL, &vio, NULL, 0) == -1 )
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

//ll
