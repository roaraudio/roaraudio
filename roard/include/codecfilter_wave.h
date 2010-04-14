//codecfilter_wave.h:

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

#ifndef _CODECFILTER_WAVE_H_
#define _CODECFILTER_WAVE_H_

#include <roaraudio.h>

#ifndef ROAR_WITHOUT_CF_WAVE

// most important codec IDs:
#define ROAR_RIFF_WAVE_CID_PCM        0x0001
#define ROAR_RIFF_WAVE_CID_IEEE_FLOAT 0x0003
#define ROAR_RIFF_WAVE_CID_ALAW       0x0006
#define ROAR_RIFF_WAVE_CID_MULAW      0x0007

// Invalid but used by RSound:
#define ROAR_RIFF_WAVE_CID_RSOUND     0x0000

// Type is encoded in length by RSound using the following values:
#define ROAR_RIFF_WAVE_RSID_S16_LE    0x0001
#define ROAR_RIFF_WAVE_RSID_S16_BE    0x0002
#define ROAR_RIFF_WAVE_RSID_U16_LE    0x0004
#define ROAR_RIFF_WAVE_RSID_U16_BE    0x0008
#define ROAR_RIFF_WAVE_RSID_U8        0x0010
#define ROAR_RIFF_WAVE_RSID_S8        0x0020


struct codecfilter_wave_inst {
 struct roar_stream_server *  stream;
 struct roar_stream_server * vstream;
 int opened;
};


int cf_wave_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_wave_close(CODECFILTER_USERDATA_T   inst);

int cf_wave_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_wave_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

int cf_wave_ctl(CODECFILTER_USERDATA_T   inst, int cmd, void * data);

#endif

#endif

//ll
