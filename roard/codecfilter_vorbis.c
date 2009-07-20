//codecfilter_vorbis.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#define ROAR_REQUIRE_LIBVORBISFILE

#include "roard.h"

#ifdef ROAR_HAVE_LIBVORBISFILE

int _g_cf_vorbis_vfvio_return_err (void) {
 return -1;
}

ov_callbacks _g_cf_vorbis_vfvio = {
  .read_func  = cf_vorbis_vfvio_read,
  .seek_func  = (int    (*)(void *, ogg_int64_t, int      )) _g_cf_vorbis_vfvio_return_err,
  .close_func = (int    (*)(void *                        )) _g_cf_vorbis_vfvio_return_err,
  .tell_func  = (long   (*)(void *                        )) _g_cf_vorbis_vfvio_return_err
};

size_t cf_vorbis_vfvio_read (void *ptr, size_t size, size_t nmemb, void *datasource) {
 ssize_t r;

 r = stream_vio_s_read(ROAR_STREAM_SERVER(datasource), ptr, size*nmemb);

 ROAR_DBG("cf_vorbis_vfvio_read(ptr=%p, size=%lu, nmemb=%lu, datasource=%p): r=%i", ptr, size, nmemb, datasource, r);

 if ( r == -1 )
  return 0;

 if ( r > 0 )
  errno = 0;

 r /= size;
 
 ROAR_DBG("cf_vorbis_vfvio_read(ptr=%p, size=%lu, nmemb=%lu, datasource=%p) = %i", ptr, size, nmemb, datasource, r);
 return r;
}

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
 self->encoding               = 0;
 self->encoder.v_base_quality = 0.3;
 self->encoder.srn            = -1;
