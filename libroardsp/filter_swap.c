//swap.c:

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

int roardsp_swap_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_swap * inst = malloc(sizeof(struct roardsp_swap));

 ROAR_WARN("roardsp_swap_init(*): inst=%p", inst);

 if ( inst == NULL ) {
  ROAR_ERR("roardsp_swap_init(*): Can not alloc memory for filter swap: %s", strerror(errno));
  filter->inst = NULL;
  return -1;
 }

 filter->inst = inst;

 return roardsp_swap_reset(filter, ROARDSP_RESET_FULL);
}

int roardsp_swap_uninit (struct roardsp_filter * filter) {
 if ( filter == NULL )
  return -1;

 if ( filter->inst == NULL )
  return -1;

 free(filter->inst);

 return 0;
}

int roardsp_swap_calc162(struct roardsp_filter * filter, void * data, size_t samples) {
 int16_t * d = data;
 register int16_t s;
 int i;

 for (i = 0; i < samples; i += 2) {
  ROAR_WARN("roardsp_swap_calc162(*): d[i]=%i, d[i+1]=%i", d[i], d[i+1]);
  s      = d[i];
  d[i]   = d[i+1];
  d[i+1] = s;
 }

 return 0;
}

int roardsp_swap_ctl    (struct roardsp_filter * filter, int cmd, void * data) {
 return -1;
}

int roardsp_swap_reset  (struct roardsp_filter * filter, int what) {
 struct roardsp_swap * inst = NULL;
 int i;

 if ( filter == NULL )
  return -1;

 if ( (inst = filter->inst) == NULL )
  return -1;

 switch (what) {
  case ROARDSP_RESET_NONE:
  case ROARDSP_RESET_STATE:
    return  0;
   break;
  case ROARDSP_RESET_FULL:
    memset(inst, 0, sizeof(struct roardsp_swap));

    for (i = 0; i < ROAR_MAX_CHANNELS; i++)
     inst->map[i] = i;

    inst->map[0] = 1;
    inst->map[1] = 0;
    return  0;
   break;
  default:
    return -1;
 }

 return -1;
}

//ll
