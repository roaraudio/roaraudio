//codecfilter.h:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _CODECFILTER_H_
#define _CODECFILTER_H_

#include <roaraudio.h>

#define CODECFILTER_USERDATA_T void *

struct roar_stream_server;

#define ROAR_CODECFILTER_NONE  0
#define ROAR_CODECFILTER_READ  1
#define ROAR_CODECFILTER_WRITE 2

struct roar_codecfilter {
 int    codec;
 char * name;
 char * desc;
 char * options;
 int    flags;
 int (*open )(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter);
 int (*close)(CODECFILTER_USERDATA_T   inst);
 int (*pause)(CODECFILTER_USERDATA_T   inst, int newstate);
 int (*write)(CODECFILTER_USERDATA_T   inst, char * buf, int len);
 int (*read )(CODECFILTER_USERDATA_T   inst, char * buf, int len);
 int (*flush)(CODECFILTER_USERDATA_T   inst);
};


#include "codecfilter_cmd.h"
#include "codecfilter_wave.h"


#ifdef ROAR_HAVE_LIBVORBISFILE
#include "codecfilter_vorbis.h"
#endif

#ifdef ROAR_HAVE_LIBCELT
#include "codecfilter_celt.h"
#endif

#ifdef ROAR_HAVE_LIBSPEEX
#include "codecfilter_speex.h"
#endif

#ifdef ROAR_HAVE_LIBSNDFILE
#include "codecfilter_sndfile.h"
#endif

#ifdef ROAR_HAVE_LIBFISHSOUND
#include "codecfilter_fishsound.h"
#endif

//cf alaw/mulaw:
#include <libroardsp/libroardsp.h>

void print_codecfilterlist (void);

int codecfilter_open (CODECFILTER_USERDATA_T * inst,
                 int * codecfilter_id, char * codecfilter /* NOTE: this is not part of struct roar_codecfilter's def! */,
                 int codec, struct roar_stream_server * info);
int codecfilter_close(CODECFILTER_USERDATA_T   inst, int codecfilter);
int codecfilter_pause(CODECFILTER_USERDATA_T   inst, int codecfilter, int newstate);
int codecfilter_write(CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len);
int codecfilter_read (CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len);
int codecfilter_flush(CODECFILTER_USERDATA_T   inst, int codecfilter);

// codecfilter without a own .h:
int cf_alaw_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_alaw_close(CODECFILTER_USERDATA_T   inst);

int cf_alaw_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_alaw_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

int cf_mulaw_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_mulaw_close(CODECFILTER_USERDATA_T   inst);

int cf_mulaw_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_mulaw_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);


#endif

//ll
