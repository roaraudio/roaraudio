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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_HAVE_LIBFISHSOUND

int cf_fishsound_decoded_float (FishSound * fsound, float ** pcm, long frames, void * user_data) {
 struct codecfilter_fishsound_inst * self = (struct codecfilter_fishsound_inst *) user_data;
 struct roar_stream * stream = ROAR_STREAM(self->stream);
 struct roar_buffer * buf;
 int i;
 double s;
 union {
  void    * v;
  char    * c;
  int16_t * i16;
  int32_t * i32;
 } data;

 ROAR_DBG("cf_fishsound_decoded_float(fsound=%p, pcm=%p, frames=%li, user_data=%p) = ?", fsound, pcm, frames, user_data);

 ROAR_DBG("cf_fishsound_decoded_float(*): self->opened=%i", self->opened);

 if (!self->opened) {
   fish_sound_command(fsound, FISH_SOUND_GET_INFO, &(self->fsinfo),
                       sizeof(FishSoundInfo));
 }

 if ( roar_buffer_new(&buf, frames*stream->info.bits*stream->info.channels/8) == -1 )
  return -1;

 if ( roar_buffer_get_data(buf, &data.v) == -1 )
  return -1;

 frames *= self->fsinfo.channels;

 switch (stream->info.bits) {
  case  8:
    for (i = 0; i < frames; i++) {
     s  = ((float*)pcm)[i];
     s *= 127;
     data.c[i] = s;
    }
   break;
  case 16:
    for (i = 0; i < frames; i++) {
     s  = ((float*)pcm)[i];
     s *= 32767;
     data.i16[i] = s;
    }
   break;
  case 32:
    for (i = 0; i < frames; i++) {
     s  = ((float*)pcm)[i];
     s *= 2147483647;
     data.i32[i] = s;
    }
   break;
  default:
    return -1;
 }

 if ( self->buffer == NULL ) {
  self->buffer = buf;
 } else {
  roar_buffer_add(self->buffer, buf);
 }

 return -1;
}

int cf_fishsound_read_packet (OGGZ * oggz, ogg_packet * op, long serialno, void * user_data) {
 FishSound * fsound = (FishSound *)user_data;

 fish_sound_prepare_truncation(fsound, op->granulepos, op->e_o_s);
 fish_sound_decode(fsound, op->packet, op->bytes);

 return 0;
}

int cf_fishsound_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_fishsound_inst * self = malloc(sizeof(struct codecfilter_fishsound_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 if ( !self )
  return -1;

 if ( s->dir != ROAR_DIR_PLAY ) {
  free(self);
  return -1;
 }

 self->stream               = info;
 self->opened               = 0;
 self->buffer               = NULL;
 self->fsound               = fish_sound_new(FISH_SOUND_DECODE, &(self->fsinfo));

 fish_sound_set_interleave(self->fsound, 1);
 fish_sound_set_decoded_float_ilv(self->fsound, cf_fishsound_decoded_float, (void*)self);

 self->oggz = oggz_new(OGGZ_READ);

 oggz_set_read_callback(self->oggz, -1, cf_fishsound_read_packet, self->fsound);

 *inst = (CODECFILTER_USERDATA_T) self;

 return 0;
}

int cf_fishsound_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_fishsound_inst * self = (struct codecfilter_fishsound_inst *) inst;

 if ( inst == NULL )
  return -1;

 oggz_close(self->oggz);
 fish_sound_delete(self->fsound);

 if ( self->buffer != NULL )
  roar_buffer_free(self->buffer);

 free(inst);
 return 0;
}

int cf_fishsound_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_fishsound_inst * self = (struct codecfilter_fishsound_inst *) inst;
 struct roar_stream * s = ROAR_STREAM(self->stream);
 long inlen;
 int need_data = 0;
 struct roar_buffer_stats stats;
 size_t stlen;

 ROAR_DBG("cf_fishsound_read(inst=%p, buf=%p, len=%i) = ?", inst, buf, len);

/*
 if ( self->opened ) {
//  return stream_vio_s_read(self->stream, buf, len);
 } else {
/-*
  if (stream_vio_s_read(self->stream, tbuf, 44) != 44) {
   return -1;
  }
*ä/

  self->opened = 1;

  errno = EAGAIN;
  return -1;
 }
*/

 if ( self->buffer == NULL ) {
  need_data = 1;
 } else {
  if ( roar_buffer_ring_stats(self->buffer, &stats) == -1 )
   return -1;

  if ( stats.bytes < len )
   need_data = 1;
 }

 ROAR_DBG("cf_fishsound_read(*): need_data=%i, self->opened=%i", need_data, self->opened);

// while (need_data) {
  if ( (inlen = stream_vio_s_read(self->stream, buf, len)) == -1 ) {
//   if ( errno != EAGAIN ) {
    return -1;
/*
   } else {
    return -1;
   }
*/
  } else {
   if ( inlen == 0 )
    return 0;

   oggz_read_input(self->oggz, (unsigned char *)buf, inlen);

   if( self->buffer != NULL ) {
    if ( roar_buffer_ring_stats(self->buffer, &stats) == -1 )
     return -1;

    if ( stats.bytes < len ) {
     need_data = 1;
    } else {
     need_data = 0;
    }
   }
  }
// }

 ROAR_DBG("cf_fishsound_read(*): need_data=%i, self->opened=%i", need_data, self->opened);

 if ( need_data ) {
  errno = EAGAIN;
  return -1;
 }

 if ( !self->opened ) {
  s->info.channels = self->fsinfo.channels;
  s->info.rate     = self->fsinfo.samplerate;
  s->info.bits     = g_sa->bits;
  s->info.codec    = ROAR_CODEC_NATIVE;
  self->opened     = 1;
  errno            = EAGAIN;
  ROAR_DBG("cf_fishsound_read(inst=%p, buf=%p, len=%i) = -1 // errno=EAGAIN", inst, buf, len);
  return -1;
 }

 // ok, now we should have all the data we want...

 stlen = len;
 if ( roar_buffer_shift_out(&(self->buffer), buf, &stlen) == -1 ) {
  ROAR_DBG("cf_fishsound_read(inst=%p, buf=%p, len=%i) = -1 // roar_buffer_shift_out() failed", inst, buf, len);
  return -1;
 }

 ROAR_DBG("cf_fishsound_read(inst=%p, buf=%p, len=%i) = %i", inst, buf, len, (int)stlen);
 return stlen;
}

#endif

//ll
