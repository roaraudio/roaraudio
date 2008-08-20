//codecfilter_vorbis.c:

#include "roard.h"

#ifdef ROAR_HAVE_LIBVORBISFILE

int cf_vorbis_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_vorbis_inst * self = malloc(sizeof(struct codecfilter_vorbis_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 if ( !self )
  return -1;

 self->current_section      = -1;
 self->last_section         = -1;
 self->opened               =  0;
 self->got_it_running       =  0;
 self->stream               = info;
// self->outlen               = ROAR_OUTPUT_BUFFER_SAMPLES * s->info.channels * s->info.bits / 8; // optimal size
#ifdef ROAR_HAVE_LIBVORBISENC
 self->encoding             = 0;
#endif

 ROAR_DBG("cf_vorbis_open(*): info->id=%i", ROAR_STREAM(info)->id);

 if ( (self->in = fdopen(s->fh, "r")) == NULL ) {
  free((void*)self);
  return -1;
 }

 *inst = (CODECFILTER_USERDATA_T) self;

 s->info.codec = ROAR_CODEC_DEFAULT;
 s->info.bits  = 16;

 if ( s->dir == ROAR_DIR_PLAY ) {
  return 0;
 } else if ( s->dir == ROAR_DIR_MONITOR || s->dir == ROAR_DIR_OUTPUT ) {
#ifdef ROAR_HAVE_LIBVORBISENC
  // set up the encoder here

  memset(&(self->encoder), 0, sizeof(self->encoder));

  self->encoding = 1;

  vorbis_info_init(&(self->encoder.vi));
  vorbis_comment_init(&(self->encoder.vc));
  vorbis_comment_add_tag(&(self->encoder.vc), "SERVER", "RoarAudio");
  vorbis_comment_add_tag(&(self->encoder.vc), "ENCODER", "RoarAudio Vorbis codecfilter");

  if( vorbis_encode_init_vbr(&(self->encoder.vi), (long) s->info.channels, (long) s->info.rate,
                                                  self->encoder.v_base_quality) != 0 ) {
   ROAR_ERR("cf_vorbis_open(*): Can not vorbis_encode_init_vbr(*)!");
   vorbis_info_clear(&(self->encoder.vi)); // TODO: do we need to free vc also?
   free(self);
   return -1;
  }

  vorbis_analysis_init(&(self->encoder.vd), &(self->encoder.vi));
  vorbis_block_init(&(self->encoder.vd), &(self->encoder.vb));

                                     //  "RA"<<16 + PID<<8 + Stream ID
  ogg_stream_init(&(self->encoder.os), 0x52410000 + ((getpid() & 0xff)<<8) + s->id);

#else
 free(self);
 return -1;
#endif
 } else {
  free(self);
  return -1;
 }

 return 0;
}

int cf_vorbis_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;

 if ( !inst )
  return -1;

 if ( self->got_it_running )
  ov_clear(&(self->vf));

#ifdef ROAR_HAVE_LIBVORBISENC
 if ( self->encoding ) {
  ogg_stream_clear(&(self->encoder.os));
  vorbis_block_clear(&(self->encoder.vb));
  vorbis_dsp_clear(&(self->encoder.vd));
  vorbis_info_clear(&(self->encoder.vi));
 }
#endif

 free(inst);
 return 0;
}

int cf_vorbis_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
#ifdef ROAR_HAVE_LIBVORBISENC
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;
 struct roar_stream * s = ROAR_STREAM(self->stream);
 ogg_packet header;
 ogg_packet header_comm;
 ogg_packet header_code;
 float ** encbuf;
 int i, c;
 int chans;
 int end;
 int16_t * data = (int16_t *) buf;

 if ( ! self->opened ) {
  vorbis_analysis_headerout(&(self->encoder.vd), &(self->encoder.vc), &header, &header_comm, &header_code);

  ogg_stream_packetin(&(self->encoder.os), &header);
  ogg_stream_packetin(&(self->encoder.os), &header_comm);
  ogg_stream_packetin(&(self->encoder.os), &header_code);

  while (ogg_stream_flush(&(self->encoder.os), &(self->encoder.og))) {
   if ( stream_vio_s_write(self->stream, self->encoder.og.header, self->encoder.og.header_len)
                                                                 != self->encoder.og.header_len ||
        stream_vio_s_write(self->stream, self->encoder.og.body,   self->encoder.og.body_len  )
                                                                 != self->encoder.og.body_len     ) {
    free(self); // TODO: do we need addional cleanup?
    return -1;
   }
  }
  self->opened = 1;
 } else {
  encbuf = vorbis_analysis_buffer(&(self->encoder.vd), len /* TODO: need to lookup the menaing of this */);
  chans  = s->info.channels;
  end    = len/(2*chans);

  if ( chans == 1 ) { // use optimized code
   for (i = 0; i < end; i++)
    encbuf[0][i] = data[i]/32768.0;

  } else if ( chans == 2 ) { // use optimized code
   for (i = 0; i < end; i++) {
    encbuf[0][i] = data[2*i  ]/32768.0;
    encbuf[1][i] = data[2*i+1]/32768.0;
   }
  } else { // use generic multi channel code
   for (i = 0; i < end; i++) {
    for (c = 0; c < chans; c++) {
     encbuf[c][i] = data[chans*i+c]/32768.0;
    }
   }
  }

  vorbis_analysis_wrote(&(self->encoder.vd), i);

  while ( vorbis_analysis_blockout(&(self->encoder.vd), &(self->encoder.vb)) == 1 ) {
   vorbis_analysis(&(self->encoder.vb), &(self->encoder.op));
   vorbis_bitrate_addblock(&(self->encoder.vb));

   while ( vorbis_bitrate_flushpacket(&(self->encoder.vd), &(self->encoder.op)) ) {
    ogg_stream_packetin(&(self->encoder.os), &(self->encoder.op));

    while( ogg_stream_pageout(&(self->encoder.os), &(self->encoder.og)) ) {
     if (
          stream_vio_s_write(self->stream, self->encoder.og.header, self->encoder.og.header_len) == -1 ||
          stream_vio_s_write(self->stream, self->encoder.og.body,   self->encoder.og.body_len  ) == -1   ) {
      return -1;
     }
    }
   }
  }
 }

  return len; // we assume every thing was written (at least into our dsp anaylises buffer
#else
 errno = ENOSYS;
 return -1;
#endif
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
 struct roar_stream * s = ROAR_STREAM(self->stream);
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
