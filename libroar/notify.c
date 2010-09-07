//notify.c:

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

struct roar_subscriber {
 uint32_t flags;
 uint32_t event;
 int emitter;
 int target;
 int target_type;
 void (*cb)(struct roar_event * event, void * userdata);
 void * userdata;
 size_t refc;
 unsigned int hash;
// struct roar_subscriber * prev;
 struct roar_subscriber * next;
};

struct roar_notify_core {
 size_t refc;
 size_t listc;
 struct roar_subscriber ** lists;
};

#define _CKRCORE(ret) if ( core == NULL ) { roar_errno = ROAR_ERROR_INVAL; return (ret); }
#define _CKICORE() _CKRCORE(-1)

static unsigned int _hash_event (struct roar_notify_core * core, struct roar_event * event) {
 unsigned int hash = 0;
 unsigned int mask = core->listc - 1;

 hash ^= event->event >>  0;
 hash ^= event->event >>  8;
 hash ^= event->event >> 16;
 hash ^= event->event >> 24;

 return hash & mask;
}

struct roar_notify_core * roar_notify_core_new(ssize_t lists) {
 struct roar_notify_core * core = NULL;

 switch (lists) {
  case   1:
  case   2:
  case   4:
  case   8:
  case  16:
  case  32:
  case  64:
  case 128:
  case 256:
    // they all are ok.
   break;
  case  -1:
    // handle defult:
    lists = 16;
   break;
  default:
    roar_errno = ROAR_ERROR_INVAL;
    return NULL;
   break;
 }

 if ( (core = roar_mm_malloc(sizeof(struct roar_notify_core))) == NULL ) {
  return NULL;
 }

 memset(core, 0, sizeof(struct roar_notify_core));

 core->refc  = 1;
 core->listc = lists;
 core->lists = roar_mm_malloc(lists*sizeof(struct roar_subscriber *));

 if ( core->lists == NULL ) {
  roar_mm_free(core);
  return NULL;
 }

 return core;
}

int roar_notify_core_ref(struct roar_notify_core * core) {
 _CKICORE();

 core->refc++;

 return 0;
}

int roar_notify_core_unref(struct roar_notify_core * core) {
 struct roar_subscriber * cur, * next;
 size_t i;

 _CKICORE();

 if ( core->refc == 0 ) {
  ROAR_ERR("roar_notify_core_unref(core=%p): refc is zero, must be greater zero. resolving by setting to one.", core);
  core->refc = 1;
 }

 core->refc--;

 if ( core->refc > 0 ) {
  return 0;
 }

 for (i = 0; i < core->listc; i++) {
  cur = core->lists[i];
  while (cur != NULL) {
   next = cur->next;

   roar_mm_free(cur);

   cur = next;
  }
 }

 roar_mm_free(core->lists);
 roar_mm_free(core);
 return 0;
}

struct roar_subscriber * roar_notify_core_subscribe(struct roar_notify_core * core, struct roar_event * event, void (*cb)(struct roar_event * event, void * userdata), void * userdata) {
 struct roar_subscriber * subs = NULL;
 struct roar_subscriber * cur, * old;

 _CKRCORE(NULL);

 if ( event == NULL || cb == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return NULL;
 }

 if ( (subs = roar_mm_malloc(sizeof(struct roar_subscriber))) == NULL ) {
  return NULL;
 }

 memset(subs, 0, sizeof(struct roar_subscriber));

 subs->flags       = 0;
 subs->event       = event->event;
 subs->emitter     = event->emitter;
 subs->target      = event->target;
 subs->target_type = event->target_type;
 subs->cb          = cb;
 subs->userdata    = userdata;
 subs->refc        = 1;
 subs->hash        = _hash_event(core, event);
 subs->next        = NULL;

 if ( (cur = core->lists[subs->hash]) == NULL ) {
  core->lists[subs->hash] = subs;
 } else {
  old = cur;
  while (cur != NULL) {
   old = cur;
   cur = cur->next;
  }

  old->next = subs;
 }

 return subs;
}

int roar_notify_core_unsubscribe(struct roar_notify_core * core, struct roar_subscriber * subscriber) {
 struct roar_subscriber * cur, * old;

 _CKICORE();

 if ( subscriber ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 if ( (cur = core->lists[subscriber->hash]) == subscriber ) {
  core->lists[subscriber->hash] = core->lists[subscriber->hash]->next;
 } else {
  old = NULL;
  while (cur != NULL && cur != subscriber ) {
   old = cur;
   cur = cur->next;
  }

  if ( cur != subscriber )
   return -1;

  old->next = cur->next;
  roar_mm_free(cur);
 }

 return 0;
}

int roar_notify_core_emit(struct roar_notify_core * core, struct roar_event * event) {
 struct roar_subscriber * cur;

 _CKICORE();

 if ( event ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 cur = core->lists[_hash_event(core, event)];

 while (cur != NULL) {
  if ( cur->cb == NULL ) {
   ROAR_ERR("roar_notify_core_emit(core=%p, event=%p): cur=%p, cb is set NULL, bad.", core, event, cur);
  } else {
   cur->cb(event, cur->userdata);
  }
  cur = cur->next;
 }

 return 0;
}

//ll
