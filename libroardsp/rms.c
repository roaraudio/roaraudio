//rms.c:

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


//ll