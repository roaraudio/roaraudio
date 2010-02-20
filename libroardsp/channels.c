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
 // general:
 {ROARDSP_CHAN_NONE,           "NONE",           "NONE"           },
 // waveform:
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
 // midi:
 {ROARDSP_CHAN_MIDI0,           "MIDI0",         NULL             },
 {ROARDSP_CHAN_MIDI1,           "MIDI1",         NULL             },
 {ROARDSP_CHAN_MIDI2,           "MIDI2",         NULL             },
 {ROARDSP_CHAN_MIDI3,           "MIDI3",         NULL             },
 {ROARDSP_CHAN_MIDI4,           "MIDI4",         NULL             },
 {ROARDSP_CHAN_MIDI5,           "MIDI5",         NULL             },
 {ROARDSP_CHAN_MIDI6,           "MIDI6",         NULL             },
 {ROARDSP_CHAN_MIDI7,           "MIDI7",         NULL             },
 {ROARDSP_CHAN_MIDI8,           "MIDI8",         NULL             },
 {ROARDSP_CHAN_MIDI9,           "MIDI9",         NULL             },
 {ROARDSP_CHAN_MIDI10,          "MIDI10",        NULL             },
 {ROARDSP_CHAN_MIDI11,          "MIDI11",        NULL             },
 {ROARDSP_CHAN_MIDI12,          "MIDI12",        NULL             },
 {ROARDSP_CHAN_MIDI13,          "MIDI13",        NULL             },
 {ROARDSP_CHAN_MIDI14,          "MIDI14",        NULL             },
 {ROARDSP_CHAN_MIDI15,          "MIDI15",        NULL             },
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
  if ( !strcasecmp(_g_chans[i].name, str) ||
       (_g_chans[i].sn != NULL && !strcasecmp(_g_chans[i].sn, str)) )
   return _g_chans[i].id;

 return -1;
}

int    roardsp_chanlist2str(char * list, size_t len, char * str, size_t strlen) {
 char * name;
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

  name = roardsp_chan2str(list[i]);

  if ( name == NULL ) {
   strcat(str, "<<<INVALID>>>");
  } else {
   strcat(str, name);
  }
 }

 return 0;
}

