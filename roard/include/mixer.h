//output.h:

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

#ifndef _MIXER_H_
#define _MIXER_H_

#include <roaraudio.h>

#define _MIXER_NAME_PREFIX ""
#define _MIXER_NAME_SUFFIX " Mixer"
#define _MIXER_NAME(x)     (_MIXER_NAME_PREFIX x _MIXER_NAME_SUFFIX)

int need_vol_change  (int channels, struct roar_mixer_settings * set);

int add_mixer (int subsys, char * name, struct roar_stream_server ** ss_ptr);

#endif

//ll
