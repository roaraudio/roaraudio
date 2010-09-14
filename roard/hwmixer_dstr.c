//hwmixer_dstr.c:

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

int hwmixer_dstr_open(struct hwmixer_stream * stream, char * drv, char * dev, int fh, char * basename, struct roar_keyval * subnames, size_t subnamelen) {
 struct roar_vio_calls * vio = roar_mm_malloc(sizeof(struct roar_vio_calls));
 struct roar_vio_defaults def;
 struct roar_stream_server * ss;

 if ( vio == NULL )
  return -1;

 if ( fh == -1 ) {
  if ( dev == NULL ) {
   roar_mm_free(vio);
   return -1;
  }

  if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, O_WRONLY|O_CREAT|O_TRUNC, 0644) == -1 ) {
   roar_mm_free(vio);
   return -1;
  }

  if ( roar_vio_open_dstr(vio, dev, &def, 1) == -1 ) {
   roar_mm_free(vio);
   return -1;
  }
 } else {
  if ( roar_vio_open_fh(vio, fh) == -1 ) {
   roar_mm_free(vio);
   return -1;
  }
 }

 stream->baseud = vio;

 roar_vio_printf(vio, "No data yet.\n");
 roar_vio_lseek(vio, 0, SEEK_SET);
 roar_vio_sync(vio);

 if (streams_get(stream->basestream, &ss) != -1) {
  ROAR_STREAM(ss)->info.channels = 2;
 } else {
  ROAR_WARN("hwmixer_dstr_open(*): can not get object for basestream %i", stream->basestream);
 }

 return 0;
}

int hwmixer_dstr_close(struct hwmixer_stream * stream) {
 roar_vio_close(stream->baseud);
 roar_mm_free(stream->baseud);
 return 0;
}

int hwmixer_dstr_set_vol(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings) {
 struct roar_vio_calls * vio = stream->baseud;
 int i;

 roar_vio_printf(vio, "[Stream %2i of basestream %2i]\n", stream->stream, stream->basestream);
 roar_vio_printf(vio, "Channels: %2i\n", channels);
 roar_vio_printf(vio, "Mode: %1i\n", mode);
 roar_vio_printf(vio, "Scale: %5i\n", (int)settings->scale);
 roar_vio_printf(vio, "RPG: %5i/%5i\n", (int)settings->rpg_mul, (int)settings->rpg_div);

 for (i = 0; i < channels; i++) {
  roar_vio_printf(vio, "Channel[%2i]: %5i\n", i, (int)settings->mixer[i]);
 }

 roar_vio_lseek(vio, 0, SEEK_SET);
 roar_vio_sync(vio);
 return 0;
}

//ll
