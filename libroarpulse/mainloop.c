//mainloop.c:

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

/** Allocate a new main loop object */
pa_mainloop *pa_mainloop_new(void);

/** Free a main loop object */
void pa_mainloop_free(pa_mainloop* m);

/** Prepare for a single iteration of the main loop. Returns a negative value
on error or exit request. timeout specifies a maximum timeout for the subsequent
poll, or -1 for blocking behaviour. .*/
int pa_mainloop_prepare(pa_mainloop *m, int timeout);

/** Execute the previously prepared poll. Returns a negative value on error.*/
int pa_mainloop_poll(pa_mainloop *m);

/** Dispatch timeout, io and deferred events from the previously executed poll. Returns
a negative value on error. On success returns the number of source dispatched. */
int pa_mainloop_dispatch(pa_mainloop *m);

/** Return the return value as specified with the main loop's quit() routine. */
int pa_mainloop_get_retval(pa_mainloop *m);

/** Run a single iteration of the main loop. This is a convenience function
for pa_mainloop_prepare(), pa_mainloop_poll() and pa_mainloop_dispatch().
Returns a negative value on error or exit request. If block is nonzero,
block for events if none are queued. Optionally return the return value as
specified with the main loop's quit() routine in the integer variable retval points
to. On success returns the number of sources dispatched in this iteration. */
int pa_mainloop_iterate(pa_mainloop *m, int block, int *retval);

/** Run unlimited iterations of the main loop object until the main loop's quit() routine is called. */
int pa_mainloop_run(pa_mainloop *m, int *retval);

/** Return the abstract main loop abstraction layer vtable for this main loop. */
pa_mainloop_api* pa_mainloop_get_api(pa_mainloop*m);

/** Shutdown the main loop */
void pa_mainloop_quit(pa_mainloop *m, int r);

/** Interrupt a running poll (for threaded systems) */
void pa_mainloop_wakeup(pa_mainloop *m);

/** Generic prototype of a poll() like function */
typedef int (*pa_poll_func)(struct pollfd *ufds, unsigned long nfds, int timeout, void*userdata);

/** Change the poll() implementation */
void pa_mainloop_set_poll_func(pa_mainloop *m, pa_poll_func poll_func, void *userdata);

//ll
