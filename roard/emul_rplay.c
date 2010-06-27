//emul_rplay.c:

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

struct emul_rplay_command emul_rplay_commands[] = {
 {"access",      NULL, -1, -1, NULL},
 {"application", NULL,  1, -1, NULL},
 {"continue",    NULL,  1, -1, NULL},
 {"die",         NULL,  1, -1, NULL},
 {"done",        NULL,  1, -1, NULL}, // #ifdef DEBUG
 {"find",        NULL,  1,  1, NULL},
 {"get",         NULL,  1,  1, NULL},
 {"help",        NULL, -1, -1, NULL},
 {"info",        NULL,  1,  1, NULL},
 {"list",        NULL,  0,  1, NULL},
 {"modify",      NULL,  2, -1, NULL},
 {"monitor",     NULL,  1, -1, NULL},
 {"pause",       NULL,  1, -1, NULL},
 {"play",        NULL,  1, -1, NULL},
 {"put",         NULL,  2, -1, NULL},
 {"quit",        NULL,  0,  0, NULL},
 {"reset",       NULL,  0,  0, NULL},
 {"set",         NULL,  1, -1, NULL},
 {"skip",        NULL,  1,  1, NULL},
 {"status",      NULL,  0,  0, NULL},
 {"stop",        NULL,  1, -1, NULL},
 {"version",     NULL,  0,  0, NULL},
 {"volume",      NULL,  0,  1, NULL},
 {"wait",        NULL, -1, -1, NULL},
 {NULL, NULL, -1, -1, NULL}
};

static inline int is_true(const char * str) {
 const char * ts[] = {"true", "t", "1", "yes", "y", "on"};
 int i;

 for (i = 0; i < sizeof(ts)/sizeof(*ts); i++)
  if ( !strcasecmp(str, ts[i]) )
   return 1;

 return 0;
}

static inline int is_false(const char * str) {
 return !is_true(str);
}


int emul_rplay_check_client  (int client, struct roar_vio_calls * vio);

int emul_rplay_exec_command  (int client, struct roar_vio_calls * vio, char * command) {
 struct emul_rplay_command * cmd;
 struct roar_keyval * kv;
 ssize_t kvlen;
 char * para = NULL;
 char * c;
 int last_was_space = 0;

 for (c = command; *c != 0; c++) {
  if ( *c == ' ' || *c == '\t' ) {
   last_was_space = 1;
   *c = 0;
  } else {
   if ( last_was_space ) {
    para = c;
    break;
   }
  }
 }

 if ( para == NULL ) {
  kv = NULL; 
  kvlen = 0;
 } else {
  kvlen = roar_keyval_split(&kv, para, " \t", "=", 0);
  if ( kvlen == -1 )
   return emul_rplay_send_error(client, NULL, vio, NULL, 0, "Can not parse parameter list");
 }

 for (cmd = emul_rplay_commands; cmd->name != NULL; cmd++) {
  if ( !strcasecmp(cmd->name, command) )
   break;
 }

 if ( cmd->name == NULL )
  return emul_rplay_send_error(client, NULL, vio, kv, kvlen, "unknown command");

 if ( cmd->handler == NULL )
  return emul_rplay_send_error(client, cmd, vio, kv, kvlen, "unsupported command");

 return cmd->handler(client, cmd, vio, kv, kvlen);
}

int emul_rplay_send_error    (int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen, const char * msg) {
 struct roar_keyval * kvr;
 const char * command;
 const char * cd = NULL;

 if ( cmd != NULL ) {
  command = cmd->name;
 } else {
  command = "(unknown)";
 }

 if ( kv != NULL ) {
  kvr = roar_keyval_lookup(kv, "client-data", kvlen, 0);
  if ( kvr != NULL )
   cd = kvr->value;
 }

 if ( cd == NULL )
  cd = "";

 return roar_vio_printf("-error=\"%s\" command=\"%s\" client-data=\"%s\"\n", msg, command, cd) <= 0 ? -1 : 0;
}

//ll
