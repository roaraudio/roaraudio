//sample.h:

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

#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <roaraudio.h>

#define ROAR_SAMPLES_MAX  32

struct roar_sample * g_samples[ROAR_SAMPLES_MAX];

int samples_init      (void);
int samples_free      (void);
int samples_new       (void);
int samples_delete    (int id);
int samples_set_name  (int id, char * name);
int samples_add_data  (int id, void * data, size_t len);

#endif

//ll
