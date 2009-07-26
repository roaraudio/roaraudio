//transcode_celt.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroardsp.h"

#ifdef ROAR_HAVE_LIBCELT

int roar_xcoder_celt_init       (struct roar_xcoder * state) {
 return -1;
}

int roar_xcoder_celt_uninit     (struct roar_xcoder * state) {
 return -1;
}

int roar_xcoder_celt_packet_size(struct roar_xcoder * state, int samples) {
 return -1;
}

int roar_xcoder_celt_encode     (struct roar_xcoder * state, void * buf, size_t len) {
 return -1;
}

int roar_xcoder_celt_decode     (struct roar_xcoder * state, void * buf, size_t len) {
 return -1;
}

#endif

//ll
