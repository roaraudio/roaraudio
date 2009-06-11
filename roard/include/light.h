//light.h:

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

#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <roaraudio.h>

#define LIGHT_CHANNELS_DEFAULT 512 /* one DMX Universe */

struct {
 unsigned int    channels;
 unsigned char * state;
 unsigned char * changes;
} g_light_state;

int light_init  (unsigned int channels);
int light_free  (void);

int light_update(void);
int light_reset (void);
int light_reinit(void);

int light_check_stream  (int id);
int light_send_stream   (int id);

#endif

//ll
