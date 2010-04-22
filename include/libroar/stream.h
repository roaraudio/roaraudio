//stream.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARSTREAM_H_
#define _LIBROARSTREAM_H_

#include "roaraudio.h"

char * roar_dir2str (int dir);
int    roar_str2dir (char * name);

#define ROAR_FLAG_NONE           0x0000
#define ROAR_FLAG_PRIMARY        0x0001
#define ROAR_FLAG_OUTPUT         0x0002
#define ROAR_FLAG_DRIVER         ROAR_FLAG_OUTPUT
#define ROAR_FLAG_SOURCE         0x0004
#define ROAR_FLAG_SYNC           0x0008
#define ROAR_FLAG_META           0x0010
#define ROAR_FLAG_AUTOCONF       0x0020
#define ROAR_FLAG_CLEANMETA      0x0040
#define ROAR_FLAG_HWMIXER        0x0080
#define ROAR_FLAG_PAUSE          0x0100
#define ROAR_FLAG_MUTE           0x0200
#define ROAR_FLAG_MMAP           0x0400
#define ROAR_FLAG_ANTIECHO       0x0800
#define ROAR_FLAG_VIRTUAL        0x1000
#define ROAR_FLAG_RECSOURCE      0x2000
#define ROAR_FLAG_PASSMIXER      0x4000
#define ROAR_FLAG_PRETHRU        0x8000
// next are the exteded flags (> 16 bits)
//#define ROAR_FLAG_SYNC           0x08
#define ROAR_FLAG_IMMUTABLE      0x00010000
#define ROAR_FLAG_ENHANCE        0x00020000

#define ROAR_SET_FLAG            0
#define ROAR_RESET_FLAG          1

#define ROAR_STREAMSTATE_UNKNOWN   -1
#define ROAR_STREAMSTATE_NULL       0
#define ROAR_STREAMSTATE_UNUSED     ROAR_STREAMSTATE_NULL
#define ROAR_STREAMSTATE_INITING    1
#define ROAR_STREAMSTATE_NEW        2
#define ROAR_STREAMSTATE_OLD        3
#define ROAR_STREAMSTATE_CLOSING    4

// PA currently defines: video, music, game, event, phone, animation, production, a11y
// RA includes         : YES    YES    YES   YES    YES    NO         NO          NO

#define ROAR_ROLE_UNKNOWN          -1
#define ROAR_ROLE_NONE              0
#define ROAR_ROLE_MUSIC             1
#define ROAR_ROLE_VIDEO             2
#define ROAR_ROLE_GAME              3
#define ROAR_ROLE_EVENT             4
#define ROAR_ROLE_BEEP              5
#define ROAR_ROLE_PHONE             6
#define ROAR_ROLE_BACKGROUND_MUSIC  7
#define ROAR_ROLE_VOICE             8
#define ROAR_ROLE_INSTRUMENT        9

struct roar_stream_info {
 int block_size;
 int pre_underruns;
 int post_underruns;
 int codec;
 uint32_t flags;
 char * driver;
 uint_least32_t delay;
 int state;
 int mixer;
 int role;
};

int roar_stream_connect (struct roar_connection * con, struct roar_stream * s, int dir);

int roar_stream_new     (struct roar_stream * s, unsigned int rate, unsigned int channels, unsigned int bits, unsigned int codec);

int roar_stream_set_rel_id(struct roar_stream * s, int id);
int roar_stream_get_rel_id(struct roar_stream * s);

int roar_stream_new_by_id(struct roar_stream * s, int id);
int roar_stream_new_empty(struct roar_stream * s);

int roar_stream_set_id (struct roar_stream * s, int id);
int roar_stream_get_id (struct roar_stream * s);

int roar_stream_set_fh (struct roar_stream * s, int fh);
int roar_stream_get_fh (struct roar_stream * s);

int roar_stream_set_dir (struct roar_stream * s, int dir);
int roar_stream_get_dir (struct roar_stream * s);

int roar_stream_exec    (struct roar_connection * con, struct roar_stream * s);
int roar_stream_connect_to (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port);
int roar_stream_connect_to_ask (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port);
int roar_stream_passfh  (struct roar_connection * con, struct roar_stream * s, int fh);

int roar_stream_attach_simple (struct roar_connection * con, struct roar_stream * s, int client);

int roar_stream_add_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len);

int roar_stream_send_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len);

int roar_stream_get_info (struct roar_connection * con, struct roar_stream * s, struct roar_stream_info * info);
int roar_stream_get_name (struct roar_connection * con, struct roar_stream * s, char * name, size_t len);

int roar_stream_get_chanmap (struct roar_connection * con, struct roar_stream * s, char * map, size_t * len);
int roar_stream_set_chanmap (struct roar_connection * con, struct roar_stream * s, char * map, size_t   len);

int roar_stream_set_flags (struct roar_connection * con, struct roar_stream * s, int flags, int reset);

int roar_stream_set_role  (struct roar_connection * con, struct roar_stream * s, int role);

int roar_stream_s2m     (struct roar_stream * s, struct roar_message * m);
int roar_stream_m2s     (struct roar_stream * s, struct roar_message * m);

int    roar_str2codec (char * codec);
char * roar_codec2str (int    codec);

char * roar_streamstate2str(int streamstate);

int    roar_str2role  (char * role);
char * roar_role2str  (int    role);

#endif

//ll
