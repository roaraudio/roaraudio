//rms.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

int64_t roar_rms2_1_8  (int8_t  * data, size_t samples) {
 register int64_t s = 0;
 register size_t  i;

 for (i = 0; i < samples; i++)
  s += data[i] * data[i];

 s /= samples;

 return s;
}

int64_t roar_rms2_1_16 (int16_t * data, size_t samples) {
 register int64_t s = 0;
 register size_t  i;

 for (i = 0; i < samples; i++)
  s += data[i] * data[i];

 s /= samples;

 return s;
}

int64_t roar_rms2_1_32 (int32_t * data, size_t samples) {
 register int64_t s = 0;
 register size_t  i;

 for (i = 0; i < samples; i++)
  s += data[i] * data[i];

 s /= samples;

 return s;
}


int roar_rms2_1_8_2    (int8_t  * data, size_t samples, int64_t * rms) {
 register int64_t a0 = 0, a1 = 0;
 register size_t  i;

 if ( samples == 0 )
  return 0;

 if ( data == NULL )
  return -1;

 if ( samples & 0x1 ) /* odd */
  return -1;

 for (i = 0; i < samples; i += 2) {
  a0 += data[i+0] * data[i+0];
  a1 += data[i+1] * data[i+1];
 }

 samples /= 2;

 a0 /= samples;
 a1 /= samples;

 rms[0] = a0;
 rms[1] = a1;

 return 0;
}

int roar_rms2_1_16_2   (int16_t * data, size_t samples, int64_t * rms) {
 register int64_t a0 = 0, a1 = 0;
 register size_t  i;

 if ( samples == 0 )
  return 0;

 if ( data == NULL )
  return -1;

 if ( samples & 0x1 ) /* odd */
  return -1;

 for (i = 0; i < samples; i += 2) {
  a0 += data[i+0] * data[i+0];
  a1 += data[i+1] * data[i+1];
 }

 samples /= 2;

 a0 /= samples;
 a1 /= samples;

 rms[0] = a0;
 rms[1] = a1;

 return 0;
}

int roar_rms2_1_32_2   (int32_t * data, size_t samples, int64_t * rms) {
 register int64_t a0 = 0, a1 = 0;
 register size_t  i;

 if ( samples == 0 )
  return 0;

 if ( data == NULL )
  return -1;

 if ( samples & 0x1 ) /* odd */
  return -1;

 for (i = 0; i < samples; i += 2) {
  a0 += data[i+0] * data[i+0];
  a1 += data[i+1] * data[i+1];
 }

 samples /= 2;

 a0 /= samples;
 a1 /= samples;

 rms[0] = a0;
 rms[1] = a1;

 return 0;
}


int roar_rms2_1_8_n    (int8_t  * data, size_t samples, int64_t * rms, size_t n) {
 if ( n == 0 )
  return 0;

 if ( rms == NULL )
  return -1;

 switch (n) {
  case 1: return *rms = roar_rms2_1_8(data, samples); return *rms == -1 ? -1 : 0; break;
  default:
   return -1;
 }
}

int roar_rms2_1_16_n   (int16_t * data, size_t samples, int64_t * rms, size_t n) {
 if ( n == 0 )
  return 0;

 if ( rms == NULL )
  return -1;

 switch (n) {
  case 1: return *rms = roar_rms2_1_16(data, samples); return *rms == -1 ? -1 : 0; break;
  default:
   return -1;
 }
}

int roar_rms2_1_32_n   (int32_t * data, size_t samples, int64_t * rms, size_t n) {
 if ( n == 0 )
  return 0;

 if ( rms == NULL )
  return -1;

 switch (n) {
  case 1: return *rms = roar_rms2_1_32(data, samples); return *rms == -1 ? -1 : 0; break;
  default:
   return -1;
 }
}

//ll
