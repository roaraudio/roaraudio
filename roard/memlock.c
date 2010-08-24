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

 if ( level == old_level )
  return 0;

 return -1;
}

void memlock_unload() {
}

void memlock_reload() {
}

//ll
