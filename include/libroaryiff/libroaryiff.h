//libroaryiff.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroaryiff a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroaryiff is distributed in the hope that it will be useful,
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

#ifndef _LIBROARYIFF_H_
#define _LIBROARYIFF_H_

#include <roaraudio.h>

#include <Y2/Y.h>           /* Basic Y types and constants. */
#include <Y2/Ylib.h>        /* YLib functions and structs. */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>

// YID 0 is error like -1 on libroar!
#define ROARYIFF_YID2ROAR(x) ((x)-1)
#define ROARYIFF_ROAR2YID(x) ((x)+1)

#endif

//ll
