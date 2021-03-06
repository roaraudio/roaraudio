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

#define ROAR_EVENT_NETFLAG_NONE        0x00000000
#define ROAR_EVENT_NETFLAG_DATA        0x00000001
#define ROAR_EVENT_NETFLAG_PROXYEVENT  0x00000002

#define ROAR_NOTIFY_SPECIAL       0xffffffff

// protocol cmds:
#define ROAR_NOTIFY_OFFSET_CMD    0x00000000
#define ROAR_NOTIFY_MASK_CMD      0x000000ff

// event groups (AKA meta events):
#define ROAR_NOTIFY_OFFSET_EGRP    0x00010000
#define ROAR_NOTIFY_MASK_EGRP      0x0000ffff

// object event:
#define ROAR_NOTIFY_OFFSET_OE    0x00020000
#define ROAR_NOTIFY_MASK_OE      0x0000ffff

// user defined events:
#define ROAR_NOTIFY_OFFSET_USER   0x40000000
#define ROAR_NOTIFY_MASK_USER     0x3fffffff

#define ROAR_NOTIFY_X2EVENT(x,offset,mask) ((x) == -1 ? ROAR_NOTIFY_SPECIAL : (((x) & (mask))+(offset)))
#define ROAR_NOTIFY_EVENT2X(x,offset,mask) (ROAR_NOTIFY_IS_X((x),(offset),(mask)) != 1 ? -1 : ((x) - (offset)))
#define ROAR_NOTIFY_IS_X(x,offset,mask)    ((x) == ROAR_NOTIFY_SPECIAL ? -1 : (((x) | (mask)) - (mask)) == (offset) ? 1 : 0)

#define ROAR_NOTIFY_CMD2EVENT(x)  ROAR_NOTIFY_X2EVENT((x), ROAR_NOTIFY_OFFSET_CMD,  ROAR_NOTIFY_MASK_CMD)
#define ROAR_NOTIFY_EVENT2CMD(x)  ROAR_NOTIFY_EVENT2X((x), ROAR_NOTIFY_OFFSET_CMD,  ROAR_NOTIFY_MASK_CMD)
#define ROAR_NOTIFY_IS_CMD(x)     ROAR_NOTIFY_IS_X((x),    ROAR_NOTIFY_OFFSET_CMD,  ROAR_NOTIFY_MASK_CMD)

#define ROAR_NOTIFY_EGRP2EVENT(x) ROAR_NOTIFY_X2EVENT((x), ROAR_NOTIFY_OFFSET_EGRP, ROAR_NOTIFY_MASK_EGRP)
#define ROAR_NOTIFY_EVENT2EGRP(x) ROAR_NOTIFY_EVENT2X((x), ROAR_NOTIFY_OFFSET_EGRP, ROAR_NOTIFY_MASK_EGRP)
#define ROAR_NOTIFY_IS_EGRP(x)    ROAR_NOTIFY_IS_X((x),    ROAR_NOTIFY_OFFSET_EGRP, ROAR_NOTIFY_MASK_EGRP)

#define ROAR_NOTIFY_OE2EVENT(x)   ROAR_NOTIFY_X2EVENT((x), ROAR_NOTIFY_OFFSET_OE, ROAR_NOTIFY_MASK_OE)
#define ROAR_NOTIFY_EVENT2OE(x)   ROAR_NOTIFY_EVENT2X((x), ROAR_NOTIFY_OFFSET_OE, ROAR_NOTIFY_MASK_OE)
#define ROAR_NOTIFY_IS_OE(x)      ROAR_NOTIFY_IS_X((x),    ROAR_NOTIFY_OFFSET_OE, ROAR_NOTIFY_MASK_OE)

#define ROAR_NOTIFY_USER2EVENT(x) ROAR_NOTIFY_X2EVENT((x), ROAR_NOTIFY_OFFSET_USER, ROAR_NOTIFY_MASK_USER)
#define ROAR_NOTIFY_EVENT2USER(x) ROAR_NOTIFY_EVENT2X((x), ROAR_NOTIFY_OFFSET_USER, ROAR_NOTIFY_MASK_USER)
#define ROAR_NOTIFY_IS_USER(x)    ROAR_NOTIFY_IS_X((x),    ROAR_NOTIFY_OFFSET_USER, ROAR_NOTIFY_MASK_USER)

