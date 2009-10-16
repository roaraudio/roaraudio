//amp.c:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroardsp.h"

int roar_amp_pcm(void * output, int bits, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 switch (bits) {
  case  8: return roar_amp_pcm_8bit (output, input, samples, channels, set); break;
  case 16: return roar_amp_pcm_16bit(output, input, samples, channels, set); break;
  case 24: return roar_amp_pcm_24bit(output, input, samples, channels, set); break;
  case 32: return roar_amp_pcm_32bit(output, input, samples, channels, set); break;
  default: return -1;
 }
}

int roar_amp_pcm_8bit  (int8_t  * output, int8_t  * input, int samples, int channels, struct roar_mixer_settings * set) {
 int    i;
 register int s;

 if ( !(input && output) )
  return -1;

 if (set->rpg_mul == set->rpg_div) {
  for (i = 0; i < samples; i++) {
   s  = input[i];
   s *= set->mixer[i % channels];
   s /= set->scale;
   output[i] = s;
  }
 } else {
  for (i = 0; i < samples; i++) {
   s  = input[i];
   s *= (set->mixer[i % channels] * set->rpg_mul) / set->rpg_div;
   s /= set->scale;
   output[i] = s;
  }
 }

 return 0;
}

int roar_amp_pcm_16bit (int16_t * output, int16_t * input, int samples, int channels, struct roar_mixer_settings * set) {
 int       i;
 register int s;

 if ( !(input && output) )
  return -1;

 if (set->rpg_mul == set->rpg_div) {
  for (i = 0; i < samples; i++) {
   s  = input[i];
   s *= set->mixer[i % channels];
   s /= set->scale;
   output[i] = s;
  }
 } else {
  for (i = 0; i < samples; i++) {
   s  = input[i];
   s *= (set->mixer[i % channels] * set->rpg_mul) / set->rpg_div;
   s /= set->scale;
   output[i] = s;
  }
 }

 return 0;
}

int roar_amp_pcm_24bit (void    * output, void    * input, int samples, int channels, struct roar_mixer_settings * set) {
 return -1;
}

int roar_amp_pcm_32bit (int32_t * output, int32_t * input, int samples, int channels, struct roar_mixer_settings * set) {
#ifdef ROAR_NATIVE_INT64
 int       i;
 ROAR_NATIVE_INT64 s;

 if ( !(input && output) )
  return -1;

 if (set->rpg_mul == set->rpg_div) {
  for (i = 0; i < samples; i++) {
   s  = input[i];
   s *= set->mixer[i % channels];
   s /= set->scale;
   output[i] = s;
  }
 } else {
  for (i = 0; i < samples; i++) {
   s  = input[i];
   s *= (set->mixer[i % channels] * set->rpg_mul) / set->rpg_div;
   s /= set->scale;
   output[i] = s;
  }
 }

 return 0;
#else
 return -1;
#endif
}

//ll
