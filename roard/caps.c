//caps.c:

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

static uint32_t _g_caps_standards[] = {
 // message formats:
 ROAR_STD_MAKE(ROAR_STDV_ROARAUDIO,  0, 0), // v. 0

 // command sets:
 ROAR_STD_MAKE(ROAR_STDV_ROARAUDIO,  4, 0), // basic commands

 // well known numbers:
 ROAR_STD_MAKE(ROAR_STDV_ROARAUDIO, 12, 0), // codecs

 // transports:
#ifdef ROAR_HAVE_UNIX
 ROAR_STD_MAKE(ROAR_STDV_ROARAUDIO, 23, 0), // UNIX
#endif
#ifdef ROAR_HAVE_LIBDNET
 ROAR_STD_MAKE(ROAR_STDV_ROARAUDIO, 24, 0), // DECnet
#endif
#ifdef ROAR_HAVE_IPV4
 ROAR_STD_MAKE(ROAR_STDV_ROARAUDIO, 25, 0), // TCP
#endif

 0xFFFFFFFF // terminater
};

struct roar_stds g_caps_stds = {
 .stds_len = (sizeof(_g_caps_standards)/sizeof(*_g_caps_standards)) - 1,
 .stds     = _g_caps_standards
};

//ll
