//network.c:

#include "roard.h"

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
#ifdef SO_PEERCRED
 struct roar_client * c;
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
#endif

// close(fh);

 return 0;
}

//ll
