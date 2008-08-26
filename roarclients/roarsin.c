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

int main (void) {
 int rate     = ROAR_RATE_DEFAULT;
 int bits     = 8;
 int channels = 1; /* mono */
 int codec    = ROAR_CODEC_DEFAULT;
 float freq   = 523.2;            /* middle C */
 float step   = M_PI*2*freq/rate; /* how much time per sample we have to encode ... */
 float t      = 0; /* current time */
 float length = 5; /* 5 sec */
 int fh;
 int i;
 char out[1024];

 if ( (fh = roar_simple_play(rate, channels, bits, codec, NULL, "sine gen")) == -1 ) {
  fprintf(stderr, "Error: can not open playback!\n");
  exit(1);
 }

 while (t < 2*M_PI*freq*length) {
  for (i = 0; i < 1024; i++) {
   out[i] = 127*sin(t);
   t += step;
  }
  write(fh, out, 1024);
 }

 roar_simple_close(fh);

 return 0;
}

//ll
