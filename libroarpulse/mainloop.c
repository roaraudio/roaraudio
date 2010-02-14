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
#ifdef ROAR_HAVE_H_POLL
#include <poll.h>
#endif

#define MAX_IO_EVENTS    8

#ifndef ROAR_HAVE_H_POLL
struct pollfd {
 int fd;
 short events, revents;
};
#endif

struct pa_io_event {
 int used;
 pa_mainloop_api *api;
 int fd;
 pa_io_event_flags_t events;
 pa_io_event_cb_t cb;
 void *userdata;
 pa_io_event_destroy_cb_t destroy;
};

struct pa_mainloop {
 pa_mainloop_api api;
 pa_poll_func    poll_func;
 void          * poll_userdata;
 int             poll_timeout;
 int             quit;
 int             quitval;
 pa_io_event     io_event[MAX_IO_EVENTS];
 struct pollfd   pollfd[MAX_IO_EVENTS];
 nfds_t          pollfds;
};

// IO EVENTS:
static void _roar_pa_io_enable(pa_io_event* e, pa_io_event_flags_t events);

/** Create a new IO event source object */
static pa_io_event* _roar_pa_io_new(pa_mainloop_api*a, int fd,
                                    pa_io_event_flags_t events, pa_io_event_cb_t cb, void *userdata) {
 pa_mainloop * mainloop = a->userdata;
 pa_io_event * ret = NULL;
 int i;

 for (i = 0; i < MAX_IO_EVENTS; i++) {
  if ( mainloop->io_event[i].used == 0 ) {
   ret = &(mainloop->io_event[i]);
  }
 }

 if ( ret == NULL )
  return NULL;

 ret->used = 1;

 ret->api      = a;
 ret->fd       = fd;
 ret->cb       = cb;
 ret->userdata = userdata;

 // Callbacks:
 ret->destroy  = NULL;

 _roar_pa_io_enable(ret, events);

 return ret;
}
/** Enable or disable IO events on this object */ 
static void _roar_pa_io_enable(pa_io_event* e, pa_io_event_flags_t events) {
 if ( e == NULL )
  return;

 e->events = events;
}

/** Free a IO event source object */
static void _roar_pa_io_free(pa_io_event* e) {
 if ( e == NULL )
  return;

 if ( e->destroy != NULL )
  e->destroy(e->api, e, e->userdata);

 e->used = 0;
}
/** Set a function that is called when the IO event source is destroyed. Use this to free the userdata argument if required */
static void _roar_pa_io_set_destroy(pa_io_event *e, pa_io_event_destroy_cb_t cb) {
 if ( e == NULL )
  return;

 e->destroy = cb;
}



// API:

void _roar_pa_mainloop_quit(pa_mainloop_api*a, int retval) {
 pa_mainloop * m = a->userdata;
 pa_mainloop_quit(m, retval);
}

/** Allocate a new main loop object */
pa_mainloop *pa_mainloop_new(void) {
 pa_mainloop * m = roar_mm_malloc(sizeof(pa_mainloop));

 ROAR_DBG("pa_mainloop_new() = ?");

 if ( m == NULL )
  return NULL;

 memset(m, 0, sizeof(pa_mainloop));

 m->api.userdata       = m;

 m->api.quit           = _roar_pa_mainloop_quit;

 m->api.io_new         = _roar_pa_io_new;
 m->api.io_enable      = _roar_pa_io_enable;
 m->api.io_free        = _roar_pa_io_free;
 m->api.io_set_destroy = _roar_pa_io_set_destroy;

 ROAR_DBG("pa_mainloop_new() = %p", m);
 return m;
}

/** Free a main loop object */
void pa_mainloop_free(pa_mainloop* m) {
 ROAR_DBG("pa_mainloop_free(m=%p) = ?", m);

 if ( m == NULL )
  return;

 roar_mm_free(m);

 ROAR_DBG("pa_mainloop_free(m=%p) = (void)", m);
}

/** Prepare for a single iteration of the main loop. Returns a negative value
on error or exit request. timeout specifies a maximum timeout for the subsequent
poll, or -1 for blocking behaviour. .*/
int pa_mainloop_prepare(pa_mainloop *m, int timeout) {
 short events;
 int i;

 ROAR_DBG("pa_mainloop_prepare(m=%p, timeout=%i) = ?", m, timeout);

 if ( m == NULL )
  return -1;

 if ( m->quit )
  return -2;

 m->pollfds = 0;

 for (i = 0; i < MAX_IO_EVENTS; i++) {
  if ( m->io_event[i].used ) {
   events = 0;

   if ( m->io_event[i].events & PA_IO_EVENT_INPUT )
    events |= POLLIN;

   if ( m->io_event[i].events & PA_IO_EVENT_OUTPUT )
    events |= POLLOUT;

   if ( m->io_event[i].events & PA_IO_EVENT_HANGUP )
    events |= POLLHUP;

   if ( m->io_event[i].events & PA_IO_EVENT_ERROR )
    events |= POLLERR;

   if ( events == 0 )
    continue;

   m->pollfd[m->pollfds].fd     = m->io_event[i].fd;
   m->pollfd[m->pollfds].events = events;
   m->pollfds++;
  }
 }

 m->poll_timeout = timeout;

 ROAR_DBG("pa_mainloop_prepare(m=%p, timeout=%i) = 0", m, timeout);
 return 0;
}

