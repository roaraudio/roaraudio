//hwmixer.c:

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

#include "roard.h"

void print_hwmixerlist (void) {
 printf("  Source   Flag Subsys - Description (devices)\n");
 printf("------------------------------------------------------\n");
}

int hwmixer_open(int basestream, char * drv, char * dev, int fh, char * basename, char * subnames) {
 return -1;
}

//ll
