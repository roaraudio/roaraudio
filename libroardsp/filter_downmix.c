//downmix.c:

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

int roardsp_downmix_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 int mode = ROARDSP_DOWNMIX_ARITHMETIC;

 roardsp_downmix_ctl(filter, ROARDSP_FCTL_MODE, &mode);

 ROAR_DBG("roardsp_downmix_init(*) = 0");
 return 0;
}

int roardsp_downmix_calc162  (struct roardsp_filter * filter, void * data, size_t samples) {
 int16_t * samp = (int16_t *) data;
 register int32_t mode = (ROAR_INSTINT)filter->inst;
 register union {
  int32_t i32;
  int64_t i64;
 } s;
 size_t i;

 ROAR_DBG("roardsp_downmix_calc162(*): mode=%i", mode);

 switch (mode) {
  case ROARDSP_DOWNMIX_ARITHMETIC:
    for (i = 0; i < samples; i += 2) {
     s.i32     = (samp[i] + samp[i+1])/2;
     samp[i  ] = s.i32;
     samp[i+1] = s.i32;
    }
   break;
  case ROARDSP_DOWNMIX_RMS:
    for (i = 0; i < samples; i += 2) {
     s.i64      = (int64_t)samp[i]*samp[i] + (int64_t)samp[i+1]*samp[i+1];
     s.i64      = sqrt(s.i64);
     s.i64     /=  2;

     if ( (samp[i] + samp[i+1]) < 0 )
      s.i64    *= -1;

     samp[i  ]  = s.i64;
     samp[i+1]  = s.i64;
    }
   break;
  default:
   ROAR_DBG("roardsp_downmix_calc162(*) = -1 // unknown mode");
   return -1;
  break;
 }

 ROAR_DBG("roardsp_downmix_calc162(*) = 0");
 return 0;
}

int roardsp_downmix_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 int32_t old;

 if ( cmd == ROARDSP_FCTL_MODE ) {
  old = (ROAR_INSTINT)filter->inst;
  filter->inst = (void*)(ROAR_INSTINT)*(int32_t*)data;
  *(int32_t*)data = old;
 } else {
  return -1;
 }


 return 0;
}

//ll
