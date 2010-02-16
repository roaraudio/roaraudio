//amp.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - August 2008
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

int roardsp_amp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_amp * self = roar_mm_malloc(sizeof(struct roardsp_amp));

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roardsp_amp));

 filter->inst = (void*) self;

 roardsp_filter_reset(filter, ROARDSP_RESET_FULL);

 return 0;
}

int roardsp_amp_uninit(struct roardsp_filter * filter) {

 roar_mm_free(filter->inst);
 return 0;
}

int roardsp_amp_calc8  (struct roardsp_filter * filter, void * data, size_t samples) {
 struct roardsp_amp * self = (struct roardsp_amp *) filter->inst;
 int8_t * samp = (int8_t *) data;
 register int_least32_t s;
 size_t i;

 for (i = 0; i < samples; i++) {
  s        = samp[i];
  s       *= self->mul;
  s       /= self->div;
  samp[i]  = s;
 };

 return 0;
}

int roardsp_amp_calc16  (struct roardsp_filter * filter, void * data, size_t samples) {
 struct roardsp_amp * self = (struct roardsp_amp *) filter->inst;
 int16_t * samp = (int16_t *) data;
 register int_least32_t s;
 size_t i;

 for (i = 0; i < samples; i++) {
  s        = samp[i];
  s       *= self->mul;
  s       /= self->div;
//  ROAR_DBG("roardsp_amp_calc16(*): samp[i=%u] = %i, s=%i, self->mul=%i, self->div=%i", i, samp[i], s, self->mul, self->div);
  samp[i]  = s;
 };

 return 0;
}

int roardsp_amp_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 struct roardsp_amp * self = (struct roardsp_amp *) filter->inst;
 int32_t old;

 if ( cmd == ROARDSP_FCTL_MUL ) {
  old = self->div;
  self->mul = *(int32_t*)data;
  *(int32_t*)data = old;
 } else if ( cmd == ROARDSP_FCTL_DIV ) {
  old = self->div;
  self->div = *(int32_t*)data;
  *(int32_t*)data = old;
 } else {
  return -1;
 }


 return 0;
}

int roardsp_amp_reset (struct roardsp_filter * filter, int what) {
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
    self->mul = 1;
    self->div = 1;
    return  0;
   break;
  default:
    return -1;
 }

 return -1;
}

//ll
