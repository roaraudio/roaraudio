//memlock.h:

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

#ifndef _MEMLOCK_H_
#define _MEMLOCK_H_

#include <roaraudio.h>

#define MEMLOCK_NONE         0
#define MEMLOCK_LOW          1
#define MEMLOCK_MEDIUM       2
#define MEMLOCK_HIGH         3
#define MEMLOCK_NEARLYALL    124
#define MEMLOCK_NEARLYALLSYS 125 /* include stack and heap if possible */
#define MEMLOCK_ALLCUR       126 /* specal as it uses roar_mm_mlockall() excluding future pages */
#define MEMLOCK_ALL          127 /* specal as it uses roar_mm_mlockall() including future pages */

#define MEMLOCK_DEFAULT      MEMLOCK_MEDIUM

int memlock_str2level(const char * str);
int memlock_set_level(int level);
int memlock_register(int level, void * addr, size_t len);
void memlock_unload();
void memlock_reload();

#endif

//ll
