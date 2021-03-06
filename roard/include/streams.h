//streams.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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

// stream ctls...:

#define ROAR_STREAM_CTL_COMPMASK  0xFF000000
#define ROAR_STREAM_CTL_TYPEMASK  0x00FF0000
#define ROAR_STREAM_CTL_CMDMASK   0x0000FFFF

#define ROAR_STREAM_CTL_COMP_BASE 0x00000000
#define ROAR_STREAM_CTL_COMP_CF   0x01000000
#define ROAR_STREAM_CTL_COMP_DRV  0x02000000

#define ROAR_STREAM_CTL_TYPE_VOID   0x00000000
#define ROAR_STREAM_CTL_TYPE_INT    0x00010000
#define ROAR_STREAM_CTL_TYPE_FLOAT  0x00020000
#define ROAR_STREAM_CTL_TYPE_FPI100 0x00030000 /* fix point integter int/100 */

#define STREAM_DIR_NONE             0x00
#define STREAM_DIR_IN               0x01
#define STREAM_DIR_OUT              0x02
#define STREAM_DIR_BIDIR            (STREAM_DIR_IN|STREAM_DIR_OUT)

#define MAX_LTM_WINDOWS_PER_STREAM  1

struct roar_ltm_vals {
 int64_t rms;
};

struct roar_stream_ltm {
 int window;
 int parent_window;
 int mt;
 size_t refc;
 size_t channels;
 struct roar_ltm_vals *  cur;
 struct roar_ltm_vals ** history;
};

struct roar_stream_server {
 struct roar_stream _stream;
 unsigned int pos_abs;
 int client;
 struct roar_buffer * buffer;
 size_t need_extra;
 void  * output;
 struct roar_mixer_settings mixer;
#ifdef ROAR_SUPPORT_META
 struct roar_meta meta[ROAR_META_MAX_PER_STREAM];
#endif
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
// struct roardsp_filterchain fc;
 int flags;
 uint_least32_t delay;
 char * name;
 int ready;
 struct roar_buffer * outputbuffer;
 int state;
 struct roar_buffer * prethru;
 struct roar_vio_calls jumbo;
 struct roar_vio_calls * viop;
 int mixer_stream;
 struct roardsp_chanmap chanmap;
 int role;
 int parent_stream;
 struct roar_stream_ltm ltm[MAX_LTM_WINDOWS_PER_STREAM];
 struct hwmixer_stream * mixerstream;
} * g_streams[ROAR_STREAMS_MAX];

int streams_thru_num;
int streams_recsource_id;

int streams_init   (void);
int streams_free   (void);

int streams_new    (void);
int streams_delete (int id);

int streams_set_client (int id, int client);
int streams_get_client (int id);

int streams_set_dir    (int id, int dir, int defaults);
int streams_get_dir    (int id);

int streams_set_mixer_stream(int id, int mixer);
int streams_get_mixer_stream(int id, int mixer);

int streams_set_role   (int id, int role);

int streams_get_subsys (int id);
int streams_get_ssdir  (int id);

int streams_new_virtual (int parent, struct roar_stream_server ** stream);

int streams_set_fh     (int id, int fh);
int streams_get_fh     (int id);
int streams_set_null_io(int id);
int streams_get        (int id, struct roar_stream_server ** stream);

int streams_set_socktype (int id, int socktype);
int streams_get_socktype (int id);

int streams_is_ready     (int id);

int streams_set_primary  (int id, int prim);
int streams_mark_primary (int id);
int streams_set_sync     (int id, int sync);
int streams_set_mmap     (int id, int reset);
int streams_set_flag     (int id, int flag);
int streams_set_rawflag  (int id, int flag);
int streams_reset_flag   (int id, int flag);
int streams_get_flag     (int id, int flag);

int streams_set_name     (int id, char * name);
char * streams_get_name  (int id);

int streams_calc_delay   (int id);
int streams_set_mixer    (int id);

int streams_set_map      (int id, char * map, size_t len);

int streams_ltm_ctl      (int id, int mt, int window, int cmd);
int streams_ltm_calc     (int id, struct roar_audio_info * info, void * data, size_t len);
struct roar_stream_ltm * streams_ltm_get(int id, int mt, int window);

struct hwmixer_stream * streams_get_mixerstream(int id);
int streams_set_mixerstream(int id, struct hwmixer_stream * mstream);
int streams_read_mixervalues(int id);

int streams_ctl          (int id, int_least32_t cmd, void * data);

int stream_add_buffer     (int id, struct roar_buffer *  buf);
//int stream_get_buffer     (int id, struct roar_buffer ** buf);
int stream_shift_out_buffer   (int id, void * data, size_t * len);
int stream_shift_buffer   (int id, struct roar_buffer ** buf);
int stream_unshift_buffer (int id, struct roar_buffer *  buf);

int stream_outputbuffer_request(int id, struct roar_buffer ** buf, size_t len);
int stream_outputbuffer_destroy(int id);

int stream_prethru_add(int id, struct roar_buffer * buf);
int stream_prethru_add_data(int id, void ** buf, size_t len);
int stream_prethru_destroy(int id);
int stream_prethru_send(int dst, int src);

int streams_check      (int id);
int streams_send_mon   (int id);
int streams_send_filter(int id);

int streams_get_mixbuffers   (void *** bufferlist, struct roar_audio_info * info, unsigned int pos);
int streams_get_outputbuffer (int id, void ** buffer, size_t size);
int streams_fill_mixbuffer   (int id, struct roar_audio_info * info);
int streams_fill_mixbuffer2  (int id, struct roar_audio_info * info);

ssize_t stream_vio_read (int stream, void *buf, size_t count);
ssize_t stream_vio_write(int stream, void *buf, size_t count);
ssize_t stream_vio_s_read (struct roar_stream_server * stream, void *buf, size_t count);
ssize_t stream_vio_s_write(struct roar_stream_server * stream, void *buf, size_t count);

#endif

//ll
