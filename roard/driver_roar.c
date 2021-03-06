//driver_raor.c:

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

int driver_roar_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 int dir = ROAR_DIR_PLAY;

 if ( fh != -1 ) { // this is a connection to a roard, no roar_simple_*() interface avalible for this case
  return -1;
 }

 if ( sstream != NULL ) {
  switch (ROAR_STREAM(sstream)->dir) {
   case ROAR_DIR_OUTPUT:      dir = ROAR_DIR_PLAY;       break;
   case ROAR_DIR_MIDI_OUT:    dir = ROAR_DIR_MIDI_IN;    break;
   case ROAR_DIR_LIGHT_OUT:   dir = ROAR_DIR_LIGHT_IN;   break;
// TODO: we need to know a real pos id for raw streams...
//   case ROAR_DIR_RAW_OUT:   dir = ROAR_DIR_RAW_IN;   break;
   case ROAR_DIR_COMPLEX_OUT: dir = ROAR_DIR_COMPLEX_IN; break;
   default:
     return -1;
  }
 }

 if ( roar_vio_simple_stream(inst, info->rate, info->channels, info->bits, info->codec, device, dir, "roard") == -1 ) {
  return -1;
 }

 return 0;
}

//ll
