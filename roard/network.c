//network.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_BROKEN_PEERCRED
#undef SO_PEERCRED
#endif

int net_check_listen  (void) {
 int r;
 fd_set sl;
 struct timeval tv;

 FD_ZERO(&sl);
 FD_SET(g_listen_socket, &sl);

 tv.tv_sec  = 0;
 tv.tv_usec = 1;

 if ((r = select(g_listen_socket + 1, &sl, NULL, NULL, &tv)) > 0) {
  ROAR_DBG("net_check_listen(void): We have a connection!");
  return net_get_new_client();
 }

 return r;
}

int net_get_new_client (void) {
 int fh;
 int client;
#if defined(SO_PEERCRED) || defined(ROAR_HAVE_GETPEEREID)
 struct roar_client * c;
#endif
#ifdef SO_PEERCRED
 struct ucred cred;
 socklen_t cred_len = sizeof(cred);
#endif

 fh = accept(g_listen_socket, NULL, NULL);

 ROAR_DBG("net_get_new_client(void): fh = %i", fh);

 client = clients_new();

 if ( clients_set_fh(client, fh) == -1 ) {
  ROAR_ERR("net_get_new_client(void): Can not set client's fh");

  clients_delete(client);
  close(fh);

  ROAR_DBG("net_get_new_client(void) = -1");
  return -1;
 }

#ifdef SO_PEERCRED
 if ( clients_get(client, &c) != -1 ) {
  if (getsockopt(fh, SOL_SOCKET, SO_PEERCRED, &cred, &cred_len) != -1) {
   if ( cred.pid != 0 ) {
    c->pid = cred.pid;
    c->uid = cred.uid;
    c->gid = cred.gid;
   }
  } else {
   ROAR_DBG("req_on_identify(): Can't get creds via SO_PEERCRED: %s", strerror(errno));
  }
 }
#elif defined(ROAR_HAVE_GETPEEREID)
 if ( clients_get(client, &c) != -1 ) {
  if (getpeereid(fh, &(c->uid), &(c->gid)) == -1) {
   ROAR_DBG("req_on_identify(): Can't get creds via getpeereid(): %s", strerror(errno));
  }
 }
#endif

// close(fh);

 return 0;
}

//ll
