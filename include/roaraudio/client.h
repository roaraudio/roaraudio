//stream.h:

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

#ifndef _ROARAUDIO_CLIENT_H_
#define _ROARAUDIO_CLIENT_H_

#define ROAR_BUFFER_NAME 80

#define ROAR_CLIENTS_MAX 64
#define ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT 8

#define ROAR_PROTO_NONE             0
#define ROAR_PROTO_ROARAUDIO        1
#define ROAR_PROTO_ESOUND           2

#define ROAR_BYTEORDER_UNKNOWN      0x00
#define ROAR_BYTEORDER_LE           ROAR_CODEC_LE
#define ROAR_BYTEORDER_BE           ROAR_CODEC_BE
#define ROAR_BYTEORDER_PDP          ROAR_CODEC_PDP
#define ROAR_BYTEORDER_NETWORK      ROAR_BYTEORDER_BE

struct roar_client {
 int fh; /* controll connection */
// int last_stream; /* id of the last stream created */
 char name[ROAR_BUFFER_NAME];
 int pid;
 int uid;
 int gid;
 char host[ROAR_BUFFER_NAME];
 int execed;
 int streams[ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT];
 struct roar_acl_target * acl;
 int proto;
 int byteorder;
};

#endif

//ll
