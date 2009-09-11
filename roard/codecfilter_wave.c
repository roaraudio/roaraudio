//codecfilter_wave.c:

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

#include "roard.h"

#ifndef ROAR_WITHOUT_CF_WAVE

int cf_wave_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_wave_inst * self = malloc(sizeof(struct codecfilter_wave_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 if ( !self )
  return -1;

 self->stream               = info;
 self->vstream              = NULL;
 self->opened               = 0;

 *inst = (CODECFILTER_USERDATA_T) self;

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));
 return 0;
}

int cf_wave_close(CODECFILTER_USERDATA_T   inst) {
// struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;

 if ( !inst )
  return -1;

 free(inst);
 return 0;
}

int cf_wave_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;
 int r = -1;
 char tbuf[44];
 struct roar_stream * ps = ROAR_STREAM(self->stream);
 struct roar_stream *  s;
 struct roar_audio_info info;
 uint16_t tmp16;
 uint32_t tmp32;
 int codec = -1;
 int vid, fh;

 if ( self->opened ) {
  return stream_vio_s_read(self->stream, buf, len);
 } else {
  if (stream_vio_s_read(self->stream, tbuf, 44) != 44) {
   return -1;
  }

  // TODO: write better code here!

  if ( (fh = streams_get_fh(ps->id)) == -1 ) {
   return -1;
  }

  if ( (vid = streams_new_virtual(ps->id, &(self->vstream))) == -1 ) {
   return -1;
  }

  ROAR_DBG("cf_wave_read(*): self->vstream=%p", self->vstream);

  s = ROAR_STREAM(self->vstream);

  memcpy(&tmp32, tbuf+24, 4);
  s->info.rate = ROAR_LE2HOST32(tmp32);

  memcpy(&tmp16, tbuf+22, 2);
  s->info.channels = ROAR_LE2HOST16(tmp16);

  memcpy(&tmp16, tbuf+34, 2);
  s->info.bits = ROAR_LE2HOST16(tmp16);

  memcpy(&tmp16, tbuf+20, 2);

  switch (ROAR_LE2HOST16(tmp16)) {
   case ROAR_RIFF_WAVE_CID_PCM:
     if ( s->info.bits == 8 ) {
      codec = ROAR_CODEC_PCM_U_LE;
     } else {
      codec = ROAR_CODEC_PCM_S_LE;
     }
    break;
   case ROAR_RIFF_WAVE_CID_ALAW:
     codec = ROAR_CODEC_ALAW;
    break;
   case ROAR_RIFF_WAVE_CID_MULAW:
     codec = ROAR_CODEC_MULAW;
    break;
   case ROAR_RIFF_WAVE_CID_IEEE_FLOAT:
   default:
     return -1;
  }

   s->info.codec            = codec;
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

int cf_wave_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;
 struct roar_stream           * s    = ROAR_STREAM(self->stream);
 char header[44];
 int32_t  tmp32;
 int16_t  tmp16;
 int16_t  bits;
 int16_t  codec;

 ROAR_DBG("cf_wave_write(inst=%p, buf=%p, len=%i) = ?", inst, buf, len);
 ROAR_DBG("cf_wave_write(inst=%p, buf=%p, len=%i): self->opened=%i", inst, buf, len, self->opened);

 if ( self->opened ) {
  return stream_vio_s_write(self->stream, buf, len);
 } else {

  if ( s->fh == -1 ) {
   errno = EAGAIN;
   return -1;
  }

  memcpy(header   , "RIFF\367\377\377\177WAVEfmt \020", 17);
  memcpy(header+36, "data\313\377\377\177", 8);

  switch (s->info.codec) {
   case ROAR_CODEC_PCM_S_LE:
     codec = 0x0001;
    break;
   default:
     ROAR_ERR("cf_wave_write(*) Codec not supported!");
     return -1;
    break;
  }

  ROAR_DBG("cf_wave_write(*) Codec supported!");

  bits = s->info.bits;
  memcpy(header+24, &(s->info.rate    ), 4);
  memcpy(header+22, &(s->info.channels), 2);
  memcpy(header+34, &bits, 2);

  tmp16 = s->info.channels * bits / 8;
  memcpy(header+32, &tmp16, 2);
  tmp32 = tmp16 * s->info.rate;
  memcpy(header+28, &tmp32, 4);
  memcpy(header+20, &codec, 2);

  if ( stream_vio_s_write(self->stream, header, 44) != 44 )
   return -1;

  self->opened = 1;

  errno = EAGAIN;
//  return -1;

  len = stream_vio_s_write(self->stream, buf, len);

  cf_wave_close(inst);
  ROAR_STREAM_SERVER(s)->codecfilter = -1;

  return len;

//  return stream_vio_s_write(self->stream, buf, len);
 }

 return -1;
}

int cf_wave_ctl(CODECFILTER_USERDATA_T   inst, int cmd, void * data) {
 struct codecfilter_wave_inst * self = (struct codecfilter_wave_inst *) inst;
 int_least32_t type = cmd & ROAR_STREAM_CTL_TYPEMASK;

 cmd -= type;

 ROAR_DBG("cf_wave_ctl(*): command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));

 switch (cmd) {
  case ROAR_CODECFILTER_CTL2CMD(ROAR_CODECFILTER_CTL_VIRTUAL_DELETE):
    streams_delete(ROAR_STREAM(self->stream)->id);
    return 0;
   break;
  default:
    ROAR_DBG("cf_wave_ctl(*): Unknown command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));
    return -1;
 }

 return -1;
}

#endif

//ll
