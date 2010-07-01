//codecfilter_au.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#ifndef _CODECFILTER_AU_H_
#define _CODECFILTER_AU_H_

#include <roaraudio.h>

#ifndef ROAR_WITHOUT_CF_AU

#define ROAR_AU_MIN_HEADER_LEN        (6*4)
#define ROAR_AU_MAGIC                 0x2e736e64

#define ROAR_AU_DATASIZE              0xffffffff

// most important codec IDs:
#define ROAR_AU_CID_MULAW             0x00000001
#define ROAR_AU_CID_PCM_S_8           0x00000002
#define ROAR_AU_CID_PCM_S_16          0x00000003
#define ROAR_AU_CID_PCM_S_24          0x00000004
#define ROAR_AU_CID_PCM_S_32          0x00000005
#define ROAR_AU_CID_ALAW              0x00000033


struct codecfilter_au_inst {
 struct roar_stream_server *  stream;
 struct roar_stream_server * vstream;
 int opened;
};


int cf_au_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_au_close(CODECFILTER_USERDATA_T   inst);

int cf_au_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_au_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

int cf_au_ctl(CODECFILTER_USERDATA_T   inst, int cmd, void * data);

#endif

#endif

//ll