#endif

 ROAR_DBG("cf_vorbis_open(*): info->id=%i", ROAR_STREAM(info)->id);

 *inst = (CODECFILTER_USERDATA_T) self;

 s->info.codec = ROAR_CODEC_DEFAULT;
 s->info.bits  = 16;

 if ( s->dir == ROAR_DIR_PLAY ) {
  return 0;
 } else if ( s->dir == ROAR_DIR_MONITOR || s->dir == ROAR_DIR_OUTPUT ) {
#ifdef ROAR_HAVE_LIBVORBISENC
  // set up the encoder here
// this is delayed to the write function
/*
 if ( cf_vorbis_encode_start(self) == -1 ) {
  free(self);
  return -1;
 }
*/
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
  cf_vorbis_encode_end(self);
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
  if ( !self->encoding ) {
   if ( cf_vorbis_encode_start(self) == -1 ) {
    return -1;
   }
  }

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

  if ( cf_vorbis_encode_flushout(self) == -1 )
   return -1;
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
  if ( ov_open_callbacks((void*)self->stream, &(self->vf), NULL, 0, _g_cf_vorbis_vfvio) < 0 ) {
   return 0;
  }
  errno = EAGAIN;
  return -1;
 }

 if ( self->opened < 16 ) {
  errno = EAGAIN;
  return -1;
 }


 self->got_it_running = 1;

 while (todo) {
  r = ov_read(&(self->vf), buf+done, todo, 0, 2, 1, &(self->current_section));
  if ( r == OV_HOLE ) {
   ROAR_DBG("cf_vorbis_read(*): Hole in stream");
  } else if ( r < 1 ) {
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
#ifdef ROAR_SUPPORT_META
 vorbis_info *vi = ov_info(&(self->vf), -1);
 char **ptr = ov_comment(&(self->vf), -1)->user_comments;
 char key[ROAR_META_MAX_NAMELEN] = {0}, value[LIBROAR_BUFFER_MSGDATA] = {0};
 struct roar_stream * s = ROAR_STREAM(self->stream);
 int type;
 int j, h = 0;
 float rpg_track = 0, rpg_album = 0;
 int meta_ok;

 s->info.channels = vi->channels;
 s->info.rate     = vi->rate;
 s->info.bits     = 16;
 s->info.codec    = ROAR_CODEC_DEFAULT;

 stream_meta_clear(s->id);

 while(*ptr){
  meta_ok = 1;

   for (j = 0; (*ptr)[j] != 0 && (*ptr)[j] != '='; j++) {
    if ( j == ROAR_META_MAX_NAMELEN ) {
     ROAR_ERR("cf_vorbis_update_stream(*): invalid meta data on stream %i: meta data key too long", s->id);
     meta_ok = 0;
     j = 0;
     break;
    }
    key[j] = (*ptr)[j];
   }
   key[j] = 0;

   if ( meta_ok ) {
    for (j++, h = 0; (*ptr)[j] != 0 && (*ptr)[j] != '='; j++) {
     if ( h == LIBROAR_BUFFER_MSGDATA ) {
      ROAR_ERR("update_stream(*): invalid meta data on stream %i: meta data value for key '%s' too long", s->id, key);
      meta_ok = 0;
      h = 0;
      break;
     }
     value[h++] = (*ptr)[j];
    }
    value[h]   = 0;
   }

   if ( meta_ok ) {
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
   }

   ptr++;
 }

 if ( rpg_album ) {
  self->stream->mixer.rpg_div = 2718;  // = int(exp(1)*1000)
  self->stream->mixer.rpg_mul = (float)rpg_album*2718;
 } else if ( rpg_track ) {
  self->stream->mixer.rpg_div = 2718;
  self->stream->mixer.rpg_mul = (float)rpg_track*2718;
 }

 stream_meta_finalize(s->id);
#endif
 //printf("RPG: mul=%i, div=%i\n", self->stream->mixer.rpg_mul, self->stream->mixer.rpg_div);
 return 0;
}

int cf_vorbis_encode_start  (struct codecfilter_vorbis_inst * self) {
#ifdef ROAR_HAVE_LIBVORBISENC
 int srn = self->encoder.srn; // this value is allrady inited...
#ifdef ROAR_SUPPORT_META
 int len = 0;
 int i;
 int types[ROAR_META_MAX_PER_STREAM];
#endif
 int sid = ROAR_STREAM(self->stream)->id;
 float v_base_quality = self->encoder.v_base_quality;
 char val[LIBROAR_BUFFER_MSGDATA];

  val[LIBROAR_BUFFER_MSGDATA-1] = 0;

  memset(&(self->encoder), 0, sizeof(self->encoder));

  self->encoding = 1;
  self->encoder.srn = srn + 1;
  self->encoder.v_base_quality = v_base_quality;

  vorbis_info_init(&(self->encoder.vi));
  vorbis_comment_init(&(self->encoder.vc));
  vorbis_comment_add_tag(&(self->encoder.vc), "SERVER", "RoarAudio");
  vorbis_comment_add_tag(&(self->encoder.vc), "ENCODER", "RoarAudio Vorbis codecfilter");

#ifdef ROAR_SUPPORT_META
  if ( (len = stream_meta_list(sid, types, ROAR_META_MAX_PER_STREAM)) != -1 ) {
   for (i = 0; i < len; i++) {
//int stream_meta_get     (int id, int type, char * name, char * val, size_t len);
    if ( stream_meta_get(sid, types[i], NULL, val, LIBROAR_BUFFER_MSGDATA-1) == 0 )
     vorbis_comment_add_tag(&(self->encoder.vc), roar_meta_strtype(types[i]), val);
   }
  }
#endif

  ROAR_DBG("cf_vorbis_encode_start(*): q=%f", v_base_quality);

  if( vorbis_encode_init_vbr(&(self->encoder.vi), (long) ROAR_STREAM(self->stream)->info.channels,
                                                  (long) ROAR_STREAM(self->stream)->info.rate,
                                                  v_base_quality) != 0 ) {
   ROAR_ERR("cf_vorbis_encode_start(*): Can not vorbis_encode_init_vbr(*)!");
   vorbis_info_clear(&(self->encoder.vi)); // TODO: do we need to free vc also?
   return -1;
  }

  vorbis_analysis_init(&(self->encoder.vd), &(self->encoder.vi));
  vorbis_block_init(&(self->encoder.vd), &(self->encoder.vb));

  ROAR_DBG("cf_vorbis_encode_start(*): srn=%i", self->encoder.srn);
                                     //  "RA"<<16 + PID<<8 + Stream ID
  ogg_stream_init(&(self->encoder.os),
                   (((0x5241 + self->encoder.srn) & 0xffff)<<16) +
                   ( (getpid()                    & 0x00ff)<< 8) +
                   (  sid                         & 0x00ff));
 return 0;
#else
 return -1;
#endif
}

int cf_vorbis_encode_end    (struct codecfilter_vorbis_inst * self) {
#ifdef ROAR_HAVE_LIBVORBISENC
 if ( self->encoding ) {
  // try to flush up to an EOS page...
  vorbis_analysis_buffer(&(self->encoder.vd), 2*ROAR_MAX_CHANNELS);
  vorbis_analysis_wrote(&(self->encoder.vd), 0);
  cf_vorbis_encode_flushout(self);

  // clean up...
  ogg_stream_clear(&(self->encoder.os));
  vorbis_block_clear(&(self->encoder.vb));
  vorbis_dsp_clear(&(self->encoder.vd));
  vorbis_info_clear(&(self->encoder.vi));
  self->opened = 0;
 }

 return 0;
#else
 return -1;
#endif
}

int cf_vorbis_encode_flushout(struct codecfilter_vorbis_inst * self) {
#ifdef ROAR_HAVE_LIBVORBISENC
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

 return 0;
#else
 return -1;
#endif
}

int cf_vorbis_ctl(CODECFILTER_USERDATA_T   inst, int cmd, void * data) {
 struct codecfilter_vorbis_inst * self = (struct codecfilter_vorbis_inst *) inst;
 int_least32_t type = cmd & ROAR_STREAM_CTL_TYPEMASK;

 cmd -= type;

 switch (cmd) {
  case ROAR_CODECFILTER_CTL2CMD(ROAR_CODECFILTER_CTL_META_UPDATE):
    if ( type != ROAR_STREAM_CTL_TYPE_VOID )
     return -1;

    ROAR_DBG("cf_vorbis_ctl(*): stoping stream...");
    if ( cf_vorbis_encode_end(self) == -1 )
     return -1;
    ROAR_DBG("cf_vorbis_ctl(*): restarting stream...");
    if ( cf_vorbis_encode_start(self) == -1 )
     return -1;

    return 0;
   break;
  case ROAR_CODECFILTER_CTL2CMD(ROAR_CODECFILTER_CTL_SET_Q):
    if ( type != ROAR_STREAM_CTL_TYPE_FLOAT )
     return -1;

    ROAR_DBG("cf_vorbis_ctl(*): setting quality to q=%f", *(float*)data);

    self->encoder.v_base_quality = *(float*)data / 10;

    if ( self->encoding ) {
     ROAR_DBG("cf_vorbis_ctl(*): we are allready encoding, restart...");
     ROAR_DBG("cf_vorbis_ctl(*): stoping stream...");
     if ( cf_vorbis_encode_end(self) == -1 )
      return -1;
     ROAR_DBG("cf_vorbis_ctl(*): restarting stream...");
     if ( cf_vorbis_encode_start(self) == -1 )
      return -1;
    }

    return 0;
   break;
  default:
    ROAR_DBG("cf_vorbis_ctl(*): Unknown command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));
    return -1;
 }

 return -1;
}

#endif
//ll
