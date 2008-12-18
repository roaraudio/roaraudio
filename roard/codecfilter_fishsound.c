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

#ifdef ROAR_HAVE_LIBFISHSOUND

int cf_fishsound_decoded_float (FishSound * fsound, float ** pcm, long frames, void * user_data) {
 return -1;
}

int cf_fishsound_read_packet (OGGZ * oggz, ogg_packet * op, long serialno, void * user_data) {
 FishSound * fsound = (FishSound *)user_data;

 fish_sound_prepare_truncation (fsound, op->granulepos, op->e_o_s);
 fish_sound_decode (fsound, op->packet, op->bytes);

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

 free(inst);
 return 0;
}

int cf_fishsound_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_fishsound_inst * self = (struct codecfilter_fishsound_inst *) inst;
 struct roar_stream * s = ROAR_STREAM(self->stream);
 long inlen;

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

 if ( (inlen = stream_vio_s_read(self->stream, buf, len)) == -1 )
  return -1;

 oggz_read_input(self->oggz, (unsigned char *)buf, inlen);

 if ( !self->opened ) {
  s->info.channels = self->fsinfo.channels;
  s->info.rate     = self->fsinfo.samplerate;
  s->info.bits     = g_sa->bits;
  s->info.codec    = ROAR_CODEC_NATIVE;
 }

 return -1;
}

#endif

//ll
