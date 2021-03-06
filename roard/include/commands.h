//commands.h:

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
 nd_exec*
 */

#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include <roaraudio.h>

#define COMMAND_MAX_NAMELEN  80
#define COMMAND_MAX_COMMANDS 32

#define COMMAND_FLAG_NONE          0x00000000
#define COMMAND_FLAG_OUT_CLOSECON  0x00000001
#define COMMAND_FLAG_OUT_LONGDATA  0x00000002
#define COMMAND_FLAG_OUT_NOSEND    0x00000004

struct roar_command {
 int cmd;
#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_MINIMAL)
 char name[COMMAND_MAX_NAMELEN];
#else
 char * name;
#endif
 int (*handler)(int client, struct roar_message * mes, char ** data, uint32_t flags[2]);
 enum roard_client_acclev minacclev;
};

int command_get_id_by_cmd (int command);
int command_exec (int client, struct roar_message * mes, char ** data, uint32_t flags[2]);
int command_get_name (int command, char ** name);

#endif

//ll
