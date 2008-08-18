//stream.h:

#ifndef _ROARAUDIO_STREAM_H_
#define _ROARAUDIO_STREAM_H_

#define ROAR_DIR_DEFAULT ROAR_DIR_PLAY

#define ROAR_DIR_PLAY     1
#define ROAR_DIR_RECORD   2
#define ROAR_DIR_MONITOR  3
#define ROAR_DIR_FILTER   4

#define ROAR_DIR_OUTPUT   5

//#define ROAR_DIR_MIXING   6
//#define ROAR_DIR_INTERNAL 7

#define ROAR_DIR_META     8


#define ROAR_CARE_NOPOS  0
#define ROAR_CARE_POS    1

#define ROAR_STREAM(a) ((struct roar_stream*)(a))

#define ROAR_STREAMS_MAX  64

struct roar_stream {
 int id;

 int fh;
 int dir;
 int care_pos;

 uint32_t pos;
 uint32_t pos_rel_id;

 struct roar_audio_info info;

 int    datalen;
 char * database;
 int    offset;
 char * dataoff;
};

#endif

//ll
