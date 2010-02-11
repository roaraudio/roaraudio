//channelmap.c:

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

/** Initialize the specified channel map and return a pointer to it */
pa_channel_map* pa_channel_map_init(pa_channel_map *m);

/** Initialize the specified channel map for monoaural audio and return a pointer to it */
pa_channel_map* pa_channel_map_init_mono(pa_channel_map *m) {
 m->channels = 1;
 m->map[0]   = PA_CHANNEL_POSITION_MONO;

 return m;
}

/** Initialize the specified channel map for stereophonic audio and return a pointer to it */
pa_channel_map* pa_channel_map_init_stereo(pa_channel_map *m) {
 m->channels = 2;
 m->map[0]   = PA_CHANNEL_POSITION_LEFT;
 m->map[1]   = PA_CHANNEL_POSITION_RIGHT;

 return m;
}

/** Initialize the specified channel map for the specified number
 * of channels using default labels and return a pointer to it. */
pa_channel_map* pa_channel_map_init_auto(pa_channel_map *m, unsigned channels, pa_channel_map_def_t def) {
 if ( m == NULL || channels == 0 )
  return NULL;

 switch (channels) {
  case 1: return pa_channel_map_init_mono(m);   break;
  case 2: return pa_channel_map_init_stereo(m); break;
 }

 switch (def) {
  default:
    return NULL;
 }
}

/** Return a text label for the specified channel position */
const char* pa_channel_position_to_string(pa_channel_position_t pos);

/** The maximum length of strings returned by pa_channel_map_snprint() */
#define PA_CHANNEL_MAP_SNPRINT_MAX 336

/** Make a humand readable string from the specified channel map */
char* pa_channel_map_snprint(char *s, size_t l, const pa_channel_map *map);

/** Parse a channel position list into a channel map structure. \since 0.8.1 */
pa_channel_map *pa_channel_map_parse(pa_channel_map *map, const char *s);

/** Compare two channel maps. Return 1 if both match. */
int pa_channel_map_equal(const pa_channel_map *a, const pa_channel_map *b);

/** Return non-zero of the specified channel map is considered valid */
int pa_channel_map_valid(const pa_channel_map *map);

//ll
