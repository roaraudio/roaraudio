//codecfilter_speex.c:

#include "roard.h"
#ifdef ROAR_HAVE_LIBSPEEX

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_speex_inst * self = malloc(sizeof(struct codecfilter_speex_inst));

 *inst = NULL;

 if (!self)
  return -1;

 *inst = (void*) self;

 return 0;
}

int cf_speex_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 if (!self)
  return -1;

 free((void*)self);

 return 0;
}

int cf_speex_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 return -1;
}
int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_speex_inst * self = (struct codecfilter_speex_inst *) inst;

 return -1;
}

#endif

//ll
