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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

//ll