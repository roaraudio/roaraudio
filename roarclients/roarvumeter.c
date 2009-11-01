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
#include <libroardsp/libroardsp.h>

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

int vumeter16bit2ch (struct roar_vio_calls * vio, int samples, int16_t * buf, int mode, struct roardsp_filterchain * fc) {
 int i;
 int samples_half = samples/2;
 int64_t suml, sumr;
 double  rmsl, rmsr;
 int run_filters = roardsp_fchain_num(fc);

 printf("\e[s");
 fflush(stdout);

 while (roar_vio_read(vio, buf, samples * 2) > 0) {
  suml = sumr = 0;

  if ( run_filters ) {
   roardsp_fchain_calc(fc, buf, samples * 2);
  }

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

int vumeter (struct roar_vio_calls * vio, int samples, int bits, int channels, int mode, struct roardsp_filterchain * fc) {
 void * buf = malloc((samples*bits*channels)/8);

 if ( !buf )
  return -1;

 if ( bits == 16 ) {
  if ( channels == 2 ) {
   vumeter16bit2ch(vio, samples, (int16_t *) buf, mode, fc);
   free(buf);
   return 0;
  } else {
   free(buf);
   return -1;
  }
 } else {
  free(buf);
  return -1;
 }
}

int main (int argc, char * argv[]) {
 struct roar_connection       con;
 struct roar_stream           s;
 struct roardsp_filterchain   fchain;
 struct roardsp_filter      * filter;
 float  lowpass_freq = 0;
 int    rate     = ROAR_RATE_DEFAULT;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 int    samples  = -1;
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
  } else if ( strcmp(k, "--lowpass") == 0 ) {
   lowpass_freq = atof(argv[++i]);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( samples == -1 )
  samples = rate/10;

 if ( roar_simple_connect(&con, server, "roarvumeter") == -1 ) {
  fprintf(stderr, "Error: can not connect to server!\n");
  return 1;
 }

 if ( roar_vio_simple_new_stream_obj(&stream, &con, &s, rate, channels, bits, codec, ROAR_DIR_MONITOR) == -1) {
  fprintf(stderr, "Error: can not start monetoring\n");
  return 1;
 }

 if ( roar_vio_open_re(&re, &stream) == -1 ) {
  roar_vio_close(&stream);
  fprintf(stderr, "Error: can not open RE VIO layer\n");
  return 1;
 }

 if ( roardsp_fchain_init(&fchain) == -1 ) {
  roar_vio_close(&re);
  fprintf(stderr, "Error: can not init filterchain\n");
  return 1;
 }

 if ( lowpass_freq > 1 ) {
  if ( roardsp_filter_new(&filter, &s, ROARDSP_FILTER_LOWP) == -1 ) {
   fprintf(stderr, "Error: can not open lowpass\n");
   roar_vio_close(&re);
   roardsp_fchain_uninit(&fchain);
   return 1;
  }

  if ( roardsp_filter_ctl(filter, ROARDSP_FCTL_FREQ, &lowpass_freq) == -1 ) {
   fprintf(stderr, "Error: can not set filter frequency\n");
   roar_vio_close(&re);
   roardsp_fchain_uninit(&fchain);
   return 1;
  }

  if ( roardsp_fchain_add(&fchain, filter) == -1 ) {
   fprintf(stderr, "Error: can not set filter frequency\n");
   roar_vio_close(&re);
   roardsp_fchain_uninit(&fchain);
   return 1;
  }
 }

 vumeter(&re, samples*channels, bits, channels, mode, &fchain);

 printf("\n"); // if the reach this then roard has quited and we should print a newline

 roar_vio_close(&re);

 roar_disconnect(&con);

 roardsp_fchain_uninit(&fchain);

 return 0;
}

#else
int main (void) {
 fprintf(stderr, "Error: No Math library support compiled in.\n");
 return 1;
}
#endif

//ll
