//units.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#ifndef _ROARAUDIO_UNITS_H_
#define _ROARAUDIO_UNITS_H_

/* suffix: */
#define _MICRO  (1/1000000)
#define _MILI   (1/1000)
#define _ONE    1
#define _KILO   1000
#define _MEGA   (_KILO*_KILO)
#define _GIGA   (_KILO*_MEGA)
#define _TERA   (_KILO*_GIGA)


/* Time: */
#if defined(_UNITS_T_BASE_MSEC)
#define _SEC  1000
#elif defined(_UNITS_T_BASE_USEC)
#define _SEC  1000000
//#elif defined(_UNITS_T_BASE_SEC)
#else
#define _SEC  1
#endif

#define _MIN  (60*_SEC)
#define _HOUR (60*_MIN)

#define _MSEC (_SEC*_MILI)
#define _USEC (_SEC*_MICRO)


/* distance */
#define _METER 1
#define _AE    (149597870691*_METER)
#define _LJ    (9460730472580800*_METER)
#define _PC    (3.2615668*_LJ)

/* Bits -> Bytes: */
#define _BIT2BYTE(x) (((int)((x)/8)) + ((x) % 8 ? 1 : 0))
#define _BYTE2BIT(x) ((x)*8)

#define _8BIT  _BIT2BYTE(8)
#define _16BIT _BIT2BYTE(16)
#define _24BIT _BIT2BYTE(24)
#define _32BIT _BIT2BYTE(32)
#define _64BIT _BIT2BYTE(64)


#endif

//ll
