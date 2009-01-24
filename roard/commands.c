//commands.c:

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

struct roar_command g_commands[COMMAND_MAX_COMMANDS] = {
  {ROAR_CMD_NOOP,         "NOOP",         req_on_noop},
  {ROAR_CMD_IDENTIFY,     "IDENTIFY",     req_on_identify},
  {ROAR_CMD_AUTH,         "AUTH",         req_on_auth},
  {ROAR_CMD_WHOAMI,       "WHOAMI",       req_on_whoami},


  {ROAR_CMD_NEW_STREAM,   "NEW_STREAM",   req_on_new_stream},

  {ROAR_CMD_SET_META,     "SET_META",     req_on_set_meta},
  {ROAR_CMD_GET_META,     "GET_META",     req_on_get_meta},
  {ROAR_CMD_LIST_META,    "LIST_META",    req_on_list_meta},

  {ROAR_CMD_EXEC_STREAM,  "EXEC_STREAM",  req_on_exec_stream},
  {ROAR_CMD_QUIT,         "QUIT",         (int(*)(int client, struct roar_message * mes, char * data))clients_delete},

  {ROAR_CMD_CON_STREAM,   "CON_STREAM",   req_on_con_stream},
  {ROAR_CMD_PASSFH,       "PASSFH",       req_on_passfh},

  {ROAR_CMD_SERVER_OINFO, "SERVER_OINFO", req_on_server_oinfo},
  {ROAR_CMD_GET_STANDBY,  "GET_STANDBY",  req_on_get_standby},
  {ROAR_CMD_SET_STANDBY,  "SET_STANDBY",  req_on_set_standby},
  {ROAR_CMD_EXIT,         "EXIT",         req_on_exit},

  {ROAR_CMD_LIST_CLIENTS, "LIST_CLIENTS", req_on_list_clients},
  {ROAR_CMD_LIST_STREAMS, "LIST_STREAMS", req_on_list_streams},
  {ROAR_CMD_GET_CLIENT,   "GET_CLIENT",   req_on_get_client},
  {ROAR_CMD_GET_STREAM,   "GET_STREAM",   req_on_get_stream},
  {ROAR_CMD_KICK,         "KICK",         req_on_kick},
  {ROAR_CMD_ATTACH,       "ATTACH",       req_on_attach},
  {ROAR_CMD_SET_VOL,      "SET_VOL",      req_on_set_vol},
  {ROAR_CMD_GET_VOL,      "GET_VOL",      req_on_get_vol},
  {ROAR_CMD_GET_STREAM_PARA, "GET_STREAM_PARA", req_on_get_stream_para},
  {ROAR_CMD_SET_STREAM_PARA, "SET_STREAM_PARA", req_on_set_stream_para},

  {ROAR_CMD_ADD_DATA,     "ADD_DATA",     req_on_add_data},

  {ROAR_CMD_EOL,          "END OF LIST",  NULL}
 };

int command_get_id_by_cmd (int command) {
 int i;
 int cmd;

 for (i = 0; i < COMMAND_MAX_COMMANDS; i++) {
  if ( (cmd = g_commands[i].cmd) == ROAR_CMD_EOL )
   break;

  if ( cmd == command )
   return i;
 }

 return -1;
}

int command_exec (int client, struct roar_message * mes, char * data) {
 int cmd = command_get_id_by_cmd(mes->cmd);
 int (*func)(int client, struct roar_message * mes, char * data);

 if ( cmd == -1 )
  return -1;

 if ( (func = g_commands[cmd].handler) == NULL )
  return -1;

 ROAR_DBG("command_exec(*): Execing command %i(%s) via %p", cmd, g_commands[cmd].name, func);

 return func(client, mes, data);
}

int command_get_name (int command, char ** name) {
 int cmd = command_get_id_by_cmd(command);

 if ( cmd == -1 ) {
  *name = NULL;
  return -1;
 }

 *name = g_commands[cmd].name;

 return 0;
}


//ll
