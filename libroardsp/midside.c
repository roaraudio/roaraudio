//midside.c:

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

int roar_conv_s2ms_8      (void * out, void * in, int samples) {
 char * ip = in;
 char * op = out;
 register int i;
 register char mid;
 register char side;

 for (i = 0; i < samples; i += 2) {
  mid     = (ip[i] + ip[i+1])/2;
  side    = (ip[i] - ip[i+1])/2;
  op[i  ] = mid;
  op[i+1] = side;
 }

 return 0;
}

int roar_conv_s2ms_16     (void * out, void * in, int samples) {
 int16_t * ip = in;
 int16_t * op = out;
 register int i;
 register int16_t mid;
 register int16_t side;

 for (i = 0; i < samples; i += 2) {
  mid     = (ip[i] + ip[i+1])/2;
  side    = (ip[i] - ip[i+1])/2;
  op[i  ] = mid;
  op[i+1] = side;
 }

 return 0;
}

int roar_conv_s2ms_32     (void * out, void * in, int samples) {
 int32_t * ip = in;
 int32_t * op = out;
 register int i;
 register int32_t mid;
 register int32_t side;

 for (i = 0; i < samples; i += 2) {
  mid     = (ip[i] + ip[i+1])/2;
  side    = (ip[i] - ip[i+1])/2;
  op[i  ] = mid;
  op[i+1] = side;
 }

 return 0;
}

int roar_conv_ms2s_8      (void * out, void * in, int samples) {
 char * ip = in;
 char * op = out;
 register int i;
 register char mid;
 register char side;

 for (i = 0; i < samples; i += 2) {
  mid     = ip[i];
  side    = ip[i+1];
  op[i  ] = mid + side;
  op[i+1] = mid - side;
 }

 return 0;
}

int roar_conv_ms2s_16     (void * out, void * in, int samples) {
 int16_t * ip = in;
 int16_t * op = out;
 register int i;
 register int16_t mid;
 register int16_t side;

 for (i = 0; i < samples; i += 2) {
  mid     = ip[i];
  side    = ip[i+1];
  op[i  ] = mid + side;
  op[i+1] = mid - side;
 }

 return 0;
}

int roar_conv_ms2s_32     (void * out, void * in, int samples) {
 int32_t * ip = in;
 int32_t * op = out;
 register int i;
 register int32_t mid;
 register int32_t side;

 for (i = 0; i < samples; i += 2) {
  mid     = ip[i];
  side    = ip[i+1];
  op[i  ] = mid + side;
  op[i+1] = mid - side;
 }

 return 0;
}


//ll
