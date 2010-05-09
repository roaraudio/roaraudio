//mixer.c:

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

int need_vol_change  (int channels, struct roar_mixer_settings * set) {
 int i;

 if ( set == NULL || channels < 1 || channels > ROAR_MAX_CHANNELS )
  return -1;

 if ( set->rpg_mul != set->rpg_div )
  return 1;

 for (i = 0; i < channels; i++)
  if ( set->mixer[i] != set->scale )
   return 1;

 return 0;
}

#define _err() streams_delete(stream); return -1
int add_mixer (int subsys, char * name, struct roar_stream_server ** ss_ptr) {
 struct roar_stream_server * ss;
 int stream;

 if ( (stream = streams_new()) == -1 )
  return -1;

 if ( streams_get(stream, &ss) == -1 ) {
  _err();
 }

 if ( streams_set_name(stream, name) == -1 ) {
  _err();
 }

 if ( client_stream_add(g_self_client, stream) == -1 ) {
  _err();
 }

 if ( streams_set_dir(stream, ROAR_DIR_MIXING, 1) == -1 ) {
  _err();
 }

 // the mixers are the most sync thing we have...
 if ( streams_set_rawflag(stream, ROAR_FLAG_SYNC) == -1 ) {
  _err();
 }

 if ( streams_set_flag(stream, ROAR_FLAG_IMMUTABLE) == -1 ) {
  _err();
 }

 switch (subsys) {
  case ROAR_SUBSYS_WAVEFORM:
    ss->delay = _MEGA/ROAR_OUTPUT_CFREQ;
   break;
 }

 if ( ss_ptr != NULL )
  *ss_ptr = ss;

 return stream;
}

//ll
