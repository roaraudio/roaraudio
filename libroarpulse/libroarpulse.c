//libroarpulse.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libpulse*. They are mostly copyrighted by:
 *  Lennart Poettering <poettering@users.sourceforge.net> and
 *  Pierre Ossman <drzeus@drzeus.cx>
 *
 *  This file is part of libroarpulse a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>

int roar_codec_pulse2roar (int codec) {
 return ROAR_CODEC_DEFAULT;
}

int roar_pa_sspec2auinfo (struct roar_audio_info * info, const pa_sample_spec * ss) {
 if ( ss == NULL || info == NULL )
  return -1;

 info->rate     = ss->rate;
 info->channels = ss->channels;

 switch (ss->format) {
  case PA_SAMPLE_U8:
    info->bits  = 8;
    info->codec = ROAR_CODEC_PCM_U_LE;
   break;
  case PA_SAMPLE_ALAW:
    info->bits  = 8;
    info->codec = ROAR_CODEC_ALAW;
   break;
  case PA_SAMPLE_ULAW:
    info->bits  = 8;
    info->codec = ROAR_CODEC_MULAW;
   break;
  case PA_SAMPLE_S16LE:
    info->bits  = 16;
    info->codec = ROAR_CODEC_PCM_S_LE;
   break;
  case PA_SAMPLE_S16BE:
    info->bits  = 16;
    info->codec = ROAR_CODEC_PCM_S_BE;
   break;
  // invalid and not supported ones follow:
  case PA_SAMPLE_INVALID:
  case PA_SAMPLE_MAX:
  case PA_SAMPLE_FLOAT32LE:
  case PA_SAMPLE_FLOAT32BE:
  default:
    return -1;
   break;
 }

 return 0;
}

int roar_pa_auinfo2sspec (pa_sample_spec * ss, const struct roar_audio_info * info) {
 if ( ss == NULL || info == NULL )
  return -1;

 ss->rate       = info->rate;
 ss->channels   = info->channels;

 switch (info->codec) {
  case ROAR_CODEC_ALAW:
    ss->format = PA_SAMPLE_ALAW;
   break;
  case ROAR_CODEC_MULAW:
    ss->format = PA_SAMPLE_ULAW;
   break;
  case ROAR_CODEC_PCM_S_LE:
    if ( info->bits != 16 )
     return -1;
    ss->format = PA_SAMPLE_S16LE;
   break;
  case ROAR_CODEC_PCM_S_BE:
    if ( info->bits != 16 )
     return -1;
    ss->format = PA_SAMPLE_S16BE;
   break;
  case ROAR_CODEC_PCM_U_LE:
  case ROAR_CODEC_PCM_U_BE:
  case ROAR_CODEC_PCM_U_PDP:
    if ( info->bits != 8 )
     return -1;
    ss->format = PA_SAMPLE_U8;
   break;
  default:
    return -1;
   break;
 }

 return 0;
}

char * roar_pa_find_server (char * server) {
 struct roar_x11_connection * x11con;

 if ( server == NULL )
  server = getenv("PULSE_SERVER");

 if ( server == NULL ) {
  if ( (x11con = roar_x11_connect(NULL)) != NULL ) {
   server = roar_x11_get_prop(x11con, "PULSE_SERVER");
   roar_x11_disconnect(x11con);
  }
 }

 return server;
}

//ll
