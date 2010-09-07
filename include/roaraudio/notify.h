//notify.h:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE: Even though this file is LGPLed it (may) include GPLed files
 *  so the license of this file is/may therefore downgraded to GPL.
 *  See HACKING for details.
 */

#ifndef _ROARAUDIO_NOTIFY_H_
#define _ROARAUDIO_NOTIFY_H_

#define ROAR_NOTIFY_SPECIAL       0xffffffff

// protocol cmds:
#define ROAR_NOTIFY_OFFSET_CMD    0x00000000
#define ROAR_NOTIFY_MASK_CMD      0x000000ff

// event groups (AKA meta events):
#define ROAR_NOTIFY_OFFSET_EGRP    0x00010000
#define ROAR_NOTIFY_MASK_EGRP      0x0000ffff

// user defined events:
#define ROAR_NOTIFY_OFFSET_USER   0x40000000
#define ROAR_NOTIFY_MASK_CUSER    0x3fffffff

#define ROAR_NOTIFY_X2EVENT(x,offset,mask) ((x) == -1 ? ROAR_NOTIFY_SPECIAL : (((x) & (mask))+(offset)))
#define ROAR_NOTIFY_EVENT2X(x,offset,mask) (ROAR_NOTIFY_ISX((x),(offset),(mask)) != 1 ? -1 : ((x) - (offset)))
#define ROAR_NOTIFY_IS_X(x,offset,mask)    ((x) == ROAR_NOTIFY_SPECIAL ? -1 : (((x) | (mask)) - (mask)) == (offset) ? 1 : 0)

#define ROAR_NOTIFY_CMD2EVENT(x)  ROAR_NOTIFY_X2EVENT((x), ROAR_NOTIFY_OFFSET_CMD,  ROAR_NOTIFY_MASK_CMD)
#define ROAR_NOTIFY_EVENT2CMD(x)  ROAR_NOTIFY_EVENT2X((x), ROAR_NOTIFY_OFFSET_CMD,  ROAR_NOTIFY_MASK_CMD)
#define ROAR_NOTIFY_IS_CMD(x)     ROAR_NOTIFY_IS_X((x),    ROAR_NOTIFY_OFFSET_CMD,  ROAR_NOTIFY_MASK_CMD)

#define ROAR_NOTIFY_USER2EVENT(x) ROAR_NOTIFY_X2EVENT((x), ROAR_NOTIFY_OFFSET_USER, ROAR_NOTIFY_MASK_USER)
#define ROAR_NOTIFY_EVENT2USER(x) ROAR_NOTIFY_EVENT2X((x), ROAR_NOTIFY_OFFSET_USER, ROAR_NOTIFY_MASK_USER)
#define ROAR_NOTIFY_IS_USER(x)    ROAR_NOTIFY_IS_X((x),    ROAR_NOTIFY_OFFSET_USER, ROAR_NOTIFY_MASK_USER)

#endif

//ll
