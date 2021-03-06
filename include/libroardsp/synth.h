//synth.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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

#ifndef _LIBROARDSP_SYNTH_H_
#define _LIBROARDSP_SYNTH_H_

#include "libroardsp.h"

#define ROAR_SYNTH_FUNC_TYPE(name)   float (*name)(float t, struct roar_synth_state * state)
#define ROAR_SYNTH_FUNC_CAST(name) ((float (*    )(float t, struct roar_synth_state * state)) name)

// SYNF -> Synthesis Function
#define ROAR_SYNTH_SYNF_RECT ROAR_SYNTH_FUNC_CAST(roar_synth_synf_rect)
#define ROAR_SYNTH_SYNF_SAW  ROAR_SYNTH_FUNC_CAST(roar_synth_synf_saw)
#define ROAR_SYNTH_SYNF_TRI  ROAR_SYNTH_FUNC_CAST(roar_synth_synf_tri)
#define ROAR_SYNTH_SYNF_TRAP ROAR_SYNTH_FUNC_CAST(roar_synth_synf_trap)

// those use sin*()
#ifdef ROAR_HAVE_LIBM
#define ROAR_SYNTH_SYNF_SIN  ROAR_SYNTH_FUNC_CAST(sinf)
#define ROAR_SYNTH_SYNF_S2S  ROAR_SYNTH_FUNC_CAST(roar_synth_synf_s2s)
#endif

#ifdef ROAR_HAVE_LIBM
#define ROAR_SYNTH_SYNF_DEFAULT ROAR_SYNTH_SYNF_SIN
#else
#define ROAR_SYNTH_SYNF_DEFAULT ROAR_SYNTH_SYNF_TRAP
#endif

struct roar_synth_state {
 int rate;
 struct roar_note_octave * note;
 ROAR_SYNTH_FUNC_TYPE(func);
 size_t pcmoffset;
 float volume;
};

int roar_synth_init(struct roar_synth_state * state, struct roar_note_octave * note, int rate);
int roar_synth_set_offset(struct roar_synth_state * state, size_t offset);
int roar_synth_set_func  (struct roar_synth_state * state, ROAR_SYNTH_FUNC_TYPE(func));
int roar_synth_set_volume(struct roar_synth_state * state, float volume);

int roar_synth_pcmout_i16n(struct roar_synth_state * state, int16_t * out, size_t frames, int channels);
int roar_synth_pcmout_i161(struct roar_synth_state * state, int16_t * out, size_t frames);

// some basic SYNFs:
float roar_synth_synf_rect (float t, struct roar_synth_state * state);
float roar_synth_synf_saw  (float t, struct roar_synth_state * state);
float roar_synth_synf_tri  (float t, struct roar_synth_state * state);
float roar_synth_synf_trap (float t, struct roar_synth_state * state);

#ifdef ROAR_HAVE_LIBM
float roar_synth_synf_s2s  (float t, struct roar_synth_state * state);
#endif

#endif

//ll
