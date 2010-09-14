//hwmixer.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#ifndef _HWMIXER_H_
#define _HWMIXER_H_

#include <roaraudio.h>

#define HWMIXER_MODE_SET        1

struct hwmixer;

struct hwmixer_stream {
 struct hwmixer * hwmixer;
 int basestream;
 int stream;
 void * baseud;
 void * ud;
};

void print_hwmixerlist (void);
int hwmixer_open(int basestream, char * drv, char * dev, int fh, char * basename, char * subnames);
int hwmixer_close(int stream);

int hwmixer_set_volume(int id, struct roar_stream_server * ss, struct hwmixer_stream * mstream, struct roar_mixer_settings * settings);

struct hwmixer_stream * hwmixer_substream_new(struct hwmixer_stream * parent);

// Mixers:

// dstr
int hwmixer_dstr_open(struct hwmixer_stream * stream, char * drv, char * dev, int fh, char * basename, struct roar_keyval * subnames, size_t subnamelen);
int hwmixer_dstr_close(struct hwmixer_stream * stream);
int hwmixer_dstr_set_vol(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings);

// OSS
int hwmixer_oss_open(struct hwmixer_stream * stream, char * drv, char * dev, int fh, char * basename, struct roar_keyval * subnames, size_t subnamelen);
int hwmixer_oss_close(struct hwmixer_stream * stream);
int hwmixer_oss_set_vol(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings);
int hwmixer_oss_get_vol(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings);

#endif

//ll
