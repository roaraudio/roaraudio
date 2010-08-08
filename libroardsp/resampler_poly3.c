//resampler_poly3.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *      Copyright (C) Hans-Kristian 'maister' Arntzen - 2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroardsp.h"

int roar_conv_poly3_8 (int8_t * out, int8_t * in, size_t olen, size_t ilen, int channels) {
 float ratio = (float)olen / (float)ilen;
 int8_t *ip;
 int c, x;
 float pos_in;
 float poly[3];
 float y[3];
 float x_val;
 int_least16_t temp;

 /* Can't create poly out of less than 3 samples in each channel. */
 if ( ilen < 3 * channels )
  return -1;

 ip = roar_mm_malloc(ilen * sizeof(int8_t));
 if ( ip == NULL )
  return -1;

 memcpy(ip, in, ilen * sizeof(int8_t));

 olen /= channels;

 for (x = 0; x < olen; x++) {
  for (c = 0; c < channels; c++) {
   pos_in = (float)x / ratio;

   if ( (int)pos_in == 0 ) {
    y[0] = ip[0 * channels + c];
    y[1] = ip[1 * channels + c];
    y[2] = ip[2 * channels + c];
    x_val = pos_in;
    roar_math_mkpoly_3x3(poly, y);
   } else if ( (int)pos_in + 1 >= ilen/channels ) {
    /* If we're at the end of the block, we will need to interpolate against a value that is not yet known.
     * We will assume this value, by linearly extrapolating the two preceding values. From causual testing, this is not audible. */
    y[0] = ip[((int)pos_in - 1) * channels + c];
    y[1] = ip[((int)pos_in    ) * channels + c];

    // we create a 2x2 poly here and set the 3rd coefficient to zero to build a 3x3 poly
    roar_math_mkpoly_2x2(poly, y);
    poly[2] = 0;
    x_val = pos_in - (int)pos_in + 1.0;
   } else {
    y[0] = ip[((int)pos_in - 1) * channels + c];
    y[1] = ip[((int)pos_in    ) * channels + c];
    y[2] = ip[((int)pos_in + 1) * channels + c];
    x_val = pos_in - (int)pos_in + 1.0;
    roar_math_mkpoly_3x3(poly, y);
   }


   temp = (float)(poly[2]*x_val*x_val + poly[1]*x_val + poly[0] + 0.5);
   /* temp could be out of bounds, so need to check this */
   if ( temp > 0x7E ) {
    out[x * channels + c] =  0x7E;
   } else if (temp < -0x7F) {
    out[x * channels + c] = -0x7F;
   } else {
    out[x * channels + c] = (int8_t)temp;
   }
  }
 }

 roar_mm_free(ip);
 return 0;
}

int roar_conv_poly3_16 (int16_t * out, int16_t * in, size_t olen, size_t ilen, int channels) {
 float ratio = (float)olen / (float)ilen;
 int16_t *ip;
 int c, x;
 float pos_in;
 float poly[3];
 float y[3];
 float x_val;
 int_least32_t temp;

 /* Can't create poly out of less than 3 samples in each channel. */
 if ( ilen < 3 * channels )
  return -1;

 ip = roar_mm_malloc(ilen * sizeof(int16_t));
 if ( ip == NULL )
  return -1;

 memcpy(ip, in, ilen * sizeof(int16_t));

 olen /= channels;

 for (x = 0; x < olen; x++) {
  for (c = 0; c < channels; c++) {
   pos_in = (float)x / ratio;

   if ( (int)pos_in == 0 ) {
    y[0] = ip[0 * channels + c];
    y[1] = ip[1 * channels + c];
    y[2] = ip[2 * channels + c];
    x_val = pos_in;
    roar_math_mkpoly_3x3(poly, y);
   } else if ( (int)pos_in + 1 >= ilen/channels ) {
    /* If we're at the end of the block, we will need to interpolate against a value that is not yet known.
     * We will assume this value, by linearly extrapolating the two preceding values. From causual testing, this is not audible. */
    y[0] = ip[((int)pos_in - 1) * channels + c];
    y[1] = ip[((int)pos_in    ) * channels + c];

    // we create a 2x2 poly here and set the 3rd coefficient to zero to build a 3x3 poly
    roar_math_mkpoly_2x2(poly, y);
    poly[2] = 0;
    x_val = pos_in - (int)pos_in + 1.0;
   } else {
    y[0] = ip[((int)pos_in - 1) * channels + c];
    y[1] = ip[((int)pos_in    ) * channels + c];
    y[2] = ip[((int)pos_in + 1) * channels + c];
    x_val = pos_in - (int)pos_in + 1.0;
    roar_math_mkpoly_3x3(poly, y);
   }


   temp = (float)(poly[2]*x_val*x_val + poly[1]*x_val + poly[0] + 0.5);
   /* temp could be out of bounds, so need to check this */
   if (temp > 0x7FFE ) {
    out[x * channels + c] =  0x7FFE;
   } else if (temp < -0x7FFF) {
    out[x * channels + c] = -0x7FFF;
   } else {
    out[x * channels + c] = (int16_t)temp;
   }
  }
 }

 roar_mm_free(ip);
 return 0;
}

int roar_conv_poly3_32 (int32_t * out, int32_t * in, size_t olen, size_t ilen, int channels) {
 float ratio = (float)olen / (float)ilen;
 int32_t *ip;
 int c, x;
 float pos_in;
 float poly[3];
 float y[3];
 float x_val;
 int_least64_t temp;

 /* Can't create poly out of less than 3 samples in each channel. */
 if ( ilen < 3 * channels )
  return -1;

 ip = roar_mm_malloc(ilen * sizeof(int32_t));
 if ( ip == NULL )
  return -1;

 memcpy(ip, in, ilen * sizeof(int32_t));

 olen /= channels;

 for (x = 0; x < olen; x++) {
  for (c = 0; c < channels; c++) {
   pos_in = (float)x / ratio;

   if ( (int)pos_in == 0 ) {
    y[0] = ip[0 * channels + c];
    y[1] = ip[1 * channels + c];
    y[2] = ip[2 * channels + c];
    x_val = pos_in;
    roar_math_mkpoly_3x3(poly, y);
   } else if ( (int)pos_in + 1 >= ilen/channels ) {
    /* If we're at the end of the block, we will need to interpolate against a value that is not yet known.
     * We will assume this value, by linearly extrapolating the two preceding values. From causual testing, this is not audible. */
    y[0] = ip[((int)pos_in - 1) * channels + c];
    y[1] = ip[((int)pos_in    ) * channels + c];

    // we create a 2x2 poly here and set the 3rd coefficient to zero to build a 3x3 poly
    roar_math_mkpoly_2x2(poly, y);
    poly[2] = 0;
    x_val = pos_in - (int)pos_in + 1.0;
   } else {
    y[0] = ip[((int)pos_in - 1) * channels + c];
    y[1] = ip[((int)pos_in    ) * channels + c];
    y[2] = ip[((int)pos_in + 1) * channels + c];
    x_val = pos_in - (int)pos_in + 1.0;
    roar_math_mkpoly_3x3(poly, y);
   }


   temp = (float)(poly[2]*x_val*x_val + poly[1]*x_val + poly[0] + 0.5);
   /* temp could be out of bounds, so need to check this */
   if ( temp > 0x7FFFFFFE ) {
    out[x * channels + c] =  0x7FFFFFFE;
   } else if (temp < -0x7FFFFFFF) {
    out[x * channels + c] = -0x7FFFFFFF;
   } else {
    out[x * channels + c] = (int32_t)temp;
   }
  }
 }

 roar_mm_free(ip);
 return 0;
}

