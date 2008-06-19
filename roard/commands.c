//commands.c:

#include "roard.h"

struct roar_command g_commands[COMMAND_MAX_COMMANDS] = {
  {ROAR_CMD_NOOP,         "NOOP",         req_on_noop},
  {ROAR_CMD_IDENTIFY,     "IDENTIFY",     req_on_identify},
  {ROAR_CMD_AUTH,         "AUTH",         req_on_auth},
  {ROAR_CMD_NEW_STREAM,   "NEW_STREAM",   req_on_new_stream},
  {ROAR_CMD_SET_META,     "SET_META",     req_on_set_meta},
  {ROAR_CMD_EXEC_STREAM,  "EXEC_STREAM",  req_on_exec_stream},
  {ROAR_CMD_QUIT,         "QUIT",         clients_delete},

  {ROAR_CMD_SERVER_OINFO, "SERVER_OINFO", req_on_server_oinfo},
  {ROAR_CMD_GET_STANDBY,  "GET_STANDBY",  req_on_get_standby},
  {ROAR_CMD_SET_STANDBY,  "SET_STANDBY",  req_on_set_standby},
  {ROAR_CMD_EXIT,         "EXIT",         req_on_exit},

  {ROAR_CMD_LIST_CLIENTS, "LIST_CLIENTS", req_on_list_clients},
  {ROAR_CMD_LIST_STREAMS, "LIST_STREAMS", req_on_list_streams},
  {ROAR_CMD_GET_CLIENT,   "GET_CLIENT",   req_on_get_client},
  {ROAR_CMD_GET_STREAM,   "GET_STREAM",   req_on_get_stream},
  {ROAR_CMD_KICK,         "KICK",         req_on_kick},
  {ROAR_CMD_SET_VOL,      "SET_VOL",      req_on_set_vol},

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
