//math.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

int roar_math_mkpoly (float * poly, float * data, int len) {
 if ( len == 2 )
  return roar_math_mkpoly_2x2(poly, data);
 if ( len == 3 )
  return roar_math_mkpoly_3x3(poly, data);
 if ( len == 4 )
  return roar_math_mkpoly_4x4(poly, data);
 if ( len == 5 )
  return roar_math_mkpoly_5x5(poly, data);

 return -1;
}

int roar_math_mkpoly_2x2 (float * poly, float * data) {
/*
 A B
 0 1 x
 1 1 y

B = x
A = y - x
*/

 poly[0] =           data[0];
 poly[1] = data[1] - data[0];

 return 0;
}

int roar_math_mkpoly_3x3 (float * poly, float * data) {
/*
 A B C
 0 0 1 x
 1 1 1 y
 4 2 1 z
*/

 poly[0] = data[0];
 poly[1] = 2*data[1] - 2*data[0] - (data[2]-data[0])/2;
 poly[2] = (data[2]-data[0])/2 - data[1] + data[0];


 return 0;
}

int roar_math_mkpoly_4x4 (float * poly, float * data) {
/*
   a    b    c    d
   0    0    0    1  A X 0
   1    1    1    1  B Y 1
   8    4    2    1  C Z 2
  27    9    3    1  D Q 3
*/


// { a = -(3z-3y+x-q)/6, b = (4z-5y+2x-q)/2, c = -(9z-18y+11x-2q)/6, d = x }.

 poly[0] =      data[0];
 poly[1] = -(11*data[0] - 18*data[1] + 9*data[2] - 2*data[3])/6;
 poly[2] =  (2 *data[0] -  5*data[1] + 4*data[2] -   data[3])/2;
 poly[3] = -(   data[0] -  3*data[1] + 3*data[2] -   data[3])/6;

 return 0;
}

int roar_math_mkpoly_5x5 (float * poly, float * data) {
 return -1;
}


float roar_math_cvpoly     (float * poly, float t, int len) {
 float ret = 0;
 float ct  = 1;
 int i;

 if ( poly == NULL )
  return 0;

 switch (len) {
  case 4: return roar_math_cvpoly_4x4(poly, t);
 }

 for (i = 0; i < len; i++) {
  ret += poly[i] * ct;
  ct  *= t;
 }

 return ret;
}

float roar_math_cvpoly_4x4 (float * poly, float t) {
 float ret = poly[0];
 float ct  = t;

 ret += poly[1] * ct;
 ct  *= t;
 ret += poly[2] * ct;
 ct  *= t;
 ret += poly[3] * ct;

// printf("ret=%f\n", ret);

 return ret;
}

//ll
