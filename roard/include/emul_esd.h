//emul_esd.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifndef _EMUL_ESD_H_
#define _EMUL_ESD_H_

#include <roaraudio.h>

#ifndef ROAR_WITHOUT_DCOMP_EMUL_ESD
#ifdef ROAR_HAVE_ESD

struct emul_esd_command {
 int    cmd;
 size_t datalen;
#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_MINIMAL)
 char name[COMMAND_MAX_NAMELEN];
#else
 char * name;
#endif
 int (*handler)(int client, struct emul_esd_command * cmd, void * data, struct roar_vio_calls * vio);
};

int emul_esd_exec_command(int client, int cmd, struct roar_vio_calls * vio);
int emul_esd_check_client(int client, struct roar_vio_calls * vio);

#endif
#endif

#endif

//ll
