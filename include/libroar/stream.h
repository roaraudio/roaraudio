//libroarstream.h:

#ifndef _LIBROARSTREAM_H_
#define _LIBROARSTREAM_H_

#include "roaraudio.h"

#define roar_dir2str(x)   ((x) == ROAR_DIR_PLAY   ? "play"   : (x) == ROAR_DIR_MONITOR ? "monitor" : \
                           (x) == ROAR_DIR_FILTER ? "filter" : (x) == ROAR_DIR_RECORD  ? "record"  : \
                           (x) == ROAR_DIR_OUTPUT ? "output" : "unknown")

#define roar_codec2str(x) ((x) == ROAR_CODEC_PCM_S_LE  ? "pcm_s_le"  : (x) == ROAR_CODEC_PCM_S_BE  ? "pcm_s_be"  : \
                           (x) == ROAR_CODEC_PCM_S_PDP ? "pcm_s_pdp" : (x) == ROAR_CODEC_MIDI_FILE ? "midi_file" : \
                           "unknown" )

struct roar_stream_info {
 int block_size;
 int pre_underruns;
 int post_underruns;
};

int roar_stream_connect (struct roar_connection * con, struct roar_stream * s, int dir);

int roar_stream_new     (struct roar_stream * s, unsigned int rate, unsigned int channels, unsigned int bits, unsigned int codec);

int roar_stream_exec    (struct roar_connection * con, struct roar_stream * s);
int roar_stream_connect_to (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port);
int roar_stream_connect_to_ask (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port);

int roar_stream_add_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len);

int roar_stream_send_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len);

int roar_stream_get_info (struct roar_connection * con, struct roar_stream * s, struct roar_stream_info * info);

int roar_stream_s2m     (struct roar_stream * s, struct roar_message * m);
int roar_stream_m2s     (struct roar_stream * s, struct roar_message * m);

#endif

//ll
