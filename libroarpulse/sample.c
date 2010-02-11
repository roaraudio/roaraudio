//sample.c:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

/** Return the amount of bytes playback of a second of audio with the specified sample type takes */
size_t pa_bytes_per_second(const pa_sample_spec *spec);

/** Return the size of a frame with the specific sample type */
size_t pa_frame_size(const pa_sample_spec *spec) {
 if ( spec == NULL )
  return 0;

 return pa_sample_size(spec) * spec->channels;
}

/** Return the size of a sample with the specific sample type */
size_t pa_sample_size(const pa_sample_spec *spec) {
 if ( spec == NULL )
  return -1;

 switch (spec->format) {
  case PA_SAMPLE_ALAW:
  case PA_SAMPLE_ULAW:
  case PA_SAMPLE_U8:
    return 1;
   break;
  case PA_SAMPLE_S16LE:
  case PA_SAMPLE_S16BE:
    return 2;
   break;
  default:
    return 0;
   break;
 }

 return 0;
}

/** Calculate the time the specified bytes take to play with the specified sample type */
pa_usec_t pa_bytes_to_usec(uint64_t length, const pa_sample_spec *spec);

/** Calculates the number of bytes that are required for the specified time. \since 0.9 */
size_t pa_usec_to_bytes(pa_usec_t t, const pa_sample_spec *spec);

/** Return non-zero when the sample type specification is valid */
int pa_sample_spec_valid(const pa_sample_spec *spec) {
 if ( spec == NULL )
  return 0;

 if ( spec->channels < 1 || spec->channels > ROAR_MAX_CHANNELS )
  return 0;

 if ( spec->format < 0 || spec->format > PA_SAMPLE_MAX )
  return 0;

 return 1;
}

/** Return non-zero when the two sample type specifications match */
int pa_sample_spec_equal(const pa_sample_spec*a, const pa_sample_spec*b) {
 if ( a->rate != b->rate )
  return 0;

 if ( a->channels != b->channels )
  return 0;

 if ( a->format != b->format )
  return 0;

 return 1;
}

/** Return a descriptive string for the specified sample format. \since 0.8 */
static struct {
 pa_sample_format_t format;
 const char * name;
} _roar_pa_format[] = {
 {PA_SAMPLE_INVALID, NULL}
};

const char *pa_sample_format_to_string(pa_sample_format_t f);

/** Parse a sample format text. Inverse of pa_sample_format_to_string() */
pa_sample_format_t pa_parse_sample_format(const char *format);

/** Maximum required string length for pa_sample_spec_snprint() */
#define PA_SAMPLE_SPEC_SNPRINT_MAX 32

/** Pretty print a sample type specification to a string */
char* pa_sample_spec_snprint(char *s, size_t l, const pa_sample_spec *spec);

/** Pretty print a byte size value. (i.e. "2.5 MiB") */
char* pa_bytes_snprint(char *s, size_t l, unsigned v);

//ll
