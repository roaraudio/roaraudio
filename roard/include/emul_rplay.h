//emul_rplay.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

#ifndef _EMUL_RPLAY_H_
#define _EMUL_RPLAY_H_

#include <roaraudio.h>

#ifndef ROAR_WITHOUT_DCOMP_EMUL_RPLAY

struct emul_rplay_command {
 const char * name;
 const char * usage;
 ssize_t      min_args;
 ssize_t      max_args;
 int (*handler)(int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen);
};

int emul_rplay_check_client  (int client, struct roar_vio_calls * vio);

int emul_rplay_exec_command  (int client, struct roar_vio_calls * vio, char * command);

int emul_rplay_send_error    (int client, struct emul_rplay_command * cmd, struct roar_vio_calls * vio, struct roar_keyval * kv, size_t kvlen, const char * msg);

#endif

#endif

//ll
