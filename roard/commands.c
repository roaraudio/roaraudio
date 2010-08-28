//commands.c:

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

#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_MINIMAL)
#define _NAME(x) (x)
#else
#define _NAME(x) ((char*)NULL)
#endif

struct roar_command g_commands[COMMAND_MAX_COMMANDS] = {
  {ROAR_CMD_NOOP,         _NAME("NOOP"),         req_on_noop},
  {ROAR_CMD_IDENTIFY,     _NAME("IDENTIFY"),     req_on_identify},
  {ROAR_CMD_AUTH,         _NAME("AUTH"),         req_on_auth},
  {ROAR_CMD_WHOAMI,       _NAME("WHOAMI"),       req_on_whoami},


  {ROAR_CMD_NEW_STREAM,   _NAME("NEW_STREAM"),   req_on_new_stream},

#ifdef ROAR_SUPPORT_META
  {ROAR_CMD_SET_META,     _NAME("SET_META"),     req_on_set_meta},
  {ROAR_CMD_GET_META,     _NAME("GET_META"),     req_on_get_meta},
  {ROAR_CMD_LIST_META,    _NAME("LIST_META"),    req_on_list_meta},
#endif

  {ROAR_CMD_EXEC_STREAM,  _NAME("EXEC_STREAM"),  req_on_exec_stream},
  {ROAR_CMD_QUIT,         _NAME("QUIT"),         (int(*)(int client, struct roar_message * mes, char ** data, uint32_t flags[2]))clients_delete},

  {ROAR_CMD_CON_STREAM,   _NAME("CON_STREAM"),   req_on_con_stream},
  {ROAR_CMD_PASSFH,       _NAME("PASSFH"),       req_on_passfh},

  {ROAR_CMD_SERVER_OINFO, _NAME("SERVER_OINFO"), req_on_server_oinfo},
  {ROAR_CMD_GET_STANDBY,  _NAME("GET_STANDBY"),  req_on_get_standby},
  {ROAR_CMD_SET_STANDBY,  _NAME("SET_STANDBY"),  req_on_set_standby},
  {ROAR_CMD_EXIT,         _NAME("EXIT"),         req_on_exit},

  {ROAR_CMD_LIST_CLIENTS, _NAME("LIST_CLIENTS"), req_on_list_clients},
  {ROAR_CMD_LIST_STREAMS, _NAME("LIST_STREAMS"), req_on_list_streams},
  {ROAR_CMD_GET_CLIENT,   _NAME("GET_CLIENT"),   req_on_get_client},
  {ROAR_CMD_GET_STREAM,   _NAME("GET_STREAM"),   req_on_get_stream},
  {ROAR_CMD_KICK,         _NAME("KICK"),         req_on_kick},
  {ROAR_CMD_ATTACH,       _NAME("ATTACH"),       req_on_attach},
  {ROAR_CMD_SET_VOL,      _NAME("SET_VOL"),      req_on_set_vol},
  {ROAR_CMD_GET_VOL,      _NAME("GET_VOL"),      req_on_get_vol},
  {ROAR_CMD_GET_STREAM_PARA, _NAME("GET_STREAM_PARA"), req_on_get_stream_para},
  {ROAR_CMD_SET_STREAM_PARA, _NAME("SET_STREAM_PARA"), req_on_set_stream_para},

  {ROAR_CMD_ADD_DATA,     _NAME("ADD_DATA"),     req_on_add_data},

  {ROAR_CMD_BEEP,         _NAME("BEEP"),         req_on_beep},

  {ROAR_CMD_EOL,          _NAME("END OF LIST"),  NULL}
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

int command_exec (int client, struct roar_message * mes, char ** data, uint32_t flags[2]) {
 int cmd = command_get_id_by_cmd(mes->cmd);
 int (*func)(int client, struct roar_message * mes, char ** data, uint32_t flags[2]);

 if ( cmd == -1 )
  return -1;

 if ( (func = g_commands[cmd].handler) == NULL ) {
  ROAR_WARN("command_exec(client=%i, mes=%p{.cmd=%i,...}, data=%p, flags=%p) = -1 // unknown command", client, mes, (int)mes->cmd, data, flags);
  return -1;
 }

 ROAR_DBG("command_exec(*): Execing command %i(%s) via %p", cmd, g_commands[cmd].name, func);

 return func(client, mes, data, flags);
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
