//error.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libpulse*. They are mostly copyrighted by:
 *  Lennart Poettering <poettering@users.sourceforge.net> and
 *  Pierre Ossman <drzeus@drzeus.cx>
 *
 *  This file is part of libroarpulse a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>


static const struct {
 int error;
 const char * name;
} _roar_pa_errors[] = {
 {PA_OK,                       "OK"                             },
 {PA_ERR_ACCESS,               "Access denied"                  },
 {PA_ERR_COMMAND,              "Unknown command"                },
 {PA_ERR_INVALID,              "Invalid argument"               },
 {PA_ERR_EXIST,                "Entity exists"                  },
 {PA_ERR_NOENTITY,             "No such entity"                 },
 {PA_ERR_CONNECTIONREFUSED,    "Connection refused"             },
 {PA_ERR_PROTOCOL,             "Protocol error"                 },
 {PA_ERR_TIMEOUT,              "Timeout"                        },
 {PA_ERR_AUTHKEY,              "No authorization key"           },
 {PA_ERR_INTERNAL,             "Internal error"                 },
 {PA_ERR_CONNECTIONTERMINATED, "Connection terminated"          },
 {PA_ERR_KILLED,               "Entity killed"                  },
 {PA_ERR_INVALIDSERVER,        "Invalid server"                 },
 {PA_ERR_MODINITFAILED,        "Module initalization failed"    },
 {PA_ERR_BADSTATE,             "Bad state"                      },
 {PA_ERR_NODATA,               "No data"                        },
 {PA_ERR_VERSION,              "Incompatible protocol version"  },
 {PA_ERR_TOOLARGE,             "Too large"                      },
 {PA_ERR_MAX,                  "MAX"                            },
 {-1, NULL}
};

const char * pa_strerror(int error) {
 int i;

 for (i = 0; _roar_pa_errors[i].name != NULL; i++)
  if ( _roar_pa_errors[i].error == error )
   return _roar_pa_errors[i].name;

 return NULL;
}

//ll
