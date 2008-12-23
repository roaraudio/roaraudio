//streams.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

#ifndef _STREAMS_H_
#define _STREAMS_H_

#include <roaraudio.h>

/*
  Defined in <roaraudio/streams.h>

#define ROAR_STREAMS_MAX  64
*/

//#define ROAR_STREAM(x)        ((struct roar_stream*)(x))
#define ROAR_STREAM_SERVER(x) ((struct roar_stream_server*)(x))

struct roar_stream_server {
 struct roar_stream _stream;
 unsigned int pos_abs;
 int client;
 struct roar_buffer * buffer;
 size_t need_extra;
 void  * output;
 struct roar_mixer_settings mixer;
 struct roar_meta meta[ROAR_META_MAX_PER_STREAM];
 int is_new;
 int codecfilter;
 int codec_orgi;
 CODECFILTER_USERDATA_T codecfilter_inst;
 int socktype;
 int pre_underruns;
 int post_underruns;
 struct roar_vio_calls vio;
 int primary;
 int driver_id;
 struct roardsp_filterchain fc;
 int flags;
} * g_streams[ROAR_STREAMS_MAX];

int streams_init   (void);
int streams_free   (void);

int streams_new    (void);
int streams_delete (int id);

int streams_set_client (int id, int client);
int streams_get_client (int id);

int streams_set_fh     (int id, int fh);
int streams_get_fh     (int id);
int streams_get        (int id, struct roar_stream_server ** stream);

int streams_set_socktype (int id, int socktype);
int streams_get_socktype (int id);

int streams_set_primary  (int id, int prim);
int streams_mark_primary (int id);
int streams_set_flag     (int id, int flag);
int streams_reset_flag   (int id, int flag);
int streams_get_flag     (int id, int flag);

int stream_add_buffer     (int id, struct roar_buffer *  buf);
//int stream_get_buffer     (int id, struct roar_buffer ** buf);
int stream_shift_buffer   (int id, struct roar_buffer ** buf);
int stream_unshift_buffer (int id, struct roar_buffer *  buf);

int streams_check      (int id);
int streams_send_mon   (int id);
int streams_send_filter(int id);

int streams_get_mixbuffers   (void *** bufferlist, struct roar_audio_info * info, unsigned int pos);
int streams_get_outputbuffer (int id, void ** buffer, size_t size);
int streams_fill_mixbuffer   (int id, struct roar_audio_info * info);

ssize_t stream_vio_read (int stream, void *buf, size_t count);
ssize_t stream_vio_write(int stream, void *buf, size_t count);
ssize_t stream_vio_s_read (struct roar_stream_server * stream, void *buf, size_t count);
ssize_t stream_vio_s_write(struct roar_stream_server * stream, void *buf, size_t count);

#endif

//ll
