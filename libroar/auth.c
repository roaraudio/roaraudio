//auth.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  and are therefore GPL. Because of this it may be illegal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

/* How auth works:
 * 0) set stage to zero
 * 1) get server address and local node name (from uname())
 * 2) look up authfile/authdb/authservice for the server+local address + stage.
 *    if no data was found send NONE-Auth.
 * 3) send data to server
 * 4) read answer from server
 * 5) if stage of server response is non-zero increment stage to server stage+1
 *    and repeat from step 2)
 * 6) check if we got an OK or an ERROR, return correct value
 */

/* The protocol:
 * Auth request:
 * Byte 0: auth type
 * Byte 1: stage
 * Byte 2: reserved (must be zero)
 * Byte 3: reserved (must be zero)
 * Byte 4-end: auth type depending data.
 *
 * If no data is to be send bytes 2 and 3 can be omitted.
 * If no data is to be send and stage is zero bytes 1, 2 and 3 can be omitted.
 *
 * Auth response:
 * The same as the auth request.
 * if the server sends an zero size message back it means the server accepted our connection
 * and no additional stage is needed.
 * if the message type is OK the server accepted our auth.
 * if the message type is ERROR the server rejected us. we may try other auth methods.
 * if the server accepted our data and the stage is non-zero we need to continue with the next
 * stage of the auth.
 * if the server rejected us the auth type value of the response is a suggested next auth type
 * we should try if possible. This may help the client to find a working auth type.
 */

/* The protocol by auth type:
 *
 * --- NONE:
 * No data is send, the server accepts the connect or rejects it depending on some
 * magic within the server. we do not care about this.
 * The data block is not used.
 *
 * --- COOKIE:
 * We send cookies for all stages the server ask us to provide a cookie.
 * if a cookie is wrong the server rejects us or asks us for another.
 * The cookie is send as binary data in the data block.
 *
 * --- TRUST:
 * We ask the server to auth us based on our UID/GID/PID.
 * The server may reject this because we are not allowed or because it is not
 * supported by the transport.
 * If we get rejected we may try to continue with IDENT then RHOST before we use NONE.
 * The data block is not used.
 *
 * --- PASSWORD:
 * This is technically the same as COOKIE just that the cookie is limited to
 * printable ASCII chars and that the user should be asked to provide the password.
 * This may be done via a GUI popup window.
 *
 * --- SYSUSER:
 * We provide a Username + Password for a system user.
 * The data block contains of two main parts:
 * The first part is a one byte long subtype.
 * The value must be 0x01 for username+password.
 * future versions may define other types.
 * the second part is the actual data block.
 * for username+password it is splited into two fields, both terminated with \0.
 * the first is the username the last one the password as clear text.
 * Example: char data[] = "\001MyUser\0MyPassword\0";
 * 
 * --- OPENPGP_SIGN:
 *
 * --- OPENPGP_ENCRYPT:
 *
 * --- OPENPGP_AUTH:
 *
 * --- KERBEROS:
 * We use Kerberos to auth.
 *
 * --- RHOST:
 * The server is asked to auth us based on our source address.
 * The data block is not used.
 *
 * --- XAUTH:
 * We send an X11 Cookie.
 *
 * --- IDENT:
 * The server is asked to auth us based on our source address using the IDENT protocol.
 * The data block is not used.
 *
 */

