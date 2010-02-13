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

static struct {
 pa_channel_position_t pos;
 char * name;
} _roar_po_chanpos[] = {
 {PA_CHANNEL_POSITION_INVALID,               "invalid"                    },
 {PA_CHANNEL_POSITION_MONO,                  "mono"                       },
 {PA_CHANNEL_POSITION_LEFT,                  "left"                       },
 {PA_CHANNEL_POSITION_RIGHT,                 "right"                      },
 {PA_CHANNEL_POSITION_CENTER,                "center"                     },
 {PA_CHANNEL_POSITION_FRONT_LEFT,            "front-left"                 },
 {PA_CHANNEL_POSITION_FRONT_RIGHT,           "front-right"                },
 {PA_CHANNEL_POSITION_FRONT_CENTER,          "front-center"               },
 {PA_CHANNEL_POSITION_REAR_CENTER,           "rear-center"                },
 {PA_CHANNEL_POSITION_REAR_LEFT,             "rear-left"                  },
 {PA_CHANNEL_POSITION_REAR_RIGHT,            "rear-right"                 },
 {PA_CHANNEL_POSITION_LFE,                   "lfe"                        },
 {PA_CHANNEL_POSITION_SUBWOOFER,             "subwoofer"                  },
 {PA_CHANNEL_POSITION_FRONT_LEFT_OF_CENTER,  "front-left-of-center"       },
 {PA_CHANNEL_POSITION_FRONT_RIGHT_OF_CENTER, "front-right-of-center"      },
 {PA_CHANNEL_POSITION_SIDE_LEFT,             "side-left"                  },
 {PA_CHANNEL_POSITION_SIDE_RIGHT,            "side-right"                 },
 {PA_CHANNEL_POSITION_AUX0,                  "aux0"                       },
 {PA_CHANNEL_POSITION_AUX1,                  "aux1"                       },
 {PA_CHANNEL_POSITION_AUX2,                  "aux2"                       },
 {PA_CHANNEL_POSITION_AUX3,                  "aux3"                       },
 {PA_CHANNEL_POSITION_AUX4,                  "aux4"                       },
 {PA_CHANNEL_POSITION_AUX5,                  "aux5"                       },
 {PA_CHANNEL_POSITION_AUX6,                  "aux6"                       },
 {PA_CHANNEL_POSITION_AUX7,                  "aux7"                       },
 {PA_CHANNEL_POSITION_AUX8,                  "aux8"                       },
 {PA_CHANNEL_POSITION_AUX9,                  "aux9"                       },
 {PA_CHANNEL_POSITION_AUX10,                 "aux10"                      },
 {PA_CHANNEL_POSITION_AUX11,                 "aux11"                      },
 {PA_CHANNEL_POSITION_AUX12,                 "aux12"                      },
 {PA_CHANNEL_POSITION_AUX13,                 "aux13"                      },
 {PA_CHANNEL_POSITION_AUX14,                 "aux14"                      },
 {PA_CHANNEL_POSITION_AUX15,                 "aux15"                      },
 {PA_CHANNEL_POSITION_AUX16,                 "aux16"                      },
 {PA_CHANNEL_POSITION_AUX17,                 "aux17"                      },
 {PA_CHANNEL_POSITION_AUX18,                 "aux18"                      },
 {PA_CHANNEL_POSITION_AUX19,                 "aux19"                      },
 {PA_CHANNEL_POSITION_AUX20,                 "aux20"                      },
 {PA_CHANNEL_POSITION_AUX21,                 "aux21"                      },
 {PA_CHANNEL_POSITION_AUX22,                 "aux22"                      },
 {PA_CHANNEL_POSITION_AUX23,                 "aux23"                      },
 {PA_CHANNEL_POSITION_AUX24,                 "aux24"                      },
 {PA_CHANNEL_POSITION_AUX25,                 "aux25"                      },
 {PA_CHANNEL_POSITION_AUX26,                 "aux26"                      },
 {PA_CHANNEL_POSITION_AUX27,                 "aux27"                      },
 {PA_CHANNEL_POSITION_AUX28,                 "aux28"                      },
 {PA_CHANNEL_POSITION_AUX29,                 "aux29"                      },
 {PA_CHANNEL_POSITION_AUX30,                 "aux30"                      },
 {PA_CHANNEL_POSITION_AUX31,                 "aux31"                      },
 {PA_CHANNEL_POSITION_TOP_CENTER,            "top-center"                 },
 {PA_CHANNEL_POSITION_TOP_FRONT_LEFT,        "top-front-left"             },
 {PA_CHANNEL_POSITION_TOP_FRONT_RIGHT,       "top-front-right"            },
 {PA_CHANNEL_POSITION_TOP_FRONT_CENTER,      "top-front-center"           },
 {PA_CHANNEL_POSITION_TOP_REAR_LEFT,         "top-rear-left"              },
 {PA_CHANNEL_POSITION_TOP_REAR_RIGHT,        "top-rear-right"             },
 {PA_CHANNEL_POSITION_TOP_REAR_CENTER,       "top-rear-center"            },
 {PA_CHANNEL_POSITION_MAX,                   "max"                        },
 {PA_CHANNEL_POSITION_INVALID, NULL}
};

