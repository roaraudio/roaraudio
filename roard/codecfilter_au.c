//codecfilter_au.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifndef ROAR_WITHOUT_CF_AU

int cf_au_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_au_inst * self = roar_mm_malloc(sizeof(struct codecfilter_au_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 if ( !self )
  return -1;

 self->stream               = info;
 self->vstream              = NULL;
 self->opened               = 0;

 *inst = (CODECFILTER_USERDATA_T) self;

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->info.codec = ROAR_CODEC_PCM_S_BE;

 return 0;
}

int cf_au_close(CODECFILTER_USERDATA_T   inst) {
// struct codecfilter_au_inst * self = (struct codecfilter_au_inst *) inst;

 if ( !inst )
  return -1;

 roar_mm_free(inst);
 return 0;
}

int cf_au_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_au_inst * self = (struct codecfilter_au_inst *) inst;
 int r = -1;
 char tbuf[ROAR_AU_MIN_HEADER_LEN];
 uint32_t * header = (uint32_t*)tbuf;
 struct roar_stream * ps = ROAR_STREAM(self->stream);
 struct roar_stream *  s;
 struct roar_audio_info info;
 char * extra_header;
 int codec = -1;
 int vid, fh;
 int i;

 if ( self->opened ) {
  return stream_vio_s_read(self->stream, buf, len);
 } else {
  if (stream_vio_s_read(self->stream, tbuf, ROAR_AU_MIN_HEADER_LEN) != ROAR_AU_MIN_HEADER_LEN) {
   return -1;
  }

  for (i = 0; i < ROAR_AU_MIN_HEADER_LEN/4; i++) {
   header[i] = ROAR_NET2HOST32(header[i]);
   ROAR_DBG("cf_au_read(inst=%p, buf=%p, len=%i): header[%i] = 0x%.8x", inst, buf, len, i, header[i]);
  }


  // test the header, is this really a AU stream?
  if ( header[0] != ROAR_AU_MAGIC )
   return -1;

  if ( header[1] != ROAR_AU_MIN_HEADER_LEN ) {
   header[1] -= ROAR_AU_MIN_HEADER_LEN;

   if ( header[1] > 32*1024 ) // do not allow more than 32KiB header...
    return -1;

   if ( (extra_header = roar_mm_malloc(header[1])) == NULL )
    return -1;

   if ( stream_vio_s_read(self->stream, extra_header, header[1]) != header[1] ) {
    roar_mm_free(extra_header);
    return -1;
   }

   roar_mm_free(extra_header);
  }

  // TODO: write better code here!

  if ( (fh = streams_get_fh(ps->id)) == -1 ) {
   return -1;
  }

  if ( (vid = streams_new_virtual(ps->id, &(self->vstream))) == -1 ) {
   return -1;
  }

  ROAR_DBG("cf_au_read(*): self->vstream=%p", self->vstream);

  s = ROAR_STREAM(self->vstream);

  s->info.rate = header[4];

  s->info.channels = header[5];

  switch (header[3]) {
   case ROAR_AU_CID_MULAW:
     s->info.bits  =  8;
     codec         =  ROAR_CODEC_MULAW;
    break;
   case ROAR_AU_CID_ALAW:
     s->info.bits  =  8;
     codec         =  ROAR_CODEC_ALAW;
    break;
   case ROAR_AU_CID_PCM_S_8:
     s->info.bits  =  8;
     codec         =  ROAR_CODEC_PCM_S_BE;
    break;
   case ROAR_AU_CID_PCM_S_16:
     s->info.bits  = 16;
     codec         =  ROAR_CODEC_PCM_S_BE;
    break;
   case ROAR_AU_CID_PCM_S_24:
     s->info.bits  = 24;
     codec         =  ROAR_CODEC_PCM_S_BE;
    break;
   case ROAR_AU_CID_PCM_S_32:
     s->info.bits  = 32;
     codec         =  ROAR_CODEC_PCM_S_BE;
    break;
   default:
     return -1;
  }

  s->info.codec             = codec;
  self->vstream->codec_orgi = codec;

  memcpy(&info, &(s->info), sizeof(struct roar_audio_info));

  if ( streams_set_fh(vid, fh) == -1 ) {
   return -1;
  }

/*
  if ( roar_vio_open_pass(&(self->vstream->vio), &(self->stream->vio)) == -1 ) {
   return -1;
  }
*/

  memcpy(&(self->vstream->vio), &(self->stream->vio), sizeof(struct roar_vio_calls));

  if ( streams_set_null_io(ps->id) == -1 ) {
   return -1;
  }

  memcpy(&(ps->info), &info, sizeof(struct roar_audio_info));

  self->opened = 1;

  errno = EAGAIN;
  return -1;
 }

 return r;
}

