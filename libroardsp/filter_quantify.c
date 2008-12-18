//quantify.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - December 2008
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

int roardsp_quantify_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 int n = 64;

 roardsp_amp_ctl(filter, ROARDSP_FCTL_N, &n);

 ROAR_DBG("roardsp_quantify_init(*) = 0");
 return 0;
}

int roardsp_quantify_calc16  (struct roardsp_filter * filter, void * data, size_t samples) {
 int16_t * samp = (int16_t *) data;
 register float s;
 size_t i;

 for (i = 0; i < samples; i++) {
  s        = samp[i];
  s       /= 32768;
  s       *= (ROAR_INSTINT)filter->inst;
  s        = (int16_t)s;
//  ROAR_WARN("roardsp_quantify_calc16(*): s=%f", s);
  s       /= (ROAR_INSTINT)filter->inst;
  s       *= 32768;
  samp[i]  = s;
 };

 ROAR_DBG("roardsp_quantify_calc16(*) = 0");
 return 0;
}

int roardsp_quantify_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 int32_t old;

 if ( cmd == ROARDSP_FCTL_N ) {
  old = (ROAR_INSTINT)filter->inst;
  filter->inst = (void*)(ROAR_INSTINT)*(int32_t*)data;
  *(int32_t*)data = old;
 } else {
  ROAR_DBG("roardsp_quantify_ctl(*) = -1");
  return -1;
 }


 ROAR_DBG("roardsp_quantify_ctl(*) = 0");
 return 0;
}

//ll
