//roarsin.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <math.h>       /* sin() */
#include <stdio.h>      /* *printf*() */
#include <roaraudio.h>  /* libroar */

double rect (double x) {
 x /= 2*M_PI;
 x -= (int)x;

 if ( x < 0.5 )
  return  1;
 else
  return -1;
}

double saw (double x) {
 x /= 2*M_PI;
 x -= (int)x;

 return 2*x - 1;
}

double tri (double x) {
 x /= 2*M_PI;
 x -= (int)x;

 if ( x < 0.5 )
  return   4* x      - 1;
 else
  return  -4*(x-0.5) + 1;
}

int main (int argc, char * argv[]) {
 int rate     = ROAR_RATE_DEFAULT;
 int bits     = 16;
 int channels = 1; /* mono */
 int codec    = ROAR_CODEC_DEFAULT;
 float freq   = 523.2;            /* middle C */
 float t      = 0; /* current time */
 float length = 5; /* 5 sec */
 float step;       /* how much time per sample we have to encode ... */
 int fh;
 int i;
 int16_t out[1024];
 double (*func)(double x) = sin;

 for (i = 1; i < argc; i++) {
  if ( !strcmp(argv[i], "--freq") ) {
   freq   = atof(argv[++i]);
  } else if ( !strcmp(argv[i], "--time") ) {
   length = atof(argv[++i]);
  } else if ( !strcmp(argv[i], "--sin") ) {
   func   = sin;
  } else if ( !strcmp(argv[i], "--saw") ) {
   func   = saw;
  } else if ( !strcmp(argv[i], "--tri") ) {
   func   = tri;
  } else {
   return 2;
  }
 }

 step   = M_PI*2*freq/rate;

 if ( (fh = roar_simple_play(rate, channels, bits, codec, NULL, "sine gen")) == -1 ) {
  fprintf(stderr, "Error: can not open playback!\n");
  exit(1);
 }

 while (t < 2*M_PI*freq*length) {
  for (i = 0; i < 1024; i++) {
   out[i] = 32767*func(t);
   t += step;
  }
  write(fh, out, 2048);
 }

 roar_simple_close(fh);

 return 0;
}

//ll
