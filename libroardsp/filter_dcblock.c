//dcblock.c:

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

int roardsp_dcblock_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_dcblock * inst = roar_mm_malloc(sizeof(struct roardsp_dcblock));
// roardsp_downmix_ctl(filter, ROARDSP_FCTL_MODE, &mode);

 if ( inst == NULL ) {
  ROAR_ERR("roardsp_dcblock_init(*): Can not alloc memory for filter dcblock: %s", strerror(errno));
  filter->inst = NULL;
  return -1;
 }

 memset(inst, 0, sizeof(struct roardsp_dcblock));

 filter->inst = inst;

 return 0;
}

int roardsp_dcblock_uninit (struct roardsp_filter * filter) {
 if ( filter == NULL )
  return -1;

 if ( filter->inst == NULL )
  return -1;

 roar_mm_free(filter->inst);

 return 0;
}

int roardsp_dcblock_reset  (struct roardsp_filter * filter, int what) {
 if ( filter == NULL )
  return -1;

 if ( filter->inst == NULL )
  return -1;

 ROAR_DBG("roardsp_dcblock_reset(filter=%p, what=%i) = ?", filter, what);

 switch (what) {
  case ROARDSP_RESET_NONE:
    return  0;
   break;
  case ROARDSP_RESET_FULL:
    memset(filter->inst, 0, sizeof(struct roardsp_dcblock));
    return  0;
   break;
  case ROARDSP_RESET_STATE:
    memset(((struct roardsp_dcblock*)filter->inst)->dc, 0, sizeof(int32_t)*ROARDSP_DCBLOCK_NUMBLOCKS);
    ((struct roardsp_dcblock*)filter->inst)->cur = 0;
    return  0;
   break;
  default:
    return -1;
 }

 return -1;
}

int roardsp_dcblock_calc16  (struct roardsp_filter * filter, void * data, size_t samples) {
 struct roardsp_dcblock * inst = filter->inst;
 int16_t * samp = (int16_t *) data;
 register int64_t s = 0;
 size_t i;

 for (i = 0; i < samples; i++) {
//  ROAR_WARN("roardsp_dcblock_calc16(*): s=%i, samp[i=%i]=%i", s, i, samp[i]);
  s += samp[i];
 }

 ROAR_DBG("roardsp_dcblock_calc16(*): s=%i (current block of %i samples)", s, samples);

 s = (float)(s / samples);

 ROAR_DBG("roardsp_dcblock_calc16(*): inst=%p, inst->cur=%i", inst, inst->cur);
 inst->dc[(inst->cur)++] = s;

 if ( inst->cur == ROARDSP_DCBLOCK_NUMBLOCKS )
  inst->cur = 0;

 s = 0;

 for (i = 0; i < ROARDSP_DCBLOCK_NUMBLOCKS; i++)
  s += inst->dc[i];

 s /= ROARDSP_DCBLOCK_NUMBLOCKS;

// s += (ROAR_INSTINT)filter->inst;

 ROAR_DBG("roardsp_dcblock_calc16(*): new DC value: %i", s);

 for (i = 0; i < samples; i++)
  samp[i] -= s;

 ROAR_DBG("roardsp_downmix_calc16(*) = 0");
 return 0;
}



//ll
