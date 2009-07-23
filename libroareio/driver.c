//driver.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <roaraudio.h>
#include "driver.h"

struct roar_cdriver _g_roar_cdriver[] = {
 {NULL, NULL}
};

int roar_cdriver_open(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir) {
 int i;

 for (i = 0; _g_roar_cdriver[i].name != NULL; i++) {
  return _g_roar_cdriver[i].open(calls, name, dev, info, dir);
 }

 return -1;
}


//ll
