//raorfish.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarfish a part of RoarAudio,
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

#ifndef _ROARFISH_H_
#define _RAORFISH_H_

#define __USE_XOPEN

#include <roaraudio.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <limits.h>
#include <math.h>

#include <oggz/oggz.h>
#include <fishsound/fishsound.h>
#include <sndfile.h>

int play (char * file, char ** opts);

#ifdef __USE_SVID
#define MAXFLOAT HUGE
#endif

#endif

//ll
