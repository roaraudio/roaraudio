//stream.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE: Even though this file is LGPLed it (may) include GPLed files
 *  so the license of this file is/may therefore downgraded to GPL.
 *  See HACKING for details.
 */

#ifndef _ROARAUDIO_STREAM_H_
#define _ROARAUDIO_STREAM_H_

#define ROAR_DIR_DEFAULT ROAR_DIR_PLAY

#define ROAR_DIR_PLAY        1
#define ROAR_DIR_RECORD      2
#define ROAR_DIR_MONITOR     3
#define ROAR_DIR_FILTER      4

#define ROAR_DIR_OUTPUT      5

#define ROAR_DIR_MIXING      6
//#define ROAR_DIR_INTERNAL 7

#define ROAR_DIR_META        8
#define ROAR_DIR_BIDIR       9

#define ROAR_DIR_THRU       10

#define ROAR_DIR_BRIDGE     11

#define ROAR_DIR_MIDI_IN    12
#define ROAR_DIR_MIDI_OUT   13

#define ROAR_DIR_LIGHT_IN   14
#define ROAR_DIR_LIGHT_OUT  15

#define ROAR_DIR_RAW_IN     16
#define ROAR_DIR_RAW_OUT    17

#define ROAR_DIR_DIRIDS     18


#define ROAR_CARE_NOPOS  0
#define ROAR_CARE_POS    1

#define ROAR_STREAM(a) ((struct roar_stream*)(a))

#define ROAR_STREAMS_MAX  64

struct roar_stream {
 int id;

 int fh;
 int dir;
 int care_pos;

 uint32_t pos;
 uint32_t pos_rel_id;

 struct roar_audio_info info;

/*
 int    datalen;
 char * database;
 int    offset;
 char * dataoff;
*/
};

#endif

//ll