int cf_au_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_au_inst * self = (struct codecfilter_au_inst *) inst;
 struct roar_stream         * s    = ROAR_STREAM(self->stream);
 void     * headerdata;
 uint32_t * header;
 uint32_t codec;
 int      sid;
 int      i;

 ROAR_DBG("cf_au_write(inst=%p, buf=%p, len=%i) = ?", inst, buf, len);
 ROAR_DBG("cf_au_write(inst=%p, buf=%p, len=%i): self->opened=%i", inst, buf, len, self->opened);

 if ( self->opened ) {
  return stream_vio_s_write(self->stream, buf, len);
 } else {

  if ( s->fh == -1 ) {
   errno = EAGAIN;
   return -1;
  }

  sid = ROAR_STREAM(self->stream)->id;

  if ( stream_prethru_destroy(sid) == -1 ) {
   return -1;
  }

  if ( stream_prethru_add_data(sid, &headerdata, ROAR_AU_MIN_HEADER_LEN) == -1 ) {
   return -1;
  }

  header = headerdata;

  switch (s->info.codec) {
   case ROAR_CODEC_PCM_S_BE:
     switch (s->info.bits) {
      case  8: codec = ROAR_AU_CID_PCM_S_8;  break;
      case 16: codec = ROAR_AU_CID_PCM_S_16; break;
      case 24: codec = ROAR_AU_CID_PCM_S_24; break;
      case 32: codec = ROAR_AU_CID_PCM_S_32; break;
      default:
        ROAR_ERR("cf_au_write(*) bits per sample not supported!");
        return -1;
       break;
     }
    break;
   default:
     ROAR_ERR("cf_au_write(*) Codec not supported!: %s(%i)", roar_codec2str(s->info.codec), s->info.codec);
     return -1;
    break;
  }

  ROAR_DBG("cf_au_write(*) Codec supported!");

  header[0] = ROAR_AU_MAGIC;
  header[1] = ROAR_AU_MIN_HEADER_LEN;
  header[2] = ROAR_AU_DATASIZE;
  header[3] = codec;
  header[4] = s->info.rate;
  header[5] = s->info.channels;

  for (i = 0; i < ROAR_AU_MIN_HEADER_LEN/4; i++)
   header[i] = ROAR_HOST2NET32(header[i]);

  if ( stream_vio_s_write(self->stream, header, ROAR_AU_MIN_HEADER_LEN) != ROAR_AU_MIN_HEADER_LEN )
   return -1;

  self->opened = 1;

  errno = EAGAIN;
//  return -1;

  len = stream_vio_s_write(self->stream, buf, len);

  cf_au_close(inst);
  ROAR_STREAM_SERVER(s)->codecfilter = -1;

  return len;

//  return stream_vio_s_write(self->stream, buf, len);
 }

 return -1;
}

int cf_au_ctl(CODECFILTER_USERDATA_T   inst, int cmd, void * data) {
 struct codecfilter_au_inst * self = (struct codecfilter_au_inst *) inst;
 int_least32_t type = cmd & ROAR_STREAM_CTL_TYPEMASK;

 cmd -= type;

 ROAR_DBG("cf_au_ctl(*): command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));

 switch (cmd) {
  case ROAR_CODECFILTER_CTL2CMD(ROAR_CODECFILTER_CTL_VIRTUAL_DELETE):
    streams_delete(ROAR_STREAM(self->stream)->id);
    return 0;
   break;
  default:
    ROAR_DBG("cf_au_ctl(*): Unknown command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));
    return -1;
 }

 return -1;
}

#endif

//ll
