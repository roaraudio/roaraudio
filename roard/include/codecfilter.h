//codecfilter.h:

#ifndef _CODECFILTER_H_
#define _CODECFILTER_H_

#include <roaraudio.h>

#define CODECFILTER_USERDATA_T void *

struct roar_stream_server;

#define ROAR_CODECFILTER_NONE  0
#define ROAR_CODECFILTER_READ  1
#define ROAR_CODECFILTER_WRITE 2

struct roar_codecfilter {
 int    codec;
 char * name;
 char * desc;
 char * options;
 int    flags;
 int (*open )(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter);
 int (*close)(CODECFILTER_USERDATA_T   inst);
 int (*pause)(CODECFILTER_USERDATA_T   inst, int newstate);
 int (*write)(CODECFILTER_USERDATA_T   inst, char * buf, int len);
 int (*read )(CODECFILTER_USERDATA_T   inst, char * buf, int len);
 int (*flush)(CODECFILTER_USERDATA_T   inst);
};


#include "codecfilter_cmd.h"
#include "codecfilter_wave.h"


#ifdef ROAR_HAVE_LIBVORBISFILE
#include "codecfilter_vorbis.h"
#endif

#ifdef ROAR_HAVE_LIBCELT
#include "codecfilter_celt.h"
#endif

#ifdef ROAR_HAVE_LIBSPEEX
#include "codecfilter_speex.h"
#endif

void print_codecfilterlist (void);

int codecfilter_open (CODECFILTER_USERDATA_T * inst,
                 int * codecfilter_id, char * codecfilter /* NOTE: this is not part of struct roar_codecfilter's def! */,
                 int codec, struct roar_stream_server * info);
int codecfilter_close(CODECFILTER_USERDATA_T   inst, int codecfilter);
int codecfilter_pause(CODECFILTER_USERDATA_T   inst, int codecfilter, int newstate);
int codecfilter_write(CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len);
int codecfilter_read (CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len);
int codecfilter_flush(CODECFILTER_USERDATA_T   inst, int codecfilter);


#endif

//ll
