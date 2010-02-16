//mixer.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008, 2009
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

int roar_mix_pcm       (void    * output, int bits, void ** input, int samples) {
 switch (bits) {
  case  8: return roar_mix_pcm_8bit (output, (int8_t  **)input, samples); break;
  case 16: return roar_mix_pcm_16bit(output, (int16_t **)input, samples); break;
  case 24: return roar_mix_pcm_24bit(output, input, samples); break;
  case 32: return roar_mix_pcm_32bit(output, (int32_t **)input, samples); break;
  default: return -1;
 }
}

int roar_mix_pcm_8bit  (int8_t  * output, int8_t  ** input, int samples) {
 int i, s;
 register int c;

 for (s = 0; s < samples; s++) {
  c = 0;

  for (i = 0; input[i] != NULL; i++)
   c += input[i][s];

  if ( c > 127 )
   c = 127;
  else if ( c < -128 )
   c = -128;
  output[s] = (char)c;
 }

 return 0;
}

int roar_mix_pcm_16bit (int16_t * output, int16_t ** input, int samples) {
 int i, s;
 register int_least32_t c;

#ifdef DEBUG
 for (i = 0; input[i] != NULL; i++)
  ROAR_DBG("mix_clients_16bit(*): input[%i] = %p", i, input[i]);
#endif

 for (s = 0; s < samples; s++) {
  c = 0;
  for (i = 0; input[i] != NULL; i++) {
   c += input[i][s];
  }

  if ( c > 32767 )
   c = 32767;
  else if ( c < -32768 )
   c = -32768;

  output[s] = c;
 }

 return 0;
}

int roar_mix_pcm_24bit (void    * output, void    ** input, int samples) {
 return -1;
}

int roar_mix_pcm_32bit (int32_t * output, int32_t ** input, int samples) {
#ifdef ROAR_NATIVE_INT64
 int i, s;
 ROAR_NATIVE_INT64 c;

 for (s = 0; s < samples; s++) {
  c = 0;

  for (i = 0; input[i]; i++)
   c += input[i][s];

  if ( c > 21474836487LL )
   c = 2147483647LL;
  else if ( c < -2147483648LL )
   c = -2147483648LL;
  output[s] = (int32_t)c;
 }

 return  0;
#else
 return -1;
#endif
}


//ll
