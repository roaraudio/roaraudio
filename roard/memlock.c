//memlock.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of roard a part of RoarAudio,
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
 */

#include "roard.h"

#define MAX_SEGMENTS 16

struct memlock {
 int level;
 void * addr;
 size_t len;
};

static struct memlock memlock_table[MAX_SEGMENTS];

static volatile int memlock_table_inited = 0;

static void memlock_table_init (void) {
 if ( memlock_table_inited )
  return;

 memset(memlock_table, 0, sizeof(memlock_table));

 memlock_table_inited = 1;

 memlock_register(MEMLOCK_MEDIUM, memlock_table, sizeof(memlock_table));
}

int memlock_register(int level, void * addr, size_t len) {
 int i;

 if ( !memlock_table_inited )
  memlock_table_init();

 for (i = 0; i < MAX_SEGMENTS; i++) {
  if ( memlock_table[i].addr == NULL ) {
   memlock_table[i].level = level;
   memlock_table[i].addr  = addr;
   memlock_table[i].len   = len;
   return 0;
  }
 }

 return -1;
}

static inline int memlock_lock(struct memlock * seg) {
 return roar_mm_mlock(seg->addr, seg->len);
}

static inline int memlock_unlock(struct memlock * seg) {
 return roar_mm_munlock(seg->addr, seg->len);
}

int memlock_str2level(const char * str) {
 if ( str == NULL )
  return -1;

 if ( !strcasecmp(str, "none") ) {
  return MEMLOCK_NONE;
 } else if ( !strcasecmp(str, "low") ) {
  return MEMLOCK_LOW;
 } else if ( !strcasecmp(str, "medium") ) {
  return MEMLOCK_MEDIUM;
 } else if ( !strcasecmp(str, "all") ) {
  return MEMLOCK_ALL;
 } else if ( !strcasecmp(str, "default") ) {
  return MEMLOCK_DEFAULT;
 } else {
  return -1;
 }
}

int memlock_set_level(int level) {
 static int old_level = MEMLOCK_NONE;
 int i;
 int ret = 0;

 if ( !memlock_table_inited )
  memlock_table_init();

 if ( level == old_level )
  return 0;

 for (i = 0; i < MAX_SEGMENTS; i++) {
  if ( memlock_table[i].addr == NULL ) {
   if ( level > old_level ) {
    if ( memlock_table[i].level > old_level && memlock_table[i].level <= level )
     if ( memlock_lock(&(memlock_table[i])) == -1 )
      ret = -1;
   } else {
    if ( memlock_table[i].level <= old_level && memlock_table[i].level > level )
     if ( memlock_unlock(&(memlock_table[i])) == -1 )
      ret = -1;
   }
  }
 }

 old_level = level;

 return ret;
}

void memlock_unload() {
}

void memlock_reload() {
}

//ll
