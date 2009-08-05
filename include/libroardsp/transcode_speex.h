//transcode_speex.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARDSP_TRANSCODE_SPEEX_H_
#define _LIBROARDSP_TRANSCODE_SPEEX_H_

#include "libroardsp.h"

#include <speex/speex.h>
#include <speex/speex_stereo.h>

struct roar_xcoder_speex {
 SpeexBits   bits;
 void      * xcoder;
 uint16_t    mode;
 int         stereo;
 int frame_size;
 char       cc[ROAR_SPEEX_MAX_CC]; /* buffer for read() and write() */
};

int roar_xcoder_speex_init       (struct roar_xcoder * state);
int roar_xcoder_speex_uninit     (struct roar_xcoder * state);
int roar_xcoder_speex_packet_size(struct roar_xcoder * state, int samples);
int roar_xcoder_speex_encode     (struct roar_xcoder * state, void * buf, size_t len);
int roar_xcoder_speex_decode     (struct roar_xcoder * state, void * buf, size_t len);

#endif

//ll
