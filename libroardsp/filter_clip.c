//clip.c:

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

int roardsp_clip_calc16  (struct roardsp_filter * filter, void * data, size_t samples) {
 int16_t * samp = (int16_t *) data;
 register int32_t s = (ROAR_INSTINT)filter->inst;
 size_t i;

 for (i = 0; i < samples; i++) {
  if ( samp[i] > s ) {
   samp[i]  = s;
  } else if ( -samp[i] > s ) {
   samp[i]  = -s;
  }
 }

 ROAR_DBG("roardsp_quantify_calc16(*) = 0");
 return 0;
}

int roardsp_clip_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 int32_t old;

 if ( cmd == ROARDSP_FCTL_LIMIT ) {
  old = (ROAR_INSTINT)filter->inst;
  filter->inst = (void*)(ROAR_INSTINT)labs(*(int32_t*)data);
  *(int32_t*)data = old;
 } else {
  ROAR_DBG("roardsp_clip_ctl(*) = -1");
  return -1;
 }


 ROAR_DBG("roardsp_clip_ctl(*) = 0");
 return 0;
}

int roardsp_clip_reset (struct roardsp_filter * filter, int what) {
 int32_t n = 16384;

 if ( filter == NULL )
  return -1;

 switch (what) {
  case ROARDSP_RESET_NONE:
  case ROARDSP_RESET_STATE:
    return  0;
   break;
  case ROARDSP_RESET_FULL:
    roardsp_clip_ctl(filter, ROARDSP_FCTL_LIMIT, &n);
    return  0;
   break;
  default:
    return -1;
 }

 return -1;
}

//ll
