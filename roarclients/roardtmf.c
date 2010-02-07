//roardtmf.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#include <roaraudio.h>

#ifdef ROAR_HAVE_LIBM
#include <math.h>

// in ms:
#define ON_TIME  (180)
#define OFF_TIME ( 80)
#define SUM_TIME (ON_TIME+OFF_TIME)

struct tone {
 char c;
 float f0;
 float f1;
} g_tones[] = {
 {'1', 697, 1209},
 {'2', 697, 1336},
 {'3', 697, 1477},
 {'A', 697, 1633},

 {'4', 770, 1209},
 {'5', 770, 1336},
 {'6', 770, 1477},
 {'B', 770, 1633},

 {'7', 852, 1209},
 {'8', 852, 1336},
 {'9', 852, 1477},
 {'C', 852, 1633},

 {'*', 941, 1209},
 {'0', 941, 1336},
 {'#', 941, 1477},
 {'D', 941, 1633},

 {0, -1, -1}
};

void usage (void) {
 printf("roarcatvio [OPTIONS]... [FILE]\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --codec  CODEC     - Set the codec\n"
        "  --help             - Show this help\n"
       );

}

int calc (int16_t * samples, size_t len, int rate, char c) {
 struct tone * ct = NULL;
 int i;
 float t;
 float t_inc = 1./rate;
 float t_max = ON_TIME / 1000.;
 float fc0, fc1;

// printf("calc(*): t_inc=%f, t_max=%f\n", t_inc, t_max);

 if ( c >= 'a' )
  c -= 'a' - 'A';

 for (i = 0; g_tones[i].c != 0; i++) {
  if ( g_tones[i].c == c ) {
   ct = &(g_tones[i]);
   break;
  }
 }

// printf("calc(*): ct=%p\n", ct);

 if ( ct == NULL )
  return -1;

 fc0 = 2 * M_PI * ct->f0;
 fc1 = 2 * M_PI * ct->f1;

// printf("fc0=%f, fc1=%f\n", fc0, fc1);

 memset(samples, 0, len);

 for (i = 0, t = 0; t < t_max; t += t_inc, i++) {
//  printf("i=%i, t=%f\n", i, t);
  samples[i] = (sinf(fc0*t) + sinf(fc1*t))*8192.0;
 }

 return 0;
}

int main (int argc, char * argv[]) {
 int    rate     = ROAR_RATE_DEFAULT;
 int    bits     = ROAR_BITS_DEFAULT;
 int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 char * k;
 int    i;
 char * name = "roardtmf";
 struct roar_vio_calls stream;
 char * tones = NULL;
 void * buf;
 size_t samples;
 size_t len;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( !strcmp(k, "--server") || !strcmp(k, "-s") ) {
   server = argv[++i];
  } else if ( !strcmp(k, "--rate") || !strcmp(k, "-r") ) {
   rate = atoi(argv[++i]);
  } else if ( !strcmp(k, "--bits") ) {
   bits = atoi(argv[++i]);
  } else if ( !strcmp(k, "--codec") ) {
   codec = roar_str2codec(argv[++i]);
  } else if ( !strcmp(k, "--help") ) {
   usage();
   return 0;
  } else if ( tones == NULL ) {
   tones = argv[i++];
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( tones == NULL ) {
  return 1;
 }

 if ( codec != ROAR_CODEC_DEFAULT )
  return 2;
 if ( bits  != 16 )
  return 3;

 samples = SUM_TIME * rate / 1000;

/*
 printf("samples=%llu\n", (long long unsigned int)samples);
 return 0;
*/

 if ( (buf = malloc((len = bits*samples/8))) == NULL )
  return 4;

 if ( roar_vio_simple_stream(&stream, rate, 1, bits, codec, server, ROAR_DIR_PLAY, name) == -1 ) {
  fprintf(stderr, "Error: can not start playback\n");
  return 1;
 }

 while (*tones != 0 ) {
  if ( calc(buf, len, rate, *tones) == -1 )
   return 5;
  roar_vio_write(&stream, buf, len);
  tones++;
 }

 roar_vio_close(&stream);

 return 0;
}

#else
int main (void) {
 fprintf(stderr, "Error: No Math library support compiled in.\n");
 return 1;
}
#endif

//ll
