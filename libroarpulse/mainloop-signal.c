//mainloop-signal.c:

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

#define MAX_SIG 64 /* there is no way to find out */

typedef void (*pa_signal_cb_t) (pa_mainloop_api *api, pa_signal_event*e, int sig, void *userdata);

struct pa_signal_event {
 int used;
 int sig;
 pa_signal_cb_t cb;
 void * userdata;
};

static int _roar_pa_signal_inited = 0;

static struct {
 pa_mainloop_api * api;
 pa_signal_event sig[MAX_SIG];
} _roar_pa_signal;

static void _roar_pa_signal_handler (int sig) {
 pa_signal_event * e;

 ROAR_DBG("_roar_pa_signal_handler(sig=%s(%i)) = ?", strsignal(sig), sig);

 if ( sig >= MAX_SIG )
  return;

 e = &(_roar_pa_signal.sig[sig]);

 if ( !e->used )
  return;

 ROAR_DBG("_roar_pa_signal_handler(sig=%s(%i)): signal is used", strsignal(sig), sig);

 ROAR_DBG("_roar_pa_signal_handler(sig=%s(%i)): callback at %p", strsignal(sig), sig, e->cb);

 ROAR_DBG("_roar_pa_signal_handler(sig=%s(%i)): api=%p, userdata=%p", strsignal(sig), sig, _roar_pa_signal.api, e->userdata);

 if ( e->cb != NULL )
  e->cb(_roar_pa_signal.api, e, sig, e->userdata);

 ROAR_DBG("_roar_pa_signal_handler(sig=%s(%i)) = (void)", strsignal(sig), sig);
}

/** Initialize the UNIX signal subsystem and bind it to the specified main loop */
int pa_signal_init(pa_mainloop_api *api) {

 if ( _roar_pa_signal_inited )
  return -1;

 memset(&_roar_pa_signal, 0, sizeof(_roar_pa_signal));

 _roar_pa_signal.api = api;

 _roar_pa_signal_inited = 1;

 return 0;
}

/** Cleanup the signal subsystem */
void pa_signal_done(void) {
 _roar_pa_signal_inited = 0;
}

/** Create a new UNIX signal event source object */
pa_signal_event* pa_signal_new(int sig, pa_signal_cb_t callback, void *userdata) {
 if ( !_roar_pa_signal_inited )
  return NULL;

 if ( sig >= MAX_SIG )
  return NULL;

 _roar_pa_signal.sig[sig].used     = 1;
 _roar_pa_signal.sig[sig].sig      = sig;
 _roar_pa_signal.sig[sig].cb       = callback;
 _roar_pa_signal.sig[sig].userdata = userdata;

 signal(sig, _roar_pa_signal_handler);

 return &(_roar_pa_signal.sig[sig]);
}

/** Free a UNIX signal event source object */
void pa_signal_free(pa_signal_event *e) {
 if ( !_roar_pa_signal_inited )
  return;

 if ( e == NULL )
  return;

 signal(e->sig, SIG_DFL);

 e->used = 0;
}

/** Set a function that is called when the signal event source is destroyed. Use this to free the userdata argument if required */
void pa_signal_set_destroy(pa_signal_event *e, void (*callback) (pa_mainloop_api *api, pa_signal_event*e, void *userdata));

//ll
