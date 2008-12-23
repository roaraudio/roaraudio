//libroarstream.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

#define roar_dir2str(x)   ((x) == ROAR_DIR_PLAY   ? "play"   : (x) == ROAR_DIR_MONITOR ? "monitor" : \
                           (x) == ROAR_DIR_FILTER ? "filter" : (x) == ROAR_DIR_RECORD  ? "record"  : \
                           (x) == ROAR_DIR_OUTPUT ? "output" : (x) == ROAR_DIR_BIDIR   ? "bidir"   : \
                           (x) == ROAR_DIR_MIXING ? "mixing" : \
                           "unknown")
/*
#define roar_codec2str(x) ((x) == ROAR_CODEC_PCM_S_LE  ? "pcm_s_le"  : (x) == ROAR_CODEC_PCM_S_BE  ? "pcm_s_be"  : \
                           (x) == ROAR_CODEC_PCM_S_PDP ? "pcm_s_pdp" : (x) == ROAR_CODEC_MIDI_FILE ? "midi_file" : \
                           "unknown" )
*/

#define ROAR_FLAG_NONE           0x00
#define ROAR_FLAG_PRIMARY        0x01
#define ROAR_FLAG_OUTPUT         0x02
#define ROAR_FLAG_DRIVER         ROAR_FLAG_OUTPUT
#define ROAR_FLAG_SOURCE         0x04
#define ROAR_FLAG_SYNC           0x08
#define ROAR_FLAG_META           0x10
//#define ROAR_FLAG_SYNC           0x08

#define ROAR_SET_FLAG            0
#define ROAR_RESET_FLAG          1

struct roar_stream_info {
 int block_size;
 int pre_underruns;
 int post_underruns;
 int codec;
 int flags;
 char * driver;
};

int roar_stream_connect (struct roar_connection * con, struct roar_stream * s, int dir);

int roar_stream_new     (struct roar_stream * s, unsigned int rate, unsigned int channels, unsigned int bits, unsigned int codec);

int roar_stream_exec    (struct roar_connection * con, struct roar_stream * s);
int roar_stream_connect_to (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port);
int roar_stream_connect_to_ask (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port);
int roar_stream_passfh  (struct roar_connection * con, struct roar_stream * s, int fh);

int roar_stream_attach_simple (struct roar_connection * con, struct roar_stream * s, int client);

int roar_stream_add_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len);

int roar_stream_send_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len);

int roar_stream_get_info (struct roar_connection * con, struct roar_stream * s, struct roar_stream_info * info);

int roar_stream_set_flags (struct roar_connection * con, struct roar_stream * s, int flags, int reset);

int roar_stream_s2m     (struct roar_stream * s, struct roar_message * m);
int roar_stream_m2s     (struct roar_stream * s, struct roar_message * m);

int    roar_str2codec (char * codec);
char * roar_codec2str (int    codec);

#endif

//ll
