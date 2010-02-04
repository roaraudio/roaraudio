//channels.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
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
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARDSP_CHANNELS_H_
#define _LIBROARDSP_CHANNELS_H_

#include "libroardsp.h"

#define ROARDSP_CHAN_EOL                 -1
#define ROARDSP_CHAN_NONE                 0
#define ROARDSP_CHAN_FRONT_LEFT           1
#define ROARDSP_CHAN_FRONT_RIGHT          2
#define ROARDSP_CHAN_SIDE_LEFT            3
#define ROARDSP_CHAN_SIDE_RIGHT           4
#define ROARDSP_CHAN_BACK_LEFT            5
#define ROARDSP_CHAN_BACK_RIGHT           6
#define ROARDSP_CHAN_FRONT_CENTER         7
#define ROARDSP_CHAN_SIDE_CENTER          8
#define ROARDSP_CHAN_BACK_CENTER          9

#define ROARDSP_CHAN_LEFT                 ROARDSP_CHAN_FRONT_LEFT
#define ROARDSP_CHAN_RIGHT                ROARDSP_CHAN_FRONT_RIGHT
#define ROARDSP_CHAN_CENTER               ROARDSP_CHAN_FRONT_CENTER
#define ROARDSP_CHAN_MONO                 ROARDSP_CHAN_CENTER

#define ROARDSP_CHAN_MS_MID               10
#define ROARDSP_CHAN_MS_SIDE              11

#define ROARDSP_CHAN_LFE                  12

#define ROARDSP_CHANMAP_IN                0
#define ROARDSP_CHANMAP_OUT               1
#define ROARDSP_CHANMAP_MAP               2

struct roardsp_chanmap {
 char in [ROAR_MAX_CHANNELS];
 char out[ROAR_MAX_CHANNELS];
 char map[ROAR_MAX_CHANNELS];
};

char * roardsp_chan2str (int chan);
int    roardsp_str2chan(char * str);

int    roardsp_chanlist2str(int * list, size_t len, char * str, size_t strlen);

int roardsp_chanmap_calc(struct roardsp_chanmap * map, int what, int err_on_none);

#endif

//ll