/** Return a text label for the specified channel position */
const char* pa_channel_position_to_string(pa_channel_position_t pos) {
 int i;

 for (i = 0; _roar_po_chanpos[i].name != NULL; i++)
  if ( _roar_po_chanpos[i].pos == pos )
   return _roar_po_chanpos[i].name;

 return NULL;
}

/** The maximum length of strings returned by pa_channel_map_snprint() */
#define PA_CHANNEL_MAP_SNPRINT_MAX 336

/** Make a humand readable string from the specified channel map */
char* pa_channel_map_snprint(char *s, size_t l, const pa_channel_map *map) {
 const char * g;
 char * c = s;
 int i;
 size_t len;
 size_t todo = l;

 if ( map == NULL || s == NULL || l == 0 )
  return NULL;

 *c = 0;

 for (i = 0; i < map->channels; i++) {
  g = pa_channel_position_to_string(map->map[i]);

  if ( g == NULL )
   return NULL;

  len = strlen(g);

  if ( (len + 1) < todo ) {
   memcpy(c, g, len);
   c[len] = ',';
   c    += len + 1;
   todo -= len + 1;
  } else {
   return NULL;
  }
 }

 c[-1]  = 0;
 s[l-1] = 0;

 return s;
}

/** Parse a channel position list into a channel map structure. \since 0.8.1 */
pa_channel_map *pa_channel_map_parse(pa_channel_map *map, const char *s) {
 int i;
 char * c;
 size_t len;
 pa_channel_position_t * v;

 if ( map == NULL )
  return NULL;

 map->channels = 0;

 if ( s == NULL )
  return map;

 while (*s != 0) {
  c = strstr(s, ",");

  if ( c == NULL ) {
   len = strlen(s);
  } else {
   len = c - s;
  }

  v = &(map->map[map->channels++]);

  *v = PA_CHANNEL_POSITION_INVALID;

  for (i = 0; *v == PA_CHANNEL_POSITION_INVALID && _roar_po_chanpos[i].name != NULL; i++) {
   if ( !strncasecmp(_roar_po_chanpos[i].name, s, len) && _roar_po_chanpos[i].name[len] == 0 ) {
    *v = _roar_po_chanpos[i].pos;
   }
  }

  if ( c == NULL ) {
   break;
  } else {
   s = c + 1;
  }
 }

 return map;
}

/** Compare two channel maps. Return 1 if both match. */
int pa_channel_map_equal(const pa_channel_map *a, const pa_channel_map *b);

/** Return non-zero of the specified channel map is considered valid */
int pa_channel_map_valid(const pa_channel_map *map);

//ll