int    roardsp_chanlist_init(char * list, int channels, int map) {
 int i;

 if ( channels == 0 )
  return 0;

 if ( list == NULL )
  return -1;

 if ( channels > ROAR_MAX_CHANNELS )
  return -1;

 if ( map == ROARDSP_CHANLIST_MAP_MIDI ) {
  for (i = 0; i < channels; i++) {
   list[i] = i+ROARDSP_CHAN_MIDI0;
  }
 }

 // test for common maps:
 if ( channels == 1 ) {
  list[0] = ROARDSP_CHAN_MONO;
  return 0;
 }

 if ( channels == 2 ) {
  list[0] = ROARDSP_CHAN_LEFT;
  list[1] = ROARDSP_CHAN_RIGHT;
  return 0;
 }

 // test for specific maps:
 switch (map) {
  case ROARDSP_CHANLIST_MAP_ROARAUDIO:
  case ROARDSP_CHANLIST_MAP_FLAC:
    if ( map == ROARDSP_CHANLIST_MAP_FLAC && channels > 6 ) {
     // FLAC only has a fixed mapping for up to 6 channels.
     return -1;
    }

    switch (channels) {
     // FLAC or RoarAudio:
     case 3:
       list[0] = ROARDSP_CHAN_LEFT;
       list[1] = ROARDSP_CHAN_RIGHT;
       list[2] = ROARDSP_CHAN_CENTER;
      break;
     case 4:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_FRONT_RIGHT;
       list[2] = ROARDSP_CHAN_BACK_LEFT;
       list[3] = ROARDSP_CHAN_BACK_RIGHT;
      break;
     case 5:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_FRONT_RIGHT;
       list[2] = ROARDSP_CHAN_CENTER;
       list[3] = ROARDSP_CHAN_BACK_LEFT;
       list[4] = ROARDSP_CHAN_BACK_RIGHT;
      break;
     case 6:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_FRONT_RIGHT;
       list[2] = ROARDSP_CHAN_CENTER;
       list[3] = ROARDSP_CHAN_LFE;
       list[4] = ROARDSP_CHAN_BACK_LEFT;
       list[5] = ROARDSP_CHAN_BACK_RIGHT;
      break;
     // RoarAudio:
     case 7:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_FRONT_RIGHT;
       list[2] = ROARDSP_CHAN_CENTER;
       list[3] = ROARDSP_CHAN_LFE;
       list[4] = ROARDSP_CHAN_SIDE_LEFT;
       list[5] = ROARDSP_CHAN_SIDE_RIGHT;
       list[6] = ROARDSP_CHAN_BACK_CENTER;
      break;
     case 8:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_FRONT_RIGHT;
       list[2] = ROARDSP_CHAN_CENTER;
       list[3] = ROARDSP_CHAN_LFE;
       list[4] = ROARDSP_CHAN_SIDE_LEFT;
       list[5] = ROARDSP_CHAN_SIDE_RIGHT;
       list[6] = ROARDSP_CHAN_BACK_LEFT;
       list[7] = ROARDSP_CHAN_BACK_RIGHT;
      break;
     default:
       return -1;
      break;
    }
   break;
  case ROARDSP_CHANLIST_MAP_VORBIS:
    switch (channels) {
     case 3:
       list[0] = ROARDSP_CHAN_LEFT;
       list[1] = ROARDSP_CHAN_CENTER;
       list[2] = ROARDSP_CHAN_RIGHT;
      break;
     case 4:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_FRONT_RIGHT;
       list[2] = ROARDSP_CHAN_BACK_LEFT;
       list[3] = ROARDSP_CHAN_BACK_RIGHT;
      break;
     case 5:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_CENTER;
       list[2] = ROARDSP_CHAN_FRONT_RIGHT;
       list[3] = ROARDSP_CHAN_BACK_LEFT;
       list[4] = ROARDSP_CHAN_BACK_RIGHT;
      break;
     case 6:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_CENTER;
       list[2] = ROARDSP_CHAN_FRONT_RIGHT;
       list[3] = ROARDSP_CHAN_BACK_LEFT;
       list[4] = ROARDSP_CHAN_BACK_RIGHT;
       list[5] = ROARDSP_CHAN_LFE;
      break;
     case 7:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_CENTER;
       list[2] = ROARDSP_CHAN_FRONT_RIGHT;
       list[3] = ROARDSP_CHAN_SIDE_LEFT;
       list[4] = ROARDSP_CHAN_SIDE_RIGHT;
       list[5] = ROARDSP_CHAN_BACK_CENTER;
       list[6] = ROARDSP_CHAN_LFE;
      break;
     case 8:
       list[0] = ROARDSP_CHAN_FRONT_LEFT;
       list[1] = ROARDSP_CHAN_CENTER;
       list[2] = ROARDSP_CHAN_FRONT_RIGHT;
       list[3] = ROARDSP_CHAN_SIDE_LEFT;
       list[4] = ROARDSP_CHAN_SIDE_RIGHT;
       list[5] = ROARDSP_CHAN_BACK_LEFT;
       list[6] = ROARDSP_CHAN_BACK_RIGHT;
       list[7] = ROARDSP_CHAN_LFE;
      break;
     default:
       return -1;
      break;
    }
   break;
  case ROARDSP_CHANLIST_MAP_OSS:
    switch (channels) {
     case 8:
       list[7] = ROARDSP_CHAN_BACK_RIGHT;
     case 7:
       list[6] = ROARDSP_CHAN_BACK_LEFT;
     case 6:
       list[5] = ROARDSP_CHAN_SIDE_RIGHT;
     case 5:
       list[4] = ROARDSP_CHAN_SIDE_LEFT;
     case 4:
       list[3] = ROARDSP_CHAN_LFE;
     case 3:
       list[2] = ROARDSP_CHAN_CENTER;
       list[1] = ROARDSP_CHAN_RIGHT;
       list[0] = ROARDSP_CHAN_LEFT;
      break;
     default:
       return -1;
      break;
    }
   break;
  case ROARDSP_CHANLIST_MAP_ALSA:
    // we guess: L,R,BL,BR,C,LFE,SL,SR (really, just guessing, see ALSA plugin of libao)
    switch (channels) {
     case 8:
       list[7] = ROARDSP_CHAN_BACK_RIGHT;
     case 7:
       list[6] = ROARDSP_CHAN_BACK_LEFT;
     case 6:
       list[5] = ROARDSP_CHAN_LFE;
     case 5:
       list[4] = ROARDSP_CHAN_CENTER;
     case 4:
       list[3] = ROARDSP_CHAN_BACK_RIGHT;
     case 3:
       list[2] = ROARDSP_CHAN_BACK_LEFT;
       list[1] = ROARDSP_CHAN_RIGHT;
       list[0] = ROARDSP_CHAN_LEFT;
      break;
     default:
       return -1;
      break;
    }
   break;
  case ROARDSP_CHANLIST_MAP_RIFF_WAVE:
    // here we are again, guessing:  L,R,C,LFE,BL,BR,CL,CR,BC,SL,SR
    // strange, C, LFE on 4 channel file?
    // return -1 to be sure....
    switch (channels) {
     default:
       return -1;
      break;
    }
   break;
  default:
    return -1;
   break;
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
  case ROARDSP_CHANMAP_INVMAP:
    memset(map->map, (char)-1, sizeof(map->map));

    for (a = 0; a < ROAR_MAX_CHANNELS; a++) {
     if ( map->out[a] == ROARDSP_CHAN_NONE )
      continue;

     for (b = 0; b < ROAR_MAX_CHANNELS; b++) {
      if ( map->out[a] == map->in[b] ) {
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
