//codecfilter_fishsound.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
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
 */

#ifndef _CODECFILTER_FISHSOUND_H_
#define _CODECFILTER_FISHSOUND_H_

#include <roaraudio.h>

#include <oggz/oggz.h>
#include <fishsound/fishsound.h>


struct codecfilter_fishsound_inst {
 struct roar_stream_server * stream;
 int opened;
 FishSoundInfo fsinfo;
 FishSound * fsound;
 OGGZ * oggz;
 struct roar_buffer * buffer;
};


int cf_fishsound_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_fishsound_close(CODECFILTER_USERDATA_T   inst);

int cf_fishsound_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);

#endif

//ll
