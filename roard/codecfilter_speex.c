//codecfilter_speex.c:

#include "roard.h"
#ifdef ROAR_HAVE_LIBSPEEX

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 *inst = NULL;
 return -1;
}

int cf_speex_close(CODECFILTER_USERDATA_T   inst) {
 return -1;
}

int cf_speex_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 return -1;
}
int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 return -1;
}

#endif

//ll
