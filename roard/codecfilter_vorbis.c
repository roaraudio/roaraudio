//codecfilter_vorbis.c:

#include "roard.h"

int cf_vorbis_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_vorbis_inst * self = malloc(sizeof(struct codecfilter_vorbis_inst));

 if ( !self )
  return -1;

 self->current_section      = -1;
 self->last_section         = -1;
 self->opened               = 0;
 self->stream               = info;
// self->outlen               = ROAR_OUTPUT_BUFFER_SAMPLES * s->info.channels * s->info.bits / 8; // optimal size

 if ( (self->in = fdopen(((struct roar_stream*)info)->fh, "r")) == NULL ) {
  free((void*)self);
  return -1;
 }

 *inst = (CODECFILTER_USERDATA_T) self;

 return -1;
}

int cf_vorbis_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;

 if ( !inst )
  return -1;

 ov_clear(&(self->vf));

 free(inst);
 return -1;
}

int cf_vorbis_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;
 long r;

 if ( !self->opened == 2 ) {
  if ( ov_open(self->in, &(self->vf), NULL, 0) < 0 ) {
   free((void*)self);
   return -1;
  }
 }
 self->opened++;

 r = ov_read(&(self->vf), buf, len, 0, 2, 1, &(self->current_section));

 if ( r == 0 ) {
  // do some EOF handling...
  return -1;
 } else if ( r < 0 ) {
  return -1; // error in stream
 } else {
  return r;
 }
}

//ll
