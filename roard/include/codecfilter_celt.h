//codecfilter_celt.h:

#ifndef _CODECFILTER_CELT_H_
#define _CODECFILTER_CELT_H_

#include <roaraudio.h>

#ifdef ROAR_HAVE_LIBCELT

#include <celt/celt.h>
#include <celt/celt_header.h>

struct codecfilter_celt_inst {
 struct roar_stream_server * stream;
 CELTMode * mode;
 CELTEncoder * encoder;
 CELTDecoder * decoder;
 int frame_size;
 int lookahead;
 int out_size;
 char * ibuf;
 char * obuf;
 char * i_rest;
 char * o_rest;
 int s_buf;
 int fi_rest; /* how much is in rest? */
 int fo_rest; /* how much is in rest? */
 int opened;
};

int cf_celt_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_celt_close(CODECFILTER_USERDATA_T   inst);

int cf_celt_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_celt_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

#endif

#endif

//ll
