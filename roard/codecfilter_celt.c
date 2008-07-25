//codecfilter_celt.c:

#include "roard.h"

int cf_celt_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_celt_inst * self = malloc(sizeof(struct codecfilter_celt_inst));

 if ( !self )
  return -1;

 self->stream               = info;

 *inst = (CODECFILTER_USERDATA_T) self;

 ((struct roar_stream*)info)->info.codec = ROAR_CODEC_DEFAULT;

 return 0;
}

int cf_celt_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 if ( !inst )
  return -1;

 free(inst);
 return 0;
}

int cf_celt_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 return 0;
}

int cf_celt_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 return 0;
}

//ll