/** Execute the previously prepared poll. Returns a negative value on error.*/
int pa_mainloop_poll(pa_mainloop *m) {
 int ret;
 int alive = 1;

 ROAR_DBG("pa_mainloop_poll(m=%p) = ?", m);

 if ( m == NULL )
  return -1;

 if ( m->quit )
  return -2;

 while (alive) {
  if ( m->poll_func != NULL ) {
   ret = m->poll_func(m->pollfd, m->pollfds, m->poll_timeout, m->poll_userdata);
  } else {
#ifdef ROAR_HAVE_H_POLL
   ret = poll(m->pollfd, m->pollfds, m->poll_timeout);
#else
   ret = -1;
#endif
  }

  if ( ret != -1 || ( errno != EAGAIN && errno != EINTR ) ) {
   alive = 0;
  }
 }

 ROAR_DBG("pa_mainloop_poll(m=%p) = %i", m, ret);
 return ret;
}

/** Dispatch timeout, io and deferred events from the previously executed poll. Returns
a negative value on error. On success returns the number of source dispatched. */
int pa_mainloop_dispatch(pa_mainloop *m) {
 pa_io_event_flags_t events;
 int count = 0;
 int i, h;

 ROAR_DBG("pa_mainloop_dispatch(m=%p) = ?", m);

 if ( m == NULL )
  return -1;

 if ( m->quit )
  return -2;

 for (i = 0; i < m->pollfds; i++) {
  if ( m->pollfd[i].revents != 0 ) {
   for (h = 0; h < MAX_IO_EVENTS; h++) {
    if ( m->io_event[h].fd == m->pollfd[i].fd ) {
     events = PA_IO_EVENT_NULL;

     if ( m->pollfd[i].revents & POLLIN )
      events |= PA_IO_EVENT_INPUT;

     if ( m->pollfd[i].revents & POLLOUT )
      events |= PA_IO_EVENT_OUTPUT;

     if ( m->pollfd[i].revents & POLLHUP )
      events |= PA_IO_EVENT_HANGUP;

     if ( m->pollfd[i].revents & POLLERR )
      events |= PA_IO_EVENT_ERROR;

     if ( m->io_event[h].cb != NULL )
      m->io_event[h].cb(&(m->api), &(m->io_event[h]), m->pollfd[i].fd, events, m->io_event[h].userdata);

     count++;
    }
   }
  }
 }

 ROAR_DBG("pa_mainloop_dispatch(m=%p) = %i", m, count);
 return count;
}

/** Return the return value as specified with the main loop's quit() routine. */
int pa_mainloop_get_retval(pa_mainloop *m) {
 if ( m == NULL )
  return -1;

 return m->quitval;
}

/** Run a single iteration of the main loop. This is a convenience function
for pa_mainloop_prepare(), pa_mainloop_poll() and pa_mainloop_dispatch().
Returns a negative value on error or exit request. If block is nonzero,
block for events if none are queued. Optionally return the return value as
specified with the main loop's quit() routine in the integer variable retval points
to. On success returns the number of sources dispatched in this iteration. */
int pa_mainloop_iterate(pa_mainloop *m, int block, int *retval) {
 int r;

 ROAR_DBG("pa_mainloop_iterate(m=%p, block=%i, retval=%p) = ?", m, block, retval);

 if ( m == NULL )
  return -1;

 r = pa_mainloop_prepare(m, block ? -1 : 0);

 if ( r >= 0 )
  r = pa_mainloop_poll(m);

 if ( r > 0 )
  r = pa_mainloop_dispatch(m);

 if ( r == -2 && retval != NULL ) {
  *retval = m->quitval;
 }

 ROAR_DBG("pa_mainloop_iterate(m=%p, block=%i, retval=%p) = %i", m, block, retval, r);
 return r;
}

/** Run unlimited iterations of the main loop object until the main loop's quit() routine is called. */
int pa_mainloop_run(pa_mainloop *m, int *retval) {
 int r = 1;

 ROAR_DBG("pa_mainloop_run(m=%p, retval=%p) = ?", m, retval);

 if ( m == NULL )
  return -1;

 while (!(m->quit) && r > 0) {
  r = pa_mainloop_iterate(m, 1, retval);
 }

 ROAR_DBG("pa_mainloop_run(m=%p, retval=%p): r=%i", m, retval, r);

 if ( r == -2 )
  return 1;

 if ( r < 0 )
  return -1;

 return 0;
}

/** Return the abstract main loop abstraction layer vtable for this main loop. */
pa_mainloop_api* pa_mainloop_get_api(pa_mainloop*m) {
 if ( m == NULL )
  return NULL;

 return &(m->api);
}

/** Shutdown the main loop */
void pa_mainloop_quit(pa_mainloop *m, int r) {
 if ( m == NULL )
  return;

 m->quitval = r;
 m->quit    = 1;
}

/** Interrupt a running poll (for threaded systems) */
void pa_mainloop_wakeup(pa_mainloop *m);

/** Change the poll() implementation */
void pa_mainloop_set_poll_func(pa_mainloop *m, pa_poll_func poll_func, void *userdata) {
 if ( m == NULL )
  return;

 m->poll_func     = poll_func;
 m->poll_userdata = userdata;
}

//ll
