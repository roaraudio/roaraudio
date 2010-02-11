//context.c:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

struct pa_context {
 size_t refc;
 struct roar_connection con;
 char * server;
 int state;
};

// older versions:
typedef void pa_proplist;
pa_context *pa_context_new_with_proplist(pa_mainloop_api *mainloop, const char *name, pa_proplist *proplist);

pa_context *pa_context_new(pa_mainloop_api *mainloop, const char *name) {
 return pa_context_new_with_proplist(mainloop, name, NULL);
}

pa_context *pa_context_new_with_proplist(pa_mainloop_api *mainloop, const char *name, pa_proplist *proplist) {
 pa_context * c;

 if ( proplist != NULL )
  return NULL;

 if ( (c = roar_mm_malloc(sizeof(pa_context))) == NULL )
  return NULL;

 memset(c, 0, sizeof(pa_context));

 c->refc  = 1;

 c->state = PA_CONTEXT_UNCONNECTED;

 return NULL;
}

static void _context_free(pa_context *c) {
 pa_context_disconnect(c);

 if ( c->server != NULL )
  roar_mm_free(c->server);

 roar_mm_free(c);
}

pa_context* pa_context_ref(pa_context *c) {
 c->refc++;
 return c;
}

void pa_context_unref(pa_context *c) {
 c->refc--;

 if ( c->refc < 1 )
  _context_free(c);
}

int pa_context_connect(
        pa_context *c,
        const char *server,
        pa_context_flags_t flags,
        const pa_spawn_api *api) {

 if ( c == NULL )
  return -1;

 if ( c->state != PA_CONTEXT_UNCONNECTED )
  return -1;

 // we do currently not support to spawn a daemon, so we ignore flags and api.

 server = roar_pa_find_server((char*)server);

 if ( roar_simple_connect(&(c->con), (char*)server, "libroarpulse [pa_context_connect()]") == -1 ) {
  c->state = PA_CONTEXT_FAILED;
  return -1;
 }

 c->server = roar_mm_strdup(server);
 c->state  = PA_CONTEXT_READY;

 return 0;
}

void pa_context_disconnect(pa_context *c) {
 if ( c == NULL )
  return;

 if ( c->state != PA_CONTEXT_READY )
  return;

 roar_disconnect(&(c->con));

 c->state = PA_CONTEXT_TERMINATED;
}

//ll
