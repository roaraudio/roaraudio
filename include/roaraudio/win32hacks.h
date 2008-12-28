//win32hacks.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE: Even though this file is LGPLed it (may) include GPLed files
 *  so the license of this file is/may therefore downgraded to GPL.
 *  See HACKING for details.
 */

#ifndef _ROARAUDIO_WIN32HACKS_H_
#define _ROARAUDIO_WIN32HACKS_H_

#ifdef ROAR_TARGET_WIN32

#include <windows.h>

#define id_t      int
#define pid_t     int
#define uid_t     int
#define gid_t     int
#define socklen_t int

#define SHUT_RD SD_RECEIVE
#define SHUT_WR SD_SEND


#define ROAR_NETWORK_READ(x,y,z)  recv((x), (y), (z), 0)
#define ROAR_NETWORK_WRITE(x,y,z) send((x), (y), (z), 0)
#else
#define ROAR_NETWORK_READ(x,y,z)  read((x), (y), (z))
#define ROAR_NETWORK_WRITE(x,y,z) write((x), (y), (z))
#endif

#endif

//ll
