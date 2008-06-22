//math.c:

#include "libroar.h"

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

//ll
