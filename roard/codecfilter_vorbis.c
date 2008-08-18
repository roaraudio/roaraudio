//codecfilter_vorbis.c:

#include "roard.h"

#ifdef ROAR_HAVE_LIBVORBISFILE

int cf_vorbis_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_vorbis_inst * self = malloc(sizeof(struct codecfilter_vorbis_inst));

 if ( !self )
  return -1;

 self->current_section      = -1;
 self->last_section         = -1;
 self->opened               =  0;
 self->got_it_running       =  0;
 self->stream               = info;
// self->outlen               = ROAR_OUTPUT_BUFFER_SAMPLES * s->info.channels * s->info.bits / 8; // optimal size

 ROAR_DBG("cf_vorbis_open(*): info->id=%i", ROAR_STREAM(info)->id);

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

 if ( self->got_it_running )
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
//   free((void*)self);
   return 0;
  }
 }

 if ( self->opened < 16 ) {
  errno = EAGAIN;
  return -1;
 }


 self->got_it_running = 1;

 while (todo) {
  r = ov_read(&(self->vf), buf+done, todo, 0, 2, 1, &(self->current_section));
  if ( r < 1 ) {
   break;
  } else {
   if ( self->last_section != self->current_section )
    if ( cf_vorbis_update_stream(self) == -1 )
     return 0;

   self->last_section = self->current_section;
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

int cf_vorbis_update_stream (struct codecfilter_vorbis_inst * self) {
 vorbis_info *vi = ov_info(&(self->vf), -1);
 char **ptr = ov_comment(&(self->vf), -1)->user_comments;
 char key[80] = {0}, value[80] = {0};
 struct roar_stream * s = (struct roar_stream *) self->stream;
 int type;
 int j, h = 0;
 float rpg_track = 0, rpg_album = 0;

 s->info.channels = vi->channels;
 s->info.rate     = vi->rate;
 s->info.bits     = 16;
 s->info.codec    = ROAR_CODEC_DEFAULT;

 stream_meta_clear(s->id);

 while(*ptr){
   for (j = 0; (*ptr)[j] != 0 && (*ptr)[j] != '='; j++)
    key[j] = (*ptr)[j];
    key[j] = 0;

   for (j++, h = 0; (*ptr)[j] != 0 && (*ptr)[j] != '='; j++)
    value[h++] = (*ptr)[j];
    value[h]   = 0;

   type = roar_meta_inttype(key);
   if ( type != -1 )
    stream_meta_set(s->id, type, "", value);

   ROAR_DBG("cf_vorbis_update_stream(*): Meta %-16s: %s", key, value);

   if ( strcmp(key, "REPLAYGAIN_TRACK_PEAK") == 0 ) {
    rpg_track = 1/atof(value);
/*
   } else if ( strcmp(key, "REPLAYGAIN_TRACK_GAIN") == 0 ) {
    rpg_track = powf(10, atof(value)/20);
*/
   } else if ( strcmp(key, "REPLAYGAIN_ALBUM_PEAK") == 0 ) {
    rpg_album = 1/atof(value);
/* 
   } else if ( strcmp(key, "REPLAYGAIN_ALBUM_GAIN") == 0 ) {
    rpg_album = powf(10, atof(value)/20);
*/
   }

   ++ptr;
 }

 if ( rpg_album ) {
  self->stream->mixer.rpg_div = 2718;  // = int(exp(1)*1000)
  self->stream->mixer.rpg_mul = (float)rpg_album*2718;
 } else if ( rpg_track ) {
  self->stream->mixer.rpg_div = 2718;
  self->stream->mixer.rpg_mul = (float)rpg_track*2718;
 }

 //printf("RPG: mul=%i, div=%i\n", self->stream->mixer.rpg_mul, self->stream->mixer.rpg_div);
 return 0;
}

#endif

//ll
