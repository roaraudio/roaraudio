//roarvumeter.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008, 2009
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

#define BUFSIZE 1024

#define MODE_PC    1
#define MODE_DB    2

void usage (void) {
 printf("roarvumeter [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server  SERVER   - Set server hostname\n"
        "  --rate    RATE     - Set sample rate\n"
        "  --bits    BITS     - Set bits per sample\n"
        "  --chans   CHANNELS - Set number of channels\n"
        "  --samples SAMPLES  - Set number of samples\n"
        "  --help             - Show this help\n"
       );

}

int vumeter16bit2ch (struct roar_vio_calls * vio, int samples, int16_t * buf, int mode) {
 int i;
 int samples_half = samples/2;
 int64_t suml, sumr;
 double  rmsl, rmsr;

 printf("\e[s");
 fflush(stdout);

 while (roar_vio_read(vio, buf, samples * 2) > 0) {
  suml = sumr = 0;

  for (i = 0; i < samples; i += 2) {
   suml += (int64_t) buf[i  ] * (int64_t) buf[i  ];
   sumr += (int64_t) buf[i+1] * (int64_t) buf[i+1];
  }

  rmsl = sqrt((double)suml/(double)samples_half);
  rmsr = sqrt((double)sumr/(double)samples_half);

  switch (mode) {
   case MODE_PC:
     printf("L: %3i%% R: %3i%%          \e[u", (int)(rmsl/327.68), (int)(rmsr/327.68));
    break;
   case MODE_DB:
     printf("L: %6.2fdB R: %6.2fdB          \e[u", 20*log10(rmsl/32768.), 20*log10(rmsr/32768.));
    break;
  }

  fflush(stdout);
 }
 return 0;
}

int vumeter (struct roar_vio_calls * vio, int samples, int bits, int channels, int mode) {
 void * buf = malloc(samples*bits*2);

 if ( !buf )
  return -1;

 if ( bits == 16 ) {
  if ( channels == 2 ) {
   vumeter16bit2ch(vio, samples, (int16_t *) buf, mode);
   free(buf);
   return 0;
  } else {
   return -1;
  }
 } else {
  return -1;
 }
}

int main (int argc, char * argv[]) {
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 int    samples  = 441;
 char * server   = NULL;
 char * k;
 struct roar_vio_calls stream, re;
 int    i;
 int    mode = MODE_PC;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--rate") == 0 ) {
   rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 ) {
   channels = atoi(argv[++i]);
  } else if ( strcmp(k, "--samples") == 0 ) {
   samples = atoi(argv[++i]);
  } else if ( strcmp(k, "--db") == 0 ) {
   mode = MODE_DB;
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( roar_vio_simple_stream(&stream, rate, channels, bits, codec, server, ROAR_DIR_MONITOR, "roarvumeter") == -1) {
  fprintf(stderr, "Error: can not start monetoring\n");
  return 1;
 }

 if ( roar_vio_open_re(&re, &stream) == -1 ) {
  roar_vio_close(&stream);
  fprintf(stderr, "Error: can not open RE VIO layer\n");
  return 1;
 }

 vumeter(&re, samples*channels, bits, channels, mode);

 printf("\n"); // if the reach this then roard has quited and we should print a newline

 roar_vio_close(&re);

 return 0;
}

#else
int main (void) {
 fprintf(stderr, "Error: No Math library support compiled in.\n");
 return 1;
}
#endif

//ll
