//fader.h:

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

#ifndef _LIBROARDSP_FADER_H_
#define _LIBROARDSP_FADER_H_

#include "libroardsp.h"

#define ROAR_FADER_MAX_COEFF 4

struct roar_fader_state {
 int rate;
 size_t pcmoffset;
 size_t start;
 size_t stop;
 float poly[ROAR_FADER_MAX_COEFF];
 int   coeff;
};

int roar_fader_init         (struct roar_fader_state * state, float * poly, int coeff);
int roar_fader_set_rate     (struct roar_fader_state * state, int rate);
int roar_fader_set_startstop(struct roar_fader_state * state, ssize_t start, ssize_t stop);

int roar_fader_calcpcm_i16n(struct roar_fader_state * state, int16_t * data, size_t frames, int channels);
int roar_fader_calcpcm_i161(struct roar_fader_state * state, int16_t * data, size_t frames);

#endif

//ll
