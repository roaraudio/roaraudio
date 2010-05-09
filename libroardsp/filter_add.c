//add.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroardsp.h"

int roardsp_add_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_amp * self = roar_mm_malloc(sizeof(struct roardsp_amp));

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roardsp_amp));

 filter->inst = (void*) self;

 roardsp_filter_reset(filter, ROARDSP_RESET_FULL);

 return 0;
}

int roardsp_add_calc16  (struct roardsp_filter * filter, void * data, size_t samples) {
 struct roardsp_amp * self = (struct roardsp_amp *) filter->inst;
 int16_t * samp = (int16_t *) data;
 size_t i;

 for (i = 0; i < samples; i++) {
  samp[i] += self->mul / self->div;
 };

 return 0;
}

int roardsp_add_reset (struct roardsp_filter * filter, int what) {
 struct roardsp_amp * self;

 if ( filter == NULL )
  return -1;

 if ( filter->inst == NULL )
  return -1;

 self = filter->inst;

 switch (what) {
  case ROARDSP_RESET_NONE:
  case ROARDSP_RESET_STATE:
    return  0;
   break;
  case ROARDSP_RESET_FULL:
    self->mul = 0;
    self->div = 1;
    return  0;
   break;
  default:
    return -1;
 }

 return -1;
}

//ll
