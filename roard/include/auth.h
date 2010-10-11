//auth.h:

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

#ifndef _AUTH_H_
#define _AUTH_H_

#include <roaraudio.h>

#define AT_TRUST_MAX_ENTRYS 8
#define AUTH_KEYRING_LEN    8

struct at_none {
 char dummy;
};

struct at_cookie {
 char * cookie;
 size_t len;
};

struct at_trust {
 pid_t pids[AT_TRUST_MAX_ENTRYS];
 uid_t uids[AT_TRUST_MAX_ENTRYS];
 gid_t gids[AT_TRUST_MAX_ENTRYS];
 size_t pids_len, uids_len, gids_len;
};

struct at_password {
 const char * password;
};

union auth_typeunion {
 struct at_none     none;
 struct at_cookie   cookie;
 struct at_trust    trust;
 struct at_password password;
};

struct auth_key {
 int type;
 enum roard_client_acclev acclev;
 union auth_typeunion at_data;
};

struct auth_key g_auth_keyring[AUTH_KEYRING_LEN];

int auth_init (void);
int auth_free (void);

union auth_typeunion * auth_regkey_simple(int type, enum roard_client_acclev acclev);

int auth_client_ckeck(struct roar_client_server * cs, struct roar_auth_message * authmes);


int auth_addkey_anonymous(enum roard_client_acclev acclev);
int auth_addkey_password(enum roard_client_acclev acclev, const char * password);
int auth_addkey_cookie(enum roard_client_acclev acclev, const void * cookie, const size_t len);
int auth_addkey_trust(enum roard_client_acclev acclev, ...);

#endif

//ll
