//driver_roar.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include <roaraudio.h>
#include "driver.h"

int roar_cdriver_roar(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir) {
 return roar_vio_simple_stream(calls, info->rate, info->channels, info->bits, info->codec, dev, dir, "libroareio client");
}

//ll
