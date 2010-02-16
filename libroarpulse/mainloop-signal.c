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
#ifdef ROAR_HAVE_PIPE
 int pipefh[2];
 pa_io_event * io_event;
#endif
} _roar_pa_signal;

static void _roar_pa_signal_iocb(
#ifndef ROAR_HAVE_PIPE
                                 int                 sig
#else
                                 pa_mainloop_api   * a,
                                 pa_io_event       * e,
                                 int                 fd,
                                 pa_io_event_flags_t f,
                                 void *userdata
#endif
                                ) {
 pa_signal_event * se;
#ifdef ROAR_HAVE_PIPE
 int sig;
 size_t ret;

 ret = read(fd, &sig, sizeof(sig));

 if ( ret != sizeof(sig) )
  return;
#endif

 if ( sig >= MAX_SIG )
  return;

 se = &(_roar_pa_signal.sig[sig]);

 if ( !se->used )
  return;

 ROAR_DBG("_roar_pa_signal_iocb(*): sig=%s(%i), se->cb=%p", strsignal(sig), sig, se->cb);

 if ( se->cb != NULL )
  se->cb(_roar_pa_signal.api, se, sig, se->userdata);
}

static void _roar_pa_signal_handler (int sig) {
#ifdef ROAR_HAVE_PIPE
 write(_roar_pa_signal.pipefh[1], &sig, sizeof(sig));
#else
 _roar_pa_signal_iocb(sig);
#endif
}

/** Initialize the UNIX signal subsystem and bind it to the specified main loop */
int pa_signal_init(pa_mainloop_api *api) {

 if ( _roar_pa_signal_inited )
  return -1;

 memset(&_roar_pa_signal, 0, sizeof(_roar_pa_signal));

 _roar_pa_signal.api = api;

#ifdef ROAR_HAVE_PIPE
 if ( pipe(_roar_pa_signal.pipefh) == -1 )
  return -1;

 _roar_pa_signal.io_event = api->io_new(api, _roar_pa_signal.pipefh[0], PA_IO_EVENT_INPUT, _roar_pa_signal_iocb, NULL);
#endif

 _roar_pa_signal_inited = 1;

 return 0;
}

/** Cleanup the signal subsystem */
void pa_signal_done(void) {
 int i;

 for (i = 0; i < MAX_SIG; i++) {
  _roar_pa_signal.sig[i].used = 0;
 }

#ifdef ROAR_HAVE_PIPE
 _roar_pa_signal.api->io_free(_roar_pa_signal.io_event);
#endif

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
