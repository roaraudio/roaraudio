//mixer.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

int mix_clients (void * output, int bits, void ** input, int samples) {
 if ( bits == 8 ) {
  return mix_clients_8bit(output, input, samples);
 } else if ( bits == 16 ) {
  return mix_clients_16bit(output, input, samples);
 } else if ( bits == 24 ) {
  return mix_clients_24bit(output, input, samples);
 } else if ( bits == 32 ) {
  return mix_clients_32bit(output, input, samples);
 } else {
  return -1;
 }
}

int mix_clients_8bit (void * output, void ** input, int samples) {
 int i, s;
 register int c;

 for (s = 0; s < samples; s++) {
  c = 0;

  for (i = 0; input[i] != NULL; i++)
   c += ((char**)input)[i][s];

  if ( c > 127 )
   c = 127;
  else if ( c < -128 )
   c = -128;
  ((char*)output)[s] = (char)c;
 }

 return 0;
}

int mix_clients_16bit (void * output, void ** input, int samples) {
 int i, s;
 register int c;
 int16_t ** in  = (int16_t**) input;
 int16_t *  out = output;

#ifdef DEBUG
 for (i = 0; input[i] != NULL; i++)
  ROAR_DBG("mix_clients_16bit(*): input[%i] = %p", i, input[i]);
#endif

/*
 if ( input[0] != NULL )
  write(1, input[0], samples*2);
*/

/*
 if ( input[0] != NULL ) {
  memcpy(output, input[0], samples*2);
  return -1;
 }
*/

 for (s = 0; s < samples; s++) {
  c = 0;
  for (i = 0; input[i] != NULL; i++) {
//   printf("D: input[i=%i] = %p\n", i, input[i]);
   c += in[i][s];
  }

  if ( c > 32767 )
   c = 32767;
  else if ( c < -32768 )
   c = -32768;

  out[s] = c;
 }

 return 0;
}

int mix_clients_24bit (void * output, void ** input, int samples) {
 return -1;
}

int mix_clients_32bit (void * output, void ** input, int samples) {
#ifdef ROAR_NATIVE_INT64
 int32_t ** in  = (int32_t**) input;
 int32_t *  out = (int32_t* ) output;
 int i, s;
 ROAR_NATIVE_INT64 c;

 for (s = 0; s < samples; s++) {
  c = 0;

  for (i = 0; input[i]; i++)
   c += in[i][s];

  if ( c > 21474836487LL )
   c = 2147483647LL;
  else if ( c < -2147483648LL )
   c = -2147483648LL;
  out[s] = (int32_t)c;
 }

 return  0;
#else
 return -1;
#endif
}

int change_vol (void * output, int bits, void * input, int samples, int channels, struct roar_mixer_settings * set) {

 ROAR_DBG("change_vol(*): mixer at %p", set->mixer);

 if ( bits == 8 ) {
  return  change_vol_8bit(output, input, samples, channels, set);
 } else if ( bits == 16 ) {
  return  change_vol_16bit(output, input, samples, channels, set);
 } else if ( bits == 24 ) {
  return  change_vol_24bit(output, input, samples, channels, set);
 } else if ( bits == 32 ) {
  return  change_vol_32bit(output, input, samples, channels, set);
 } else {
  return -1;
 }
}

int change_vol_8bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 char * in = input, * out = output;
 int    i;
 register int s;

 if ( !(in && out) )
  return -1;

 if (set->rpg_mul == set->rpg_div) {
  for (i = 0; i < samples; i++) {
   s  = in[i];
   s *= set->mixer[i % channels];
   s /= set->scale;
   out[i] = s;
  }
 } else {
  for (i = 0; i < samples; i++) {
   s  = in[i];
   s *= (set->mixer[i % channels] * set->rpg_mul) / set->rpg_div;
   s /= set->scale;
   out[i] = s;
  }
 }

 return 0;
}

int change_vol_16bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 int16_t * in = input, * out = output;
 int       i;
 register int s;

 if ( !(in && out) )
  return -1;

 if (set->rpg_mul == set->rpg_div) {
  for (i = 0; i < samples; i++) {
   s  = in[i];
   s *= set->mixer[i % channels];
   s /= set->scale;
   out[i] = s;
  }
 } else {
  for (i = 0; i < samples; i++) {
   s  = in[i];
   s *= (set->mixer[i % channels] * set->rpg_mul) / set->rpg_div;
   s /= set->scale;
   out[i] = s;
  }
 }

 return 0;
}

int change_vol_24bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
 return -1;
}

int change_vol_32bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set) {
#ifdef ROAR_NATIVE_INT64
 int32_t * in = input, * out = output;
 int       i;
 ROAR_NATIVE_INT64 s;

 if ( !(in && out) )
  return -1;

 if (set->rpg_mul == set->rpg_div) {
  for (i = 0; i < samples; i++) {
   s  = in[i];
   s *= set->mixer[i % channels];
   s /= set->scale;
   out[i] = s;
  }
 } else {
  for (i = 0; i < samples; i++) {
   s  = in[i];
   s *= (set->mixer[i % channels] * set->rpg_mul) / set->rpg_div;
   s /= set->scale;
   out[i] = s;
  }
 }

 return 0;
#else
 return -1;
#endif
}


//ll
