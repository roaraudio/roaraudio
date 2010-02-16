//channels.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
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

#include "libroardsp.h"

static struct {
 int id;
 char * name;
 char * sn;
} _g_chans[] = {
 {ROARDSP_CHAN_NONE,           "NONE",           "NONE"           },
 {ROARDSP_CHAN_FRONT_LEFT,     "FRONT_LEFT",     "FL"             },
 {ROARDSP_CHAN_FRONT_RIGHT,    "FRONT_RIGHT",    "FR"             },
 {ROARDSP_CHAN_SIDE_LEFT,      "SIDE_LEFT",      "SL"             },
 {ROARDSP_CHAN_SIDE_RIGHT,     "SIDE_RIGHT",     "SR"             },
 {ROARDSP_CHAN_BACK_LEFT,      "BACK_LEFT",      "BL"             },
 {ROARDSP_CHAN_BACK_RIGHT,     "BACK_RIGHT",     "BR"             },
 {ROARDSP_CHAN_FRONT_CENTER,   "FRONT_CENTER",   "FC"             },
 {ROARDSP_CHAN_SIDE_CENTER,    "SIDE_CENTER",    "SC"             },
 {ROARDSP_CHAN_BACK_CENTER,    "BACK_CENTER",    "BC"             },
 {ROARDSP_CHAN_LEFT,           "LEFT",           "L"              }, // alias
 {ROARDSP_CHAN_RIGHT,          "RIGHT",          "R"              }, // alias
 {ROARDSP_CHAN_CENTER,         "CENTER",         "C"              }, // alias
 {ROARDSP_CHAN_MONO,           "MONO",           "M"              }, // alias
 {ROARDSP_CHAN_MS_MID,         "MS_MID",         "MID"            },
 {ROARDSP_CHAN_MS_SIDE,        "MS_SIDE",        "SIDE"           },
 {ROARDSP_CHAN_LFE,            "LFE",            "LFE"            },
 {ROARDSP_CHAN_EOL, NULL, NULL}
};

char * roardsp_chan2str (int chan) {
 int i;

 for (i = 0; _g_chans[i].id != ROARDSP_CHAN_EOL; i++)
  if ( _g_chans[i].id == chan )
   return _g_chans[i].name;

 return NULL;
}

int roardsp_str2chan(char * str) {
 int i;

 for (i = 0; _g_chans[i].id != ROARDSP_CHAN_EOL; i++)
  if ( !strcasecmp(_g_chans[i].name, str) || !strcasecmp(_g_chans[i].sn, str) )
   return _g_chans[i].id;

 return -1;
}

int    roardsp_chanlist2str(char * list, size_t len, char * str, size_t strlen) {
 int i;

 if ( list == NULL && len > 0 )
  return -1;

 if ( (str == NULL || strlen == 0) && len > 0 )
  return -1;

 if ( len == 0 ) {
  if ( str != NULL && strlen > 0 )
   *str = 0;

  return 0;
 }

 // TODO: FIXME: do not ignore strlen from here
 *str = 0;

 for (i = 0; i < len; i++) {
  if ( i != 0 )
   strcat(str, ",");

  strcat(str, roardsp_chan2str(list[i]));
 }

 return 0;
}

int roardsp_chanmap_calc(struct roardsp_chanmap * map, int what, int err_on_none) {
 int a, b;

 if ( map == NULL )
  return -1;

 switch (what) {
  case ROARDSP_CHANMAP_MAP:
    memset(map->map, (char)-1, sizeof(map->map));

    for (a = 0; a < ROAR_MAX_CHANNELS; a++) {
     if ( map->in[a] == ROARDSP_CHAN_NONE )
      continue;

     for (b = 0; b < ROAR_MAX_CHANNELS; b++) {
      if ( map->in[a] == map->out[b] ) {
       map->map[a] = b;
       break;
      }
     }
     if ( b == ROAR_MAX_CHANNELS ) { // src not found in dest
      if ( err_on_none )
       return -1;

      map->map[a] = -1;
     }
    }
   break;
  case ROARDSP_CHANMAP_IN:
  case ROARDSP_CHANMAP_OUT:
  default:
    return -1;
   break;
 }

 return 0;
}

//ll
