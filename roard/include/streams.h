//streams.h:

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
} * g_streams[ROAR_STREAMS_MAX];

int streams_init   (void);
int streams_free   (void);

int streams_new    (void);
int streams_delete (int id);

int streams_set_client (int id, int client);

int streams_set_fh     (int id, int fh);
int streams_get_fh     (int id);
int streams_get        (int id, struct roar_stream_server ** stream);

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


#endif

//ll
