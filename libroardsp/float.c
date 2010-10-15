//float.c:

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

static inline float   _si32_to_float (int32_t src) {
 if ( src > 0 ) {
  return (float)src / 2147483647.f;
 } else {
  return (float)src / 2147483648.f;
 }
}

static inline int32_t _float_to_si32 (float   src) {
 if ( src > 0 ) {
  return (src * 2147483647.f);
 } else {
  return (src * 2147483648.f);
 }
}

int roar_conv_int32_float(float   * dst, const int32_t * src, size_t len) {
 size_t i;

 if ( len == 0 )
  return 0;

 if ( dst == NULL || src == NULL )
  return -1;

 for (i = 0; i < len; i++)
  dst[i] = _si32_to_float(src[i]);

 return 0;
}

int roar_conv_float_int32(int32_t * dst, const float   * src, size_t len) {
 size_t i;

 if ( len == 0 )
  return 0;

 if ( dst == NULL || src == NULL )
  return -1;

 for (i = 0; i < len; i++)
  dst[i] = _float_to_si32(src[i]);

 return 0;
}

int roar_conv_int32_float_deint(float   ** dst, const int32_t * src, size_t len, size_t channels) {
 register size_t i, c, o;

 ROAR_DBG("roar_conv_int32_float_deint(*) = ?");

 if ( len == 0 )
  return 0;

 ROAR_DBG("roar_conv_int32_float_deint(*) = ?");

 if ( dst == NULL || src == NULL )
  return -1;

 ROAR_DBG("roar_conv_int32_float_deint(*) = ?");

 // check if len is a multiple of channels:
 if ( (len % channels) != 0 )
  return -1;

 ROAR_DBG("roar_conv_int32_float_deint(*) = ?");

 for (c = 0; c < channels; c++)
  if ( dst[c] == NULL )
   return -1;

 ROAR_DBG("roar_conv_int32_float_deint(*) = ?");

 switch (channels) {
  case  1:
    return roar_conv_int32_float(dst[0], src, len);
   break;
  case  2:
    for (i = o = 0; i < len; i += 2, o += 1) {
     dst[0][o] = _si32_to_float(src[i+0]);
     dst[1][o] = _si32_to_float(src[i+1]);
    }
   break;
  default:
    for (i = c = o = 0; i < len; i++) {
     dst[c][o] = _si32_to_float(src[i]);
     c++;
     if ( c == channels ) {
      c = 0;
      o++;
     }
    }
   break;
 }

 return 0;
}

int roar_conv_float_int32_enint(int32_t  * dst, const float   ** src, size_t len, size_t channels) {
 size_t i, c, o;

 if ( len == 0 )
  return 0;

 if ( dst == NULL || src == NULL )
  return -1;

 // check if len is a multiple of channels:
 if ( (len % channels) != 0 )
  return -1;

 for (c = 0; c < channels; c++)
  if ( src[c] == NULL )
   return -1;

 switch (channels) {
  default:
    for (i = c = o = 0; o < len; o++) {
     dst[o] = _float_to_si32(src[c][i]);
     c++;
     if ( c == channels ) {
      c = 0;
      i++;
     }
    }
   break;
 }

 return 0;
}

//ll
