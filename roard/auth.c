//auth.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#define _NONE ROAR_AUTH_T_AUTO

int auth_init (void) {
 int i;

 memset(g_auth_keyring, 0, sizeof(g_auth_keyring));

 for (i = 0; i < AUTH_KEYRING_LEN; i++) {
  g_auth_keyring[i].type = _NONE;
 }

 // enable guest access.
 if ( auth_addkey_anonymous(ACCLEV_ALL) == -1 )
  return -1;

#if 0
 // test password for API tests...
 auth_addkey_password(ACCLEV_ALL, "test");
#endif

 return 0;
}

int auth_free (void) {
 return 0;
}

union auth_typeunion * auth_regkey_simple(int type, enum roard_client_acclev acclev) {
 struct auth_key * key;
 int i;

 for (i = 0; i < AUTH_KEYRING_LEN; i++) {
  if ( (key = &(g_auth_keyring[i]))->type == _NONE ) {
   memset(key, 0, sizeof(struct auth_key));
   key->type   = type;
   key->acclev = acclev;
   return &(key->at_data);
  }
 }

 return NULL;
}

static int _ck_password(struct auth_key * key, struct roar_auth_message * authmes) {
 size_t len = strlen(key->at_data.password.password);

 // need to check here if we have a padding \0-byte.

 if ( len == authmes->len ) {
  if ( memcmp(key->at_data.password.password, authmes->data, len) ) {
   return -1;
  } else {
   return 1;
  }
 }

 return -1;
}

int auth_client_ckeck(struct roar_client_server * cs, struct roar_auth_message * authmes) {
 struct auth_key * key;
 int i;
 int ret;

 if ( cs == NULL || authmes == NULL )
  return -1;

 for (i = 0; i < AUTH_KEYRING_LEN; i++) {
  if ( (key = &(g_auth_keyring[i]))->type == authmes->type ) {
    ret = -1;
   switch (key->type) {
    case ROAR_AUTH_T_NONE:
      ret = 1;
     break;
    case ROAR_AUTH_T_PASSWORD:
      ret = _ck_password(key, authmes);
     break;
    case ROAR_AUTH_T_TRUST:
    case ROAR_AUTH_T_COOKIE:
    case ROAR_AUTH_T_SYSUSER:
    case ROAR_AUTH_T_RHOST:
    default:
      /* don't know what to do... */
      return -1;
     break;
   }
   switch (ret) {
    case -1:
      /* ignore this case and continue */
     break;
    case 0:
      return 0;
     break;
    case 1:
      cs->acclev = key->acclev;
      return 1;
     break;
    default: /* error! */
      return -1;
     break;
   }
  }
 }

 return -1;
}

int auth_addkey_anonymous(enum roard_client_acclev acclev) {
 if ( auth_regkey_simple(ROAR_AUTH_T_NONE, acclev) == NULL )
  return -1;
 return 0;
}

int auth_addkey_password(enum roard_client_acclev acclev, const char * password) {
 union auth_typeunion * pw;

 if ( (pw = auth_regkey_simple(ROAR_AUTH_T_PASSWORD, acclev)) == NULL )
  return -1;

 pw->password.password = password;

 return 0;
}

//ll
