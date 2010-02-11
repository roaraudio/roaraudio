//operation.c:

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

/** An asynchronous operation object */
struct pa_operation {
 size_t refc;
 pa_operation_state_t state;
};

pa_operation *roar_pa_operation_new(pa_operation_state_t initstate) {
 pa_operation * o = roar_mm_malloc(sizeof(pa_operation));

 if ( o == NULL )
  return NULL;

 memset(o, 0, sizeof(pa_operation));

 o->refc = 1;

 o->state = initstate;

 return o;
}

static void _operation_free(pa_operation *o) {
 roar_mm_free(o);
}

/** Increase the reference count by one */
pa_operation *pa_operation_ref(pa_operation *o) {
 if ( o == NULL )
  return NULL;

 o->refc++;

 return o;
}

/** Decrease the reference count by one */
void pa_operation_unref(pa_operation *o) {
 if ( o == NULL )
  return;

 o->refc--;

 if ( o->refc < 1 )
  _operation_free(o);
}

/** Cancel the operation. Beware! This will not necessarily cancel the execution of the operation on the server side. */
void pa_operation_cancel(pa_operation *o) {
 // we ignore this
}

/** Return the current status of the operation */
pa_operation_state_t pa_operation_get_state(pa_operation *o) {
 if ( o == NULL )
  return -1;

 return o->state;
}

//ll
