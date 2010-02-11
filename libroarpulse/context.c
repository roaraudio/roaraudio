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

struct _roar_pa_cb_st {
 union {
  pa_context_success_cb_t scb;
  pa_context_notify_cb_t  ncb;
 } cb;
 void * userdata;
};

#define _call_cbs(x,c,s) ((x).cb.scb == NULL ? (void)0 : (x).cb.scb((c), (s), (x).userdata))
#define _call_cbn(x,c)   ((x).cb.ncb == NULL ? (void)0 : (x).cb.ncb((c), (x).userdata))

struct pa_context {
 size_t refc;
 struct roar_connection con;
 char * server;
 char * name;
 int state;
 int errnum;
 struct {
  struct _roar_pa_cb_st set_name;
  struct _roar_pa_cb_st state_change;
 } cb;
 pa_mainloop_api * mainloop;
};

// older versions:
typedef void pa_proplist;
pa_context *pa_context_new_with_proplist(pa_mainloop_api *mainloop, const char *name, pa_proplist *proplist);

void pa_context_set_state(pa_context *c, pa_context_state_t st);

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

 c->errnum = PA_OK;

 if ( name != NULL ) {
  c->name = roar_mm_strdup(name);
 }

 c->mainloop = mainloop;

 return c;
}

static void _context_free(pa_context *c) {
 pa_context_disconnect(c);

 if ( c->server != NULL )
  roar_mm_free(c->server);

 if ( c->name != NULL )
  roar_mm_free(c->name);

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

 if ( c->state != PA_CONTEXT_UNCONNECTED ) {
  c->errnum = PA_ERR_BADSTATE;
  pa_context_set_state(c, PA_CONTEXT_FAILED);
  return -1;
 }

 // we do currently not support to spawn a daemon, so we ignore flags and api.

 server = roar_pa_find_server((char*)server);

 if ( roar_simple_connect(&(c->con), (char*)server,
                          c->name != NULL ? c->name : "libroarpulse [pa_context_connect()]") == -1 ) {
  c->errnum = PA_ERR_CONNECTIONREFUSED;
  pa_context_set_state(c, PA_CONTEXT_FAILED);
  return -1;
 }

 if ( server == NULL ) {
  c->server = NULL;
 } else {
  c->server = roar_mm_strdup(server);
 }

 pa_context_set_state(c, PA_CONTEXT_READY);

 _call_cbs(c->cb.set_name, c, 1);

 return 0;
}

void pa_context_disconnect(pa_context *c) {
 if ( c == NULL )
  return;

 if ( c->state != PA_CONTEXT_READY )
  return;

 roar_disconnect(&(c->con));

 pa_context_set_state(c, PA_CONTEXT_TERMINATED);
}



/** Set a callback function that is called whenever the context status changes */
void pa_context_set_state_callback(pa_context *c, pa_context_notify_cb_t cb, void *userdata) {
 if ( c == NULL )
  return;

 c->cb.state_change.cb.ncb   = cb;
 c->cb.state_change.userdata = userdata;
}

void pa_context_set_state(pa_context *c, pa_context_state_t st) {
 if ( c == NULL )
  return;

 c->state = st;

 _call_cbn(c->cb.state_change, c);
}

/** Return the error number of the last failed operation */
int pa_context_errno(pa_context *c) {
 if ( c == NULL )
  return PA_ERR_INVALID;

 return c->errnum;
}

/** Return non-zero if some data is pending to be written to the connection */
int pa_context_is_pending(pa_context *c) {
 return 0;
}

/** Return the current context status */
pa_context_state_t pa_context_get_state(pa_context *c) {
 if ( c == NULL )
  return PA_CONTEXT_FAILED;

 return c->state;
}

/** Drain the context. If there is nothing to drain, the function returns NULL */
pa_operation* pa_context_drain(pa_context *c, pa_context_notify_cb_t cb, void *userdata) {

 if ( cb != NULL )
  cb(c, userdata);

 return NULL;
}

/** Tell the daemon to exit. The returned operation is unlikely to
 * complete succesfully, since the daemon probably died before
 * returning a success notification */
pa_operation* pa_context_exit_daemon(pa_context *c, pa_context_success_cb_t cb, void *userdata) {
 int s = 1;

 if ( c == NULL )
  return roar_pa_operation_new(PA_OPERATION_DONE);

 if ( c->state == PA_CONTEXT_READY ) {
  if ( roar_exit(&(c->con)) == -1 ) {
   c->errnum = PA_ERR_INTERNAL;
   s = 0;
  }
 } else {
  c->errnum = PA_ERR_BADSTATE;
  s = 0;
 }

 if ( cb != NULL )
  cb(c, s, userdata);

 return roar_pa_operation_new(PA_OPERATION_DONE);
}

/** Set the name of the default sink. \since 0.4 */
pa_operation* pa_context_set_default_sink(pa_context *c, const char *name, pa_context_success_cb_t cb, void *userdata) {
 if ( c != NULL )
  c->errnum = PA_ERR_NOTSUPPORTED;

 if ( cb != NULL )
  cb(c, 0, userdata);

 return roar_pa_operation_new(PA_OPERATION_DONE);
}

/** Set the name of the default source. \since 0.4 */
pa_operation* pa_context_set_default_source(pa_context *c, const char *name, pa_context_success_cb_t cb, void *userdata) {
 if ( c != NULL )
  c->errnum = PA_ERR_NOTSUPPORTED;

 if ( cb != NULL )
  cb(c, 0, userdata);

 return roar_pa_operation_new(PA_OPERATION_DONE);
}

/** Returns 1 when the connection is to a local daemon. Returns negative when no connection has been made yet. \since 0.5 */
int pa_context_is_local(pa_context *c) {
 if ( c == NULL )
  return -1;

 if ( c->state != PA_CONTEXT_READY )
  return -1;

 // how is /local/ defined?
 return 0;
}

/** Set a different application name for context on the server. \since 0.5 */
pa_operation* pa_context_set_name(pa_context *c, const char *name, pa_context_success_cb_t cb, void *userdata) {
 if ( c == NULL )
  return roar_pa_operation_new(PA_OPERATION_DONE);

 if ( c->state != PA_CONTEXT_UNCONNECTED ) {
  c->errnum = PA_ERR_BADSTATE;

  if ( cb != NULL )
   cb(c, 0, userdata);

  return roar_pa_operation_new(PA_OPERATION_DONE);
 }

 if ( c->name != NULL )
  roar_mm_free(c->name);

 c->name = roar_mm_strdup(name);
 c->cb.set_name.cb.scb   = cb;
 c->cb.set_name.userdata = userdata;

 return roar_pa_operation_new(PA_OPERATION_DONE);
}

/** Return the server name this context is connected to. \since 0.7 */
const char* pa_context_get_server(pa_context *c) {
 if ( c == NULL )
  return NULL;

 return c->server;
}

/** Return the protocol version of the library. \since 0.8 */
uint32_t pa_context_get_protocol_version(pa_context *c) {
 return 0;
}

/** Return the protocol version of the connected server. \since 0.8 */
uint32_t pa_context_get_server_protocol_version(pa_context *c) {
 return 0;
}


//ll
