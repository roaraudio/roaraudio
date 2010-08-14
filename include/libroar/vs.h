//vs.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#ifndef _LIBROARVS_H_
#define _LIBROARVS_H_

#include "libroar.h"

struct roar_vs;

typedef struct roar_vs roar_vs_t;

const char * roar_vs_strerr(int error);

roar_vs_t * roar_vs_new_from_con(struct roar_connection * con, int * error);
roar_vs_t * roar_vs_new(const char * server, const char * name, int * error);

int roar_vs_stream(roar_vs_t * vss, const struct roar_audio_info * info, int dir, int * error);

roar_vs_t * roar_vs_new_simple(const char * server, const char * name, int rate, int channels, int codec, int bits, int dir, int * error);

#define roar_vs_new_playback(s,n,r,c,e,b,error) roar_vs_new_simple((s), (n), (r), (c), (e), (b), ROAR_DIR_PLAY, (error))

#define ROAR_VS_TRUE     1
#define ROAR_VS_FALSE    0
#define ROAR_VS_TOGGLE  -1
#define ROAR_VS_ASK     -2

int roar_vs_close(roar_vs_t * vss, int killit, int * error);

ssize_t roar_vs_write(roar_vs_t * vss, const void * buf, size_t len, int * error);
ssize_t roar_vs_read (roar_vs_t * vss,       void * buf, size_t len, int * error);

#define ROAR_VS_WAIT   1
#define ROAR_VS_NOWAIT 0

int     roar_vs_sync (roar_vs_t * vss, int wait, int * error);

int     roar_vs_blocking (roar_vs_t * vss, int val, int * error);

#define ROAR_VS_BACKEND_DEFAULT ROAR_VS_BACKEND_FIRST
#define ROAR_VS_BACKEND_NONE    -1
#define ROAR_VS_BACKEND_FIRST   -2
#define ROAR_VS_BACKEND_MEAN    -3

ssize_t roar_vs_latency(roar_vs_t * vss, int backend, int * error);

int     roar_vs_pause(roar_vs_t * vss, int val, int * error);

int     roar_vs_mute (roar_vs_t * vss, int val, int * error);

int     roar_vs_volume_mono   (roar_vs_t * vss, float c, int * error);
int     roar_vs_volume_stereo (roar_vs_t * vss, float l, float r, int * error);

int     roar_vs_volume_get    (roar_vs_t * vss, float * l, float * r, int * error);

int     roar_vs_meta          (roar_vs_t * vss, struct roar_keyval * kv, size_t len, int * error);

struct roar_connection * roar_vs_connection_obj(roar_vs_t * vss, int * error);
struct roar_stream     * roar_vs_stream_obj    (roar_vs_t * vss, int * error);
struct roar_vio_calls  * roar_vs_vio_obj       (roar_vs_t * vss, int * error);

#endif

//ll
