//filter_agc.c:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroardsp.h"

int roardsp_agc_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_agc * self = roar_mm_malloc(sizeof(struct roardsp_agc));

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roardsp_agc));

 if ( roardsp_filter_new(&(self->amp), stream, ROARDSP_FILTER_AMP) == -1 ) {
  roar_mm_free(self);
  return -1;
 }

 filter->inst = (void*) self;

 if ( roardsp_filter_reset(filter, ROARDSP_RESET_FULL) == -1 ) {
  roardsp_agc_uninit(filter);
  return -1;
 }

 return 0;
}

int roardsp_agc_uninit (struct roardsp_filter * filter) {
 struct roardsp_agc * self = filter->inst;

 roardsp_filter_free(self->amp);
 roar_mm_free(self);
 return 0;
}

int roardsp_agc_ctl    (struct roardsp_filter * filter, int cmd, void * data) {
 struct roardsp_agc * self = filter->inst;

 return -1;
}

int roardsp_agc_reset  (struct roardsp_filter * filter, int what) {
 struct roardsp_agc * self = filter->inst;

 switch (what) {
  case ROARDSP_RESET_NONE:
    return 0;
   break;
  case ROARDSP_RESET_FULL:
    if ( filter->bits < 4 )
     return -1;

    self->target_amp = 1 << (filter->bits - 3);

    if ( roardsp_filter_reset(self->amp, ROARDSP_RESET_FULL) == -1 )
     return -1;

    return -1;
   break;
  case ROARDSP_RESET_STATE:
    if ( roardsp_filter_reset(self->amp, ROARDSP_RESET_STATE) == -1 )
     return -1;
    return -1;
   break;
  default: return -1;
 }

 return 0;
}

//ll
