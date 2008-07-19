//roarsin.c:

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
