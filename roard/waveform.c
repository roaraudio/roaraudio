//waveform.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

int waveform_init  (void) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;

 if ( (g_waveform_mixer.stream = add_mixer(ROAR_SUBSYS_WAVEFORM, _MIXER_NAME("Waveform"), &ss)) == -1 )
  return -1;

 g_waveform_mixer.ss = ss;

 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 ss->state = ROAR_STREAMSTATE_OLD;

 return 0;
}

int waveform_free  (void) {
 return 0;
}

int waveform_update_mixer (void) {
 if ( streams_get_flag(g_waveform_mixer.stream, ROAR_FLAG_MUTE) == 1 ) {
  memset(g_output_buffer, 0, (g_sa->bits*ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels)/8);
  return 0;
 }

 if ( !need_vol_change(ROAR_STREAM(g_waveform_mixer.ss)->info.channels, &(g_waveform_mixer.ss->mixer)) )
  return 0;

 roar_amp_pcm(g_output_buffer, g_sa->bits, g_output_buffer,
              ROAR_OUTPUT_BUFFER_SAMPLES*g_sa->channels, g_sa->channels,
              &(g_waveform_mixer.ss->mixer));

 return 0;
}

//ll