// EGRP:
#define ROAR_NOTIFY_EGRP_GROUP_BASICS     0x0000
#define ROAR_NOTIFY_EGRP_GROUP_CLIENTS    (ROAR_OT_CLIENT << 8)
#define ROAR_NOTIFY_EGRP_GROUP_STREAMS    (ROAR_OT_STREAM << 8)
#define ROAR_NOTIFY_EGRP_GROUP_SOURCES    (ROAR_OT_SOURCE << 8)
#define ROAR_NOTIFY_EGRP_GROUP_SAMPLES    (ROAR_OT_SAMPLE << 8)

#define ROAR_EGRP_ANY_EVENT               ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_BASICS +   0)
#define ROAR_EGRP_OBJECT_NEW              ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_BASICS +   1)
#define ROAR_EGRP_OBJECT_DELETE           ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_BASICS +   2)
#define ROAR_EGRP_OBJECT_REF              ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_BASICS +   3)
#define ROAR_EGRP_OBJECT_UNREF            ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_BASICS +   4)
#define ROAR_EGRP_ERROR                   ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_BASICS +   5)

#define ROAR_EGRP_ANY_CLIENT_EVENT        ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_CLIENTS +   0)

#define ROAR_EGRP_ANY_STREAM_EVENT        ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_CLIENTS +   0)

#define ROAR_EGRP_ANY_SOURCE_EVENT        ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_CLIENTS +   0)

#define ROAR_EGRP_ANY_SAMPLE_EVENT        ROAR_NOTIFY_EGRP2EVENT(ROAR_NOTIFY_EGRP_GROUP_CLIENTS +   0)

// OE:
#define ROAR_NOTIFY_OE_GROUP_BASICS       0x0000
#define ROAR_NOTIFY_OE_GROUP_CLIENTS      (ROAR_OT_CLIENT << 8)
#define ROAR_NOTIFY_OE_GROUP_STREAMS      (ROAR_OT_STREAM << 8)
#define ROAR_NOTIFY_OE_GROUP_SOURCES      (ROAR_OT_SOURCE << 8)
#define ROAR_NOTIFY_OE_GROUP_SAMPLES      (ROAR_OT_SAMPLE << 8)

#define ROAR_OE_BASICS_CHANGE_STATE       ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_BASICS  +   0)
#define ROAR_OE_BASICS_CHANGE_FLAGS       ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_BASICS  +   5)
#define ROAR_OE_BASICS_NEW                ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_BASICS  +   6)
#define ROAR_OE_BASICS_DELETE             ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_BASICS  +   7)

#define ROAR_OE_CLIENT_CONNECT            ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_CLIENTS +   0)
#define ROAR_OE_CLIENT_DISCONNECT         ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_CLIENTS +   1)
#define ROAR_OE_CLIENT_MSG                ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_CLIENTS +   3)

//#define ROAR_OE_STREAM_CHANGE_STATE       ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   0)
#define ROAR_OE_STREAM_EXEC               ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   1)
#define ROAR_OE_STREAM_EOF                ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   2)
#define ROAR_OE_STREAM_CON_STREAM         ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   3)
#define ROAR_OE_STREAM_PASSFH             ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   4)
//#define ROAR_OE_STREAM_CHANGE_FLAGS       ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   5)
#define ROAR_OE_STREAM_CHANGE_VOLUME      ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   6)
#define ROAR_OE_STREAM_XRUN               ROAR_NOTIFY_OE2EVENT(ROAR_NOTIFY_OE_GROUP_STREAMS +   7)

#define ROAR_XRUN_NONE                    0
#define ROAR_XRUN_UNDER_PRE               1
#define ROAR_XRUN_UNDER_POST              2
#define ROAR_XRUN_OVER_PRE                3
#define ROAR_XRUN_OVER_POST               4

#endif

//ll
