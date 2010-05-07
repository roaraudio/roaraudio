//driver_esd.c:

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
#ifdef ROAR_HAVE_ESD

int driver_esd_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 esd_format_t format = ESD_STREAM | ESD_PLAY;
 char name[] = "roard";

 if ( fh != -1 )
  return -1;

 if ( info->bits >= 16 ) {
  info->bits  = 16;
  format     |= ESD_BITS16;
  info->codec = ROAR_CODEC_PCM_S;
 } else if ( info->bits < 16 ) {
  info->bits  =  8;
  format     |= ESD_BITS8;
  info->codec = ROAR_CODEC_PCM_U;
 }

 if ( info->channels >= 2 ) {
  info->channels  = 2;
  format         |= ESD_STEREO;
 } else {
  info->channels  = 1;
  format         |= ESD_MONO;
 }

 fh = esd_play_stream_fallback(format, info->rate, device, name);

 roar_vio_set_fh(inst, fh);
 roar_vio_shutdown(inst, ROAR_VIO_SHUTDOWN_READ);

 return 0;
}


#endif
//ll
