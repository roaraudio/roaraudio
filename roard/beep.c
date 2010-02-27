//beep.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

int beep_start (int client, struct roar_beep * beep) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 int stream;

 if ( beep->vol  == 0 )
  beep->vol  = ROAR_BEEP_MAX_VOL;

 if ( beep->time == 0 )
  beep->time = 512; // 512ms

 if ( beep->freq == 0 )
  beep->freq = 440;

 if ( beep->type == 0 )
  beep->type = ROAR_BEEP_TYPE_DEFAULT;

 // x, y, z location '0' is allready centered.

 // TODO: remove the following lions as soon as we support non zero values
 if ( beep->z != 0 )
  return -1;

 if ( beep->y != 0 )
  return -1;

 if ( beep->x != 0 )
  return -1;

 if ((stream = streams_new()) == -1 )
  return -1;

 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 if ( streams_get(stream, &ss) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(s->info));

 s->info.channels = 1;

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 return stream;
}

//ll
