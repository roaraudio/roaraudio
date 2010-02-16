//volume.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

/** Return non-zero when *a == *b */
int pa_cvolume_equal(const pa_cvolume *a, const pa_cvolume *b) {
 int i;

 if ( a == b )
  return 1;

 if ( a == NULL || b == NULL )
  return 0;

 if ( a->channels != b->channels )
  return 0;

 for (i = 0; i < a->channels; i++)
  if ( a->values[i] != b->values[i] )
   return 0;

 return 1;
}

/** Set the volume of all channels to the specified parameter */
pa_cvolume* pa_cvolume_set(pa_cvolume *a, unsigned channels, pa_volume_t v) {
 int i;

 if ( a == NULL )
  return NULL;

 if ( channels > PA_CHANNELS_MAX )
  return NULL;

 a->channels = channels;

 for (i = 0; i < channels; i++) {
  a->values[i] = v;
 }

 return a;
}

/** Pretty print a volume structure */
char *pa_cvolume_snprint(char *s, size_t l, const pa_cvolume *c);

/** Return the average volume of all channels */
pa_volume_t pa_cvolume_avg(const pa_cvolume *a) {
 int64_t sum = 0;
 int i;

#ifndef PA_VOLUME_INVALID
#define PA_VOLUME_INVALID ((pa_volume_t) UINT32_MAX)
#endif

 if ( a == NULL )
  return PA_VOLUME_INVALID;

 for (i = 0; i < a->channels; i++)
  sum += a->values[i];

 return sum/a->channels;
}

/** Return TRUE when the passed cvolume structure is valid, FALSE otherwise */
int pa_cvolume_valid(const pa_cvolume *v) {
 if ( v == NULL )
  return 0;

 if ( v->channels <= 0 )
  return 0;

 if ( v->channels > PA_CHANNELS_MAX )
  return 0;

 return 1;
}

/** Return non-zero if the volume of all channels is equal to the specified value */
int pa_cvolume_channels_equal_to(const pa_cvolume *a, pa_volume_t v) {
 int i;

 if ( a == NULL )
  return 0;

 for (i = 0; i < a->channels; i++)
  if ( a->values[i] != v )
   return 0;

 return 1;
}

/** Multiply two volumes specifications, return the result. This uses PA_VOLUME_NORM as neutral element of multiplication. This is only valid for software volumes! */
pa_volume_t pa_sw_volume_multiply(pa_volume_t a, pa_volume_t b);

/** Multiply to per-channel volumes and return the result in *dest. This is only valid for software volumes! */
pa_cvolume *pa_sw_cvolume_multiply(pa_cvolume *dest, const pa_cvolume *a, const pa_cvolume *b);

/** Convert a decibel value to a volume. This is only valid for software volumes! \since 0.4 */
pa_volume_t pa_sw_volume_from_dB(double f);

/** Convert a volume to a decibel value. This is only valid for software volumes! \since 0.4 */
double pa_sw_volume_to_dB(pa_volume_t v);

/** Convert a linear factor to a volume. This is only valid for software volumes! \since 0.8 */
pa_volume_t pa_sw_volume_from_linear(double v);

/** Convert a volume to a linear factor. This is only valid for software volumes! \since 0.8 */
double pa_sw_volume_to_linear(pa_volume_t v);

//ll
