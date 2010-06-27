//keyval.c:

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

struct roar_keyval * roar_keyval_lookup (struct roar_keyval *  kv, const char * key, ssize_t len, int casesens) {
 int (*sc)(const char *s1, const char *s2) = strcasecmp;
 ssize_t i;

 if ( casesens )
  sc = strcmp;

 for (i = 0; len != -1 ? (i < len) : kv[i].key != NULL; i++) {
  if ( !sc(key, kv[i].key) )
   return &(kv[i]);
 }

 return NULL;
}

static inline int is_in (const char c, const char * delm) {
 for (; *delm != 0; delm++)
  if ( *delm == c )
   return 1;

 return 0;
}

ssize_t              roar_keyval_split  (struct roar_keyval ** kv, char * str, const char * fdel, const char * kdel, int quotes) {
 struct roar_keyval * kvs;
 int    pos = -1;
 size_t len =  0;
 char * sp;
 char quote =  0;
 int last_was_seg = 0;

 if ( kv == NULL || str == NULL )
  return -1;

 // we currently do not support quotes
 if ( quotes )
  return -1;

 if ( fdel == NULL )
  fdel = " \t,";

 if ( kdel == NULL )
  kdel = "=:";

 // count num of segements:
 for (sp = str; *sp != 0; sp++) {
  if ( quote ) {
   if ( *sp == quote )
    quote = 0;
  } else {
   if ( last_was_seg ) {
    last_was_seg = !is_in(*sp, fdel);
   } else {
    if ( !is_in(*sp, fdel) ) {
     last_was_seg = 1;
     len++;
    }
   }
  }
 }

 kvs = roar_mm_malloc(sizeof(struct roar_keyval)*(len+1));

 if ( kvs == NULL )
  return -1;

 *kv = kvs;

 // End of Array Mark:
 kvs[len].key   = NULL;
 kvs[len].value = NULL;

 // do the acctual filling:
 last_was_seg = 0;

 for (sp = str; *sp != 0; sp++) {
  if ( last_was_seg ) {
   if ( is_in(*sp, fdel) ) {
    last_was_seg = 0;
    *sp = 0;
   } else {
    last_was_seg = 1;
    if ( is_in(*sp, kdel) ) {
     *sp = 0;
     kvs[pos].value = sp+1;
    }
   }
  } else {
   if ( !is_in(*sp, fdel) ) {
    last_was_seg = 1;
    pos++;
    kvs[pos].key   = sp;
    kvs[pos].value = NULL;
   }
  }
 }

 return len;
}

//ll
