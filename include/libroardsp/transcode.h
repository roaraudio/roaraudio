//transcode.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifndef _LIBROARDSP_TRANSCODE_H_
#define _LIBROARDSP_TRANSCODE_H_

#include "libroardsp.h"

#define ROAR_XCODER_STAGE_NONE        0
#define ROAR_XCODER_STAGE_INITED      0
#define ROAR_XCODER_STAGE_WAIT        1
#define ROAR_XCODER_STAGE_MAGIC       2
#define ROAR_XCODER_STAGE_OPENING     3
#define ROAR_XCODER_STAGE_OPENED      4
#define ROAR_XCODER_STAGE_CLOSING     5
#define ROAR_XCODER_STAGE_CLOSED      6

#define ROAR_BIXCODER_FLAGS_NONE      0x0000
#define ROAR_BIXCODER_FLAGS_ENCODE    0x0001
#define ROAR_BIXCODER_FLAGS_DECODE    0x0002

struct roar_xcoder;

struct roar_xcoder_entry {
 int * codecs;
 int (*init)(struct roar_xcoder * state);
 int (*uninit)(struct roar_xcoder * state);
 int (*packet_size)(struct roar_xcoder * state, int samples);
 int (*proc)(struct roar_xcoder * state, void * buf, size_t len);
};

struct roar_xcoder {
 void * inst;
 struct roar_xcoder_entry * entry;
 struct roar_vio_calls * backend;
 int flags;
 int stage;
 ssize_t packet_len;
 struct roar_buffer * iobuffer;
 struct {
  struct roar_audio_info coded;
  struct roar_audio_info pcm;
 } info;
};

struct roar_bixcoder {
 struct roar_xcoder encoder, decoder;
 int flags;
};

struct roar_transcoder {
 struct roar_xcoder encoder, decoder;
};

int roar_xcoder_init(struct roar_xcoder * state, int encoder, struct roar_audio_info * info, struct roar_vio_calls * vio);
int roar_xcoder_set_backend(struct roar_xcoder * state, struct roar_vio_calls * vio);
int roar_xcoder_packet_size(struct roar_xcoder * state, int samples);
int roar_xcoder_close      (struct roar_xcoder * state);
int roar_xcoder_proc_packet(struct roar_xcoder * state, void * buf, size_t len);
int roar_xcoder_proc       (struct roar_xcoder * state, void * buf, size_t len);

int roar_bixcoder_init(struct roar_bixcoder * state, struct roar_audio_info * info, struct roar_vio_calls * vio);
int roar_bixcoder_packet_size (struct roar_bixcoder * state, int samples);
int roar_bixcoder_close       (struct roar_bixcoder * state);
int roar_bixcoder_read_packet (struct roar_bixcoder * state, void * buf, size_t len);
int roar_bixcoder_read        (struct roar_bixcoder * state, void * buf, size_t len);
int roar_bixcoder_write_packet(struct roar_bixcoder * state, void * buf, size_t len);
int roar_bixcoder_write       (struct roar_bixcoder * state, void * buf, size_t len);

#endif

//ll