static int roar_auth_ask_server (struct roar_connection * con, struct roar_auth_message * authmes) {
 struct roar_message   mes;
 char                * header = mes.data;
 int                   ret;

 memset(&mes, 0, sizeof(struct roar_message)); // make valgrind happy!

 mes.cmd     = ROAR_CMD_AUTH;
 mes.datalen = 4;

 header[0] = authmes->type;
 header[1] = authmes->stage;
 header[2] = authmes->reserved.c[0];
 header[3] = authmes->reserved.c[1];

 if ( (ret = roar_req(con, &mes, NULL)) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 if ( mes.datalen < 4 ) {
  memset(header+mes.datalen, 0, 4-mes.datalen);
 }

 authmes->type          = header[0];
 authmes->stage         = header[1];
 authmes->reserved.c[0] = header[2];
 authmes->reserved.c[1] = header[3];

 return 0;
}

static void roar_auth_mes_init(struct roar_auth_message * authmes, int type) {
 memset(authmes, 0, sizeof(struct roar_auth_message));

 authmes->type  = type;
 authmes->stage = 0;
 authmes->data  = NULL;
 authmes->len   = 0;
}


static int try_password (struct roar_connection * con) {
 struct roar_message mes;
 struct roar_auth_message authmes;
 char * pw;

 roar_auth_mes_init(&authmes, ROAR_AUTH_T_PASSWORD);

 if ( roar_passwd_simple_ask_pw(&pw, "Password for RoarAudio Server?", NULL) == -1 ) {
  return -1;
 }

 authmes.len = strlen(pw);

 if ( roar_auth_init_mes(&mes, &authmes) == -1 ) {
  roar_mm_free(pw);
  return -1;
 }

 // do not use strcpy() because that would copy \0, too.
 memcpy(authmes.data, pw, authmes.len);

 roar_mm_free(pw);

 if ( roar_req(con, &mes, NULL) == -1 )
  return -1;

 if ( mes.cmd != ROAR_CMD_OK )
  return -1;

 if ( roar_auth_from_mes(&authmes, &mes, NULL) == -1 )
  return -1;

 if ( authmes.stage == 0 )
  return 0;

 return -1;
}

#define _EOL ROAR_AUTH_T_AUTO
int roar_auth   (struct roar_connection * con) {
 struct roar_auth_message authmes;
 int ret;
 int i;
 int ltt[] = {
              ROAR_AUTH_T_TRUST,
              ROAR_AUTH_T_IDENT,
              ROAR_AUTH_T_RHOST,
//              ROAR_AUTH_T_PASSWORD,
              ROAR_AUTH_T_NONE,
              _EOL
             };

 for (i = 0; ltt[i] != _EOL; i++) {
  switch (ltt[i]) {
    case ROAR_AUTH_T_PASSWORD:
     if ( (ret = try_password(con)) == -1 )
      continue;
     break;
    case ROAR_AUTH_T_TRUST:
    case ROAR_AUTH_T_IDENT:
    case ROAR_AUTH_T_RHOST:
    case ROAR_AUTH_T_NONE:
     roar_auth_mes_init(&authmes, ltt[i]);
     if ( (ret = roar_auth_ask_server(con, &authmes)) == -1 )
      continue;
    break;
   default: /* Bad error! */
     return -1;
    break;
  }

  if ( authmes.stage != 0 )
   continue;

  return 0;
 }

 return -1;
}


int roar_auth_from_mes(struct roar_auth_message * ames, struct roar_message * mes, void * data) {
 void * ibuf;
 char header[4] = {0, 0, 0, 0};

 if ( ames == NULL || mes == NULL )
  return -1;

 if ( data != NULL ) {
  ibuf = data;
 } else {
  ibuf = mes->data;
 }

 memset(ames, 0, sizeof(struct roar_auth_message));

 memcpy(header, ibuf, mes->datalen < 4 ? mes->datalen : 4);

 ames->type          = header[0];
 ames->stage         = header[1];
 ames->reserved.c[0] = header[2];
 ames->reserved.c[1] = header[3];

 if ( mes->datalen > 4 ) {
  ames->data = ibuf + 4;
  ames->len  = mes->datalen - 4;
 } else {
  ames->data = NULL;
  ames->len  = 0;
 }

 return 0;
}

int roar_auth_to_mes(struct roar_message * mes, void ** data, struct roar_auth_message * ames) {
 char * obuf;

 if ( mes == NULL || ames == NULL )
  return -1;

 if ( data != NULL )
  *data = NULL;

 memset(mes, 0, sizeof(struct roar_message));

 mes->cmd = ROAR_CMD_AUTH;

 if ( (ames->len + 4) > sizeof(mes->data) ) {
  *data = malloc(ames->len + 4);
  if ( *data == NULL )
   return -1;
  obuf = *data;
 } else {
  obuf = mes->data;
 }

 obuf[0] = ames->type;
 obuf[1] = ames->stage;
 obuf[2] = ames->reserved.c[0];
 obuf[3] = ames->reserved.c[1];

 memcpy(obuf + 8, ames->data, ames->len);

 mes->datalen = ames->len + 4;

 return 0;
}

int roar_auth_init_mes(struct roar_message * mes, struct roar_auth_message * ames) {
 if ( mes == NULL || ames == NULL )
  return -1;

 if ( (ames->len + 4) > sizeof(mes->data) )
  return -1;

 memset(mes, 0, sizeof(struct roar_message));

 mes->cmd = ROAR_CMD_AUTH;

 mes->data[0] = ames->type;
 mes->data[1] = ames->stage;
 mes->data[2] = ames->reserved.c[0];
 mes->data[3] = ames->reserved.c[1];

 ames->data = &(mes->data[4]);

 mes->datalen = ames->len + 4;

 return 0;
}



// String functions:
static struct {
 int    type;
 const char * name;
} _g_authts[] = {
// grep ^'#define ROAR_AUTH_T_' auth.h | while read d t d; do n=$(cut -d_ -f4 <<<$t | tr A-Z a-z); printf ' {%-28s %-10s},\n' $t, \"$n\"; done
 {ROAR_AUTH_T_NONE,            "none"    },
 {ROAR_AUTH_T_COOKIE,          "cookie"  },
 {ROAR_AUTH_T_TRUST,           "trust"   },
 {ROAR_AUTH_T_PASSWORD,        "password"},
 {ROAR_AUTH_T_SYSUSER,         "sysuser" },
 {ROAR_AUTH_T_OPENPGP_SIGN,    "openpgp" },
 {ROAR_AUTH_T_OPENPGP_ENCRYPT, "openpgp" },
 {ROAR_AUTH_T_OPENPGP_AUTH,    "openpgp" },
 {ROAR_AUTH_T_KERBEROS,        "kerberos"},
 {ROAR_AUTH_T_RHOST,           "rhost"   },
 {ROAR_AUTH_T_XAUTH,           "xauth"   },
 {ROAR_AUTH_T_IDENT,           "ident"   },
 {-1, NULL}
};

int    roar_str2autht(const char * str) {
 int i;

 for (i = 0; _g_authts[i].name != NULL; i++)
  if ( !strcasecmp(_g_authts[i].name, str) )
   return _g_authts[i].type;

 return -1;
}

const char * roar_autht2str(const int auth) {
 int i;

 for (i = 0; _g_authts[i].name != NULL; i++)
  if ( _g_authts[i].type == auth )
   return _g_authts[i].name;

 return "(UNKNOWN)";
}

//ll
