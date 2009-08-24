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
 struct roar_stream * s = ROAR_STREAM(self->stream);

 if ( self->opened ) {
  return stream_vio_s_read(self->stream, buf, len);
 } else {
  if (stream_vio_s_read(self->stream, tbuf, 44) != 44) {
   return -1;
  }

  // TODO: write better code here!

  memcpy(&(s->info.rate    ), tbuf+24, 4);
  memcpy(&(s->info.channels), tbuf+22, 2);
  memcpy(&(s->info.bits    ), tbuf+34, 2);

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

#endif

//ll
