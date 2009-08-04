//sources.h:

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

#ifndef _SOURCES_H_
#define _SOURCES_H_

#include <roaraudio.h>

#define SRC_FLAG_NONE           0x00
#define SRC_FLAG_FHSEC          0x01

struct roar_source {
 char * name;
 char * desc;
 char * devices;
 unsigned int flags;
 unsigned int subsystems;
 int (*old_open)(char * driver, char * device, char * container, char * options, int primary);
 int (*new_open)(int stream   , char * device, int fh);
};

int g_source_client;

int sources_init (void);
int sources_free (void);

int sources_set_client (int client);

void print_sourcelist (void);

int sources_add (char * driver, char * device, char * container, char * options, int primary);

int sources_add_raw  (char * driver, char * device, char * container, char * options, int primary);
int sources_add_wav  (char * driver, char * device, char * container, char * options, int primary);
int sources_add_cf   (char * driver, char * device, char * container, char * options, int primary);
int sources_add_roar (char * driver, char * device, char * container, char * options, int primary);

#endif

//ll
