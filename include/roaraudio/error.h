//error.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE: Even though this file is LGPLed it (may) include GPLed files
 *  so the license of this file is/may therefore downgraded to GPL.
 *  See HACKING for details.
 */

#ifndef _ROARAUDIO_ERROR_H_
#define _ROARAUDIO_ERROR_H_

#define ROAR_ERROR_UNKNOWN     -1 /* Unknown (maybe no) error */
#define ROAR_ERROR_NONE         0
#define ROAR_ERROR_PERM         1 /* Operation not permitted */
#define ROAR_ERROR_NOENT        2 /* No such file or directory */
#define ROAR_ERROR_BADMSG       3 /* Bad message */
#define ROAR_ERROR_BUSY         4 /* Device or resource busy */
#define ROAR_ERROR_CONNREFUSED  5 /* Connection refused */
#define ROAR_ERROR_NOSYS        6 /* Function not implemented */
#define ROAR_ERROR_NOTSUP       7 /* Operation not supported */
#define ROAR_ERROR_PIPE         8 /* Broken pipe */
#define ROAR_ERROR_PROTO        9 /* Protocol error */
#define ROAR_ERROR_RANGE       10 /* Result too large/general out of range */
#define ROAR_ERROR_MSGSIZE     11 /* Message too long */
#define ROAR_ERROR_NOMEM       12 /* Not enough space */
#define ROAR_ERROR_INVAL       13 /* Invalid argument */

#endif

//ll
