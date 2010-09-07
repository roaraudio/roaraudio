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
 void (*cb)(struct roar_notify_core * core, struct roar_event * event, void * userdata);
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
 void (*proxy)(struct roar_notify_core * core, struct roar_event * event, void * userdata);
 void * proxy_userdata;
};

static struct roar_notify_core * _libroar_notify_core = NULL;

#define _CKRCORE(ret) if ( core == NULL ) { if ( _libroar_notify_core == NULL ) { roar_errno = ROAR_ERROR_INVAL; return (ret); } else { core = _libroar_notify_core; } }
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
  case    1:
  case    2:
  case    4:
  case    8:
  case   16:
  case   32:
  case   64:
  case  128:
  case  256:
  case  512:
  case 1024:
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
 core->proxy = NULL;
 core->proxy_userdata = NULL;

 if ( core->lists == NULL ) {
  roar_mm_free(core);
  return NULL;
 }

 memset(core->lists, 0, lists*sizeof(struct roar_subscriber *));

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

int roar_notify_core_new_global(ssize_t lists) {
 if ( _libroar_notify_core != NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 if ( (_libroar_notify_core = roar_notify_core_new(lists)) == NULL )
  return -1;

 return 0;
}

int roar_notify_core_register_proxy(struct roar_notify_core * core, void (*cb)(struct roar_notify_core * core, struct roar_event * event, void * userdata), void * userdata) {
 _CKICORE();

 core->proxy = cb;
 core->proxy_userdata = userdata;

 return 0;
}


struct roar_subscriber * roar_notify_core_subscribe(struct roar_notify_core * core, struct roar_event * event, void (*cb)(struct roar_notify_core * core, struct roar_event * event, void * userdata), void * userdata) {
 struct roar_subscriber * subs = NULL;
 struct roar_subscriber * cur, * old;

 ROAR_DBG("roar_notify_core_subscribe(core=%p, event=%p, cb=%p, userdata=%p) = ?", core, event, cb, userdata);

 _CKRCORE(NULL);

 if ( event == NULL || cb == NULL ) {
  ROAR_DBG("roar_notify_core_subscribe(core=%p, event=%p, cb=%p, userdata=%p) = NULL // errno = EINVAL", core, event, cb, userdata);
  roar_errno = ROAR_ERROR_INVAL;
  return NULL;
 }

 if ( (subs = roar_mm_malloc(sizeof(struct roar_subscriber))) == NULL ) {
  ROAR_DBG("roar_notify_core_subscribe(core=%p, event=%p, cb=%p, userdata=%p) = NULL // errno = ENOMEM?", core, event, cb, userdata);
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

 ROAR_DBG("roar_notify_core_subscribe(core=%p, event=%p, cb=%p, userdata=%p) = %p", core, event, cb, userdata, subs);
 return subs;
}

int roar_notify_core_unsubscribe(struct roar_notify_core * core, struct roar_subscriber * subscriber) {
 struct roar_subscriber * cur, * old;

 _CKICORE();

 if ( subscriber == NULL ) {
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

 if ( event == NULL ) {
  roar_errno = ROAR_ERROR_INVAL;
  return -1;
 }

 if ( core->proxy != NULL && !(event->flags & ROAR_EVENT_FLAG_PROXYEVENT) ) {
  core->proxy(core, event, core->proxy_userdata);
 }

 cur = core->lists[_hash_event(core, event)];
 ROAR_DBG("roar_notify_core_emit(core=%p, event=%p): cur=%p", core, event, cur);

 while (cur != NULL) {
  ROAR_DBG("roar_notify_core_emit(core=%p, event=%p): cur=%p", core, event, cur);

  // test if we can skip this one:
  if ( !( (cur->event       == ROAR_NOTIFY_SPECIAL || cur->event       == event->event)   &&
          (cur->emitter     == -1                  || cur->emitter     == event->emitter) &&
          (cur->target      == -1                  || cur->target      == event->target)  &&
          (cur->target_type == -1                  || cur->target_type == event->target_type)
        ) ) {
   cur = cur->next;
   continue;
  }

  if ( cur->cb == NULL ) {
   ROAR_ERR("roar_notify_core_emit(core=%p, event=%p): cur=%p, cb is set NULL, bad.", core, event, cur);
  } else {
   cur->cb(core, event, cur->userdata);
  }
  cur = cur->next;
 }

 return 0;
}

int roar_notify_core_emit_simple(uint32_t event, int emitter, int target, int target_type, int arg0, int arg1, void * arg2, ssize_t arg2_len) {
 struct roar_event locevent;

 memset(&locevent, 0, sizeof(locevent));

 locevent.event = event;
 locevent.emitter = emitter;
 locevent.target = target;
 locevent.target_type = target_type;
 locevent.arg0 = arg0;
 locevent.arg1 = arg1;
 locevent.arg2 = arg2;
 locevent.arg2_len = arg2_len;

 return roar_notify_core_emit(NULL, &locevent);
}

//ll
