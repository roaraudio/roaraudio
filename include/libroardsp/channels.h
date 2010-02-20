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
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARDSP_CHANNELS_H_
#define _LIBROARDSP_CHANNELS_H_

#include "libroardsp.h"

/* Channel map:
 * Standard marix:
 *
 *       # Left       | Center       | Right       #
 * ======#============#==============#=============#
 * Front # FRONT_LEFT | FRONT_CENTER | FRONT_RIGHT #
 *       # LEFT       | CENTER/MONO  | RIGHT       #
 * ------#------------+--------------+-------------#
 * Side  # SIDE_LEFT  | SIDE_CENTER$ | SIDE_RIGH   #
 * ------#------------+--------------+-------------#
 * Back  # BACK_LEFT  | BACK_CENTER  | BACK_RIGHT  #
 * ======#============#==============#=============#
 * $ = Side Center has null-vector possion and because of this
 *     arg(Pos) is undefined. This means that the channel does
 *     not have a defined phase and therefor does can not exist.
 *     we include it just to have the matrix complet.
 * 
 *
 * Low Frequency Effects:
 *
 *  LFE is location independ.
 *
 *
 * Mid-Side (MS):
 *
 * Mid   = Left + Right
 * Side  = Left - Right
 * Left  = (Mid + Side) / 2
 * Right = (Mid - Side) / 2
 *
 */

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

// MIDI Channels:
#define ROARDSP_CHAN_MIDI0                64
#define ROARDSP_CHAN_MIDI1                65
#define ROARDSP_CHAN_MIDI2                66
#define ROARDSP_CHAN_MIDI3                67
#define ROARDSP_CHAN_MIDI4                68
#define ROARDSP_CHAN_MIDI5                69
#define ROARDSP_CHAN_MIDI6                70
#define ROARDSP_CHAN_MIDI7                71
#define ROARDSP_CHAN_MIDI8                72
#define ROARDSP_CHAN_MIDI9                73
#define ROARDSP_CHAN_MIDI10               74
#define ROARDSP_CHAN_MIDI11               75
#define ROARDSP_CHAN_MIDI12               76
#define ROARDSP_CHAN_MIDI13               77
#define ROARDSP_CHAN_MIDI14               78
#define ROARDSP_CHAN_MIDI15               79

#define ROARDSP_CHANLIST_MAP_ROARAUDIO    0
#define ROARDSP_CHANLIST_MAP_VORBIS       1
#define ROARDSP_CHANLIST_MAP_FLAC         2
#define ROARDSP_CHANLIST_MAP_RIFF_WAVE    3
#define ROARDSP_CHANLIST_MAP_OSS          4
#define ROARDSP_CHANLIST_MAP_ALSA         5
#define ROARDSP_CHANLIST_MAP_ESD          6
#define ROARDSP_CHANLIST_MAP_MIDI         7

#define ROARDSP_CHANMAP_IN                0
#define ROARDSP_CHANMAP_OUT               1
#define ROARDSP_CHANMAP_MAP               2
#define ROARDSP_CHANMAP_INVMAP            3

struct roardsp_chanmap {
 char in [ROAR_MAX_CHANNELS];
 char out[ROAR_MAX_CHANNELS];
 char map[ROAR_MAX_CHANNELS];
};

char * roardsp_chan2str (int chan);
int    roardsp_str2chan(char * str);

int    roardsp_chanlist2str(char * list, size_t len, char * str, size_t strlen);

int    roardsp_chanlist_init(char * list, int channels, int map);

int roardsp_chanmap_calc(struct roardsp_chanmap * map, int what, int err_on_none);

int roardsp_chanmap_mappcm8 (char    * out, char    * in, size_t len, size_t chans, char * map);
int roardsp_chanmap_mappcm16(int16_t * out, int16_t * in, size_t len, size_t chans, char * map);
int roardsp_chanmap_mappcm24(void    * out, void    * in, size_t len, size_t chans, char * map);
int roardsp_chanmap_mappcm32(int32_t * out, int32_t * in, size_t len, size_t chans, char * map);
int roardsp_chanmap_mappcm  (void    * out, void    * in, size_t len, size_t chans, char * map, int bits);

#endif

//ll
