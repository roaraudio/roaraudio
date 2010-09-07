//notify_proxy.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
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
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

#define MAX_EVENTS_PER_GROUP  4
#define EOL                   ROAR_NOTIFY_SPECIAL

void roar_notify_proxy_std(struct roar_notify_core * core, struct roar_event * event, void * userdata) {
 struct {
  uint32_t event_min, event_max;
  uint32_t events[MAX_EVENTS_PER_GROUP];
 } * ptr, list[] = {
  {.event_min = ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS),
   .event_max = ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS + 0xff),
   .events = {
    EOL
   }
  }
 };
 struct roar_event locevent;
 size_t i, e;
 register uint32_t oe = event->event;

 (void)userdata;

 memcpy(&locevent, event, sizeof(locevent));

 locevent.event_proxy  = locevent.event;
 locevent.flags       |= ROAR_EVENT_FLAG_PROXYEVENT;

 locevent.event = ROAR_EGRP_ANY_EVENT;

 roar_notify_core_emit(core, &locevent);


 for (i = 0; i < sizeof(list)/sizeof(*list); i++) {
  ptr = &(list[i]);
  if ( ptr->event_min <= oe && ptr->event_max >= oe ) {
   for (e = 0; ptr->events[e] != EOL; e++) {
    locevent.event = ptr->events[e];
    roar_notify_core_emit(core, &locevent);
   }
  }
 }
}

//ll
