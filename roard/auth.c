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

#if 0
 // test trust for API tests...
 auth_addkey_trust(ACCLEV_ALL, -1, 0, getuid()+1, -1, getgid()+1, -1);
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

static int _ck_cookie(struct auth_key * key, struct roar_auth_message * authmes) {
 if ( key->at_data.cookie.len == authmes->len ) {
  if ( memcmp(key->at_data.cookie.cookie, authmes->data, authmes->len) ) {
   return -1;
  } else {
   return 1;
  }
 }

 return -1;
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

static int _ck_trust(struct auth_key * key, struct roar_auth_message * authmes, struct roar_client_server * cs) {
 struct at_trust * t = &(key->at_data.trust);
 size_t i;

 // we ship pids at the moment as cs does not contain a verifyed one.

 for (i = 0; i < t->uids_len; i++)
  if ( t->uids[i] == ROAR_CLIENT(cs)->uid )
   return 1;

 for (i = 0; i < t->gids_len; i++)
  if ( t->gids[i] == ROAR_CLIENT(cs)->gid )
   return 1;

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
    case ROAR_AUTH_T_COOKIE:
      ret = _ck_cookie(key, authmes);
     break;
    case ROAR_AUTH_T_TRUST:
      ret = _ck_trust(key, authmes, cs);
     break;
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

int auth_addkey_cookie(enum roard_client_acclev acclev, const void * cookie, const size_t len) {
 union auth_typeunion * key;

 if ( (key = auth_regkey_simple(ROAR_AUTH_T_COOKIE, acclev)) == NULL )
  return -1;

 key->cookie.cookie = (void*)cookie;
 key->cookie.len    = len;

 return 0;
}

int auth_addkey_trust(enum roard_client_acclev acclev, ...) {
 union auth_typeunion * key;
 size_t i;
 va_list va;
 pid_t pid;
 uid_t uid;
 gid_t gid;
 int err = 0;

 if ( (key = auth_regkey_simple(ROAR_AUTH_T_TRUST, acclev)) == NULL )
  return -1;

 // zerosize all counters.
 memset(key, 0, sizeof(union auth_typeunion));

 va_start(va, acclev);

 do { // eval block we can leave with continue.

#define _block(var,type,array)  i = 0; \
                                do { \
                                 if ( i == AT_TRUST_MAX_ENTRYS ) { err = 1; continue; } \
                                 var = va_arg(va, type); \
                                 key->trust.array[i] = var; \
                                 i++; \
                                } while (var != -1); \
                                if ( err ) continue; \
                                key->trust.array ## _len = i;


 _block(pid, pid_t, pids);
 _block(uid, uid_t, uids);
 _block(gid, gid_t, gids);

#undef _block

 } while(0);

 va_end(va);

 if ( !err )
  return 0;

 memset(key, 0, sizeof(union auth_typeunion));

 return -1;
}

//ll
