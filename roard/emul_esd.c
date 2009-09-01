//emul_esd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

#ifndef ROAR_WITHOUT_DCOMP_EMUL_ESD
#ifdef ROAR_HAVE_ESD

#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_MINIMAL)
#define _NAME(x) (x)
#else
#define _NAME(x) ((char*)NULL)
#endif

struct emul_esd_command g_emul_esd_commands[] = {
 {ESD_PROTO_CONNECT,      ESD_KEY_LEN  +     sizeof(int), _NAME("CONNECT"),      NULL},
 {ESD_PROTO_LOCK,         ESD_KEY_LEN  +     sizeof(int), _NAME("LOCK"),         NULL},
 {ESD_PROTO_UNLOCK,       ESD_KEY_LEN  +     sizeof(int), _NAME("UNLOCK"),       NULL},
 {ESD_PROTO_STREAM_PLAY,  ESD_NAME_MAX + 2 * sizeof(int), _NAME("STREAM_PLAY"),  NULL},
 {ESD_PROTO_STREAM_REC,   ESD_NAME_MAX + 2 * sizeof(int), _NAME("STREAM_REC"),   NULL},
 {ESD_PROTO_STREAM_MON,   ESD_NAME_MAX + 2 * sizeof(int), _NAME("STREAM_MON"),   NULL},
 {ESD_PROTO_SAMPLE_CACHE, ESD_NAME_MAX + 3 * sizeof(int), _NAME("SAMPLE_CACHE"), NULL},
 {ESD_PROTO_SAMPLE_FREE,                     sizeof(int), _NAME("SAMPLE_FREE"),  NULL},
 {ESD_PROTO_SAMPLE_PLAY,                     sizeof(int), _NAME("SAMPLE_PLAY"),  NULL},
 {ESD_PROTO_SAMPLE_LOOP,                     sizeof(int), _NAME("SAMPLE_LOOP"),  NULL},
 {ESD_PROTO_SAMPLE_STOP,                     sizeof(int), _NAME("SAMPLE_STOP"),  NULL},
 {ESD_PROTO_SAMPLE_KILL,  0                             , _NAME("SAMPLE_KILL"),  NULL},
 {ESD_PROTO_STANDBY,      ESD_KEY_LEN +      sizeof(int), _NAME("STANDBY"),      NULL},
 {ESD_PROTO_RESUME,       ESD_KEY_LEN +      sizeof(int), _NAME("RESUME"),       NULL},
 {ESD_PROTO_SAMPLE_GETID, ESD_NAME_MAX,                   _NAME("SAMPLE_GETID"), NULL},
 {ESD_PROTO_STREAM_FILT,  ESD_NAME_MAX + 2 * sizeof(int), _NAME("STREAM_FILT"),  NULL},
 {ESD_PROTO_SERVER_INFO,                     sizeof(int), _NAME("SERVER_INFO"),  NULL},
 {ESD_PROTO_ALL_INFO,                        sizeof(int), _NAME("ALL_INFO"),     NULL},
 {ESD_PROTO_SUBSCRIBE,    0                             , _NAME("SUBSCRIBE"),    NULL},
 {ESD_PROTO_UNSUBSCRIBE,  0                             , _NAME("UNSUBSCRIBE"),  NULL},
 {ESD_PROTO_STREAM_PAN,                  3 * sizeof(int), _NAME("STREAM_PAN"),   NULL},
 {ESD_PROTO_SAMPLE_PAN,                  3 * sizeof(int), _NAME("SAMPLE_PAN"),   NULL},
 {ESD_PROTO_STANDBY_MODE,                    sizeof(int), _NAME("STANDBY_MODE"), NULL},
 {ESD_PROTO_LATENCY,      0                             , _NAME("LATENCY"),      NULL},
 {-1, 0, _NAME("END OF LIST"), NULL}
};

#endif
#endif

//ll
