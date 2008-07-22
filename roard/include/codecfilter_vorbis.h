//codecfilter_vorbis.h:

#ifndef _CODECFILTER_VORBIS_H_
#define _CODECFILTER_VORBIS_H_

#include <roaraudio.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <math.h>

struct codecfilter_vorbis_inst {
 int current_section;
 int last_section;
 int opened;
 FILE * in;
 struct roar_stream_server * stream;
 OggVorbis_File vf;
};

int cf_vorbis_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_vorbis_close(CODECFILTER_USERDATA_T   inst);

int cf_vorbis_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);

int cf_vorbis_update_stream (struct codecfilter_vorbis_inst * self);

#endif

//ll
