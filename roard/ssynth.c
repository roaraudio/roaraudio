//ssynth.c:

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

float ssynth_polys[SSYNTH_POLY_POLYMAX][SSYNTH_POLY_COEFF] = {
       {0.300000,  0.958333, -0.550000,  0.091667},
       {0.700010, -0.083333, -0.150000,  0.033333}
      };

int ssynth_init_config(void) {
 memset(&ssynth_conf, 0, sizeof(ssynth_conf));

 return 0;
}

#define _err() streams_delete(sid); return -1
int ssynth_init (void) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 int sid;

 if ( !ssynth_conf.enable )
  return 0;

 memset(&g_ssynth, 0, sizeof(g_ssynth));
 g_ssynth.stream = -1;

 if ( (sid = streams_new()) == -1 )
  return -1;

 if ( streams_set_client(sid, g_self_client) == -1 ) {
  _err();
 }

 if ( streams_set_dir(sid, ROAR_DIR_BRIDGE, 1) == -1 ) {
  _err();
 }

 if ( streams_set_flag(sid, ROAR_FLAG_PRIMARY) == -1 ) {
  _err();
 }

 if ( streams_set_name(sid, "Simple Synthesizer") == -1 ) {
  _err();
 }

 if ( streams_get(sid, &ss) == -1 ) {
  _err();
 }

 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->info.channels = 1;
 s->info.codec    = ROAR_CODEC_DEFAULT;

 g_ssynth.stream = sid;

 return 0;
}

int ssynth_free (void) {
 if ( !ssynth_conf.enable )
  return 0;

 return streams_delete(g_ssynth.stream);
}

//ll
