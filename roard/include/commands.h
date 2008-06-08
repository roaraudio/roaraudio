//commands.h:

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <roaraudio.h>

#define COMMAND_MAX_NAMELEN  80
#define COMMAND_MAX_COMMANDS 32

struct roar_command {
 int cmd;
 char name[COMMAND_MAX_NAMELEN];
 int (*handler)(int client, struct roar_message * mes, char * data);
};

int command_get_id_by_cmd (int command);
int command_exec (int client, struct roar_message * mes, char * data);
int command_get_name (int command, char ** name);

#endif

//ll
