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

 ((struct roar_stream*)info)->info.codec = ROAR_CODEC_DEFAULT;

 return 0;
}

int cf_vorbis_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;

 if ( !inst )
  return -1;

 ov_clear(&(self->vf));

 free(inst);
 return 0;
}

int cf_vorbis_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;
 long r;
 long todo = len;
 long done = 0;

// printf("cf_vorbis_read(inst=%p, buf=%p, len=%i) = ?\n", inst, buf, len);

 self->opened++;
 if ( self->opened == 16 ) {
  //printf("cf_vorbis_read(*): opening...\n");
  if ( ov_open(self->in, &(self->vf), NULL, 0) < 0 ) {
   free((void*)self);
   return 0;
  }
 }

 if ( self->opened < 16 ) {
  return -1;
 }

 while (todo) {
  r = ov_read(&(self->vf), buf+done, todo, 0, 2, 1, &(self->current_section));
  if ( r < 1 ) {
   break;
  } else {
   todo -= r;
   done += r;
  }
 }

 //printf("ov_read(*) = %i\n", done);

 if ( done == 0 ) {
  // do some EOF handling...
  return 0;
 } else {
  return len;
 }
}

//ll
