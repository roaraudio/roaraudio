//file.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - August 2008
 *
 *  This file is part of libroaryiff a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroaryiff is distributed in the hope that it will be useful,
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

#include <libroaryiff.h>

int YGetSoundObjectAttributes (YConnection *con, const char *path, YEventSoundObjectAttributes *buf) {
 if ( buf == NULL )
  return -1;

 buf->format = SndObjTypeDSP; // RoarAudio doesn't make a hard diffrence here
                              // TODO: maybe we should check for the codec

 // as we do not know anything we fill with defaults:
 buf->sample_size = ROAR_BITS_DEFAULT; // don't know, but seems to be in bits not bytes
 buf->channels    = ROAR_CHANNELS_DEFAULT;
 buf->sample_rate = ROAR_RATE_DEFAULT;
 buf->length      = 0;

 strncpy(buf->path, path, YPathMax);

 return 0;
}

//ll
