//codecfilter_speex.h.h:

#ifndef _CODECFILTER_SPEEX_H_
#define _CODECFILTER_SPEEX_H_

#include <roaraudio.h>
#include <speex/speex.h>
#include <speex/speex_stereo.h>

struct codecfilter_speex_inst {
 void * encoder;
 void * decoder;
 SpeexBits bits;
 struct roar_stream_server * stream;
 int frame_size;
 void     * cd; /* current data */
 char       cc[ROAR_SPEEX_MAX_CC]; /* buffer for read() and write() */
 void     * i_rest; /* rest... */
 int fi_rest;
 char * o_rest;
 int fo_rest; /* how much is in rest? */

/*
 char * ibuf;
 char * obuf;
 int out_size;
 char * i_rest;
 int s_buf;
 int fi_rest; /-* how much is in rest? *-/
*/
};

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_speex_close(CODECFILTER_USERDATA_T   inst);

int cf_speex_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

#endif

//ll
