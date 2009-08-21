//ssynth.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifndef _SSYNTH_H_
#define _SSYNTH_H_

#include <roaraudio.h>

#define SSYNTH_NOTES_MAX            16

#define SSYNTH_STAGE_NONE            0
#define SSYNTH_STAGE_UNUSED          SSYNTH_STAGE_NONE
#define SSYNTH_STAGE_KEYSTROKE       1
#define SSYNTH_STAGE_MIDSECTION      2
#define SSYNTH_STAGE_KEYRELEASE      3

#define SSYNTH_POLY_KEYDOWN          0
#define SSYNTH_POLY_KEYUP            1
#define SSYNTH_POLY_POLYMAX          2

#define SSYNTH_POLY_COEFF            4

float ssynth_polys[SSYNTH_POLY_POLYMAX][SSYNTH_POLY_COEFF];

struct {
 int enable;
} ssynth_conf;

struct {
 int stream;
 struct {
  int stage;
  char vv_down, vv_up;
  struct roar_note_octave note;
  struct roar_synth_state synth;
  struct roar_fader_state fader;
 } notes[SSYNTH_NOTES_MAX];
} g_ssynth;

int ssynth_init_config(void);

int ssynth_init (void);
int ssynth_free (void);

int ssynth_note_new(struct roar_note_octave * note, char vv);
int ssynth_note_free(int id);
int ssynth_note_find(struct roar_note_octave * note);

#endif

//ll
