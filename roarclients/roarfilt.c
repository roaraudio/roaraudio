//roarfilt.c:

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

#include <roaraudio.h>
#include <libroardsp/libroardsp.h>
#include <math.h>

#define BUFSIZE 1024
struct {
 uint16_t a, b;
 int16_t  old[ROAR_MAX_CHANNELS];
} g_lowpass;

void usage (void) {
 printf("roarfilt [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
        "  --help             - Show this help\n"
        "\n"
        "  --half             - half the volume\n"
        "  --double           - double the volume\n"
        "  --amp VAL          - Set amplification\n"
        "  --mul VAL          - Set mul\n"
        "  --div VAL          - Set div\n"
        "  --lowpass freq     - lowpass filter\n"
        "  --filter  name     - add filter name\n"
        "  --ffreq   freq     - set filter freq\n"
        "  --fmul    mult     - set filter multiplier\n"
        "  --fdiv    div      - set filter divider\n"
        "  --fn      N        - set filter N parameter\n"
        "  --flimit  limit    - set filter limit parameter\n"
        "  --fmode   mode     - set filter mode parameter\n"
        "  --fq      Q        - set filter quality\n"
       );

}

void vol2 (void * data, int mul, int div, int len) {
 int16_t * samples = (int16_t *) data;
 int i;

 len /= 2;

 for (i = 0; i < len; i++)
  samples[i] = ((int) samples[i] * mul) / div;
}

void vol1 (void * data, int mul, int div, int len) {
 int8_t * samples = (int8_t *) data;
 int i;

 for (i = 0; i < len; i++)
  samples[i] = ((int) samples[i] * mul) / div;
}

void logs2 (void * data, float scale, int len) {
 int16_t * samples = (int16_t *) data;
 int i;
 float div = logf(scale);
 float scalemul = scale - 1;
 int neg;

 len /= 2;

 //printf("logs2(data=%p, scale=%f, len=%i): scalemul=%f, div=%f\n", data, scale, len, scalemul, div);

 for (i = 0; i < len; i++) {
  if ( (neg = (samples[i] < 0)) ) 
   samples[i] = abs(samples[i]);


  samples[i] = (neg ? 32768.0 : 32767.0)*logf(1 + (scalemul*(float)samples[i]/(neg ? 32768.0 : 32767.0))) / div;

  if ( neg )
   samples[i] *= -1;
 }
}

void lowpass2 (void * data, int len, int channels) {
 int16_t * samples = (int16_t *) data;
 register int32_t s;
 int i, c;

 if ( channels > ROAR_MAX_CHANNELS )
  return;

 len /= 2 * channels;

//  *      output[N] = input[N] * A + output[N-1] * B

 for (i = 0; i < len; i++) {
  for (c = 0; c < channels; c++) {
   s = samples[i*channels + c] * g_lowpass.a + g_lowpass.old[c] * g_lowpass.b;

   s /= 65536;

   samples[i*channels + c] = s;
   g_lowpass.old[       c] = s;
  }
 }
}

int main (int argc, char * argv[]) {
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 char * k;
 int    fh;
 int    i;
 int    mul = 1, div = 1;
 int32_t tmp;
 float  logscale = 0;
 float  lp       = 0;
 char buf[BUFSIZE];
 struct roardsp_filterchain fc;
 struct roardsp_filter      filter_real[8];
 struct roardsp_filter    * filter = filter_real - 1;
 struct roar_stream         stream;

 memset(&g_lowpass, 0, sizeof(g_lowpass));

 roardsp_fchain_init(&fc);

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--rate") == 0 ) {
   rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 || strcmp(k, "--chans") == 0 ) {
   channels = atoi(argv[++i]);
  } else if ( strcmp(k, "--half") == 0 ) {
   div *= 2;
  } else if ( strcmp(k, "--double") == 0 ) {
   mul *= 2;
  } else if ( strcmp(k, "--amp") == 0 ) {
   mul *= atoi(argv[++i]);
  } else if ( strcmp(k, "--mul") == 0 ) {
   mul  = atoi(argv[++i]);
  } else if ( strcmp(k, "--div") == 0 ) {
   div  = atoi(argv[++i]);
  } else if ( strcmp(k, "--log") == 0 ) {
   logscale = atof(argv[++i]);
  } else if ( strcmp(k, "--lowpass") == 0 ) {
   lp = exp(-2 * M_PI * atof(argv[++i]) / rate) * 65536;
   g_lowpass.b = lp;
   g_lowpass.a = 65536 - lp;
//   printf("lowpass: A=%i, B=%i\n", g_lowpass.a, g_lowpass.b);
  } else if ( strcmp(k, "--filter") == 0 ) {
   stream.info.channels = channels;
   stream.info.bits     = bits;
   stream.info.rate     = rate;
   filter++;
   roardsp_filter_init(filter, &stream, roardsp_filter_str2id(argv[++i]));
   roardsp_fchain_add(&fc, filter);
  } else if ( strcmp(k, "--ffreq") == 0 ) {
   lp = atof(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_FREQ, &lp);
  } else if ( strcmp(k, "--fmul") == 0 ) {
   tmp = atoi(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_MUL, &tmp);
  } else if ( strcmp(k, "--fdiv") == 0 ) {
   tmp = atoi(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_DIV, &tmp);
  } else if ( strcmp(k, "--fn") == 0 ) {
   tmp = atoi(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_N, &tmp);
  } else if ( strcmp(k, "--fq") == 0 ) {
   tmp = atoi(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_Q, &tmp);
  } else if ( strcmp(k, "--flimit") == 0 ) {
   tmp = atoi(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_LIMIT, &tmp);
  } else if ( strcmp(k, "--fmode") == 0 ) {
   tmp = atoi(argv[++i]);
   roardsp_filter_ctl(filter, ROARDSP_FCTL_MODE, &tmp);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( (fh = roar_simple_filter(rate, channels, bits, codec, server, "roarifilt")) == -1 ) {
  fprintf(stderr, "Error: can not start playback\n");
  return 1;
 }

 if ( mul == div && logscale == 0 && g_lowpass.a == 0 && roardsp_fchain_num(&fc) == 0 ) {
  fprintf(stderr, "Error: filter is useless!\n");
  return 0;
 }

 if ( bits == 16 ) {
  while((i = read(fh, buf, BUFSIZE))) {
   if ( mul != div )
    vol2((void*)buf, mul, div, i);
   if ( logscale )
    logs2((void*)buf, logscale, i);
   if ( g_lowpass.a )
    lowpass2((void*)buf, i, channels);
   roardsp_fchain_calc(&fc, (void*)buf, (8*i)/bits);
   if (write(fh, buf, i) != i)
    break;
  }
 } else if ( bits == 8 ) {
  while((i = read(fh, buf, BUFSIZE))) {
   vol1((void*)buf, mul, div, i);
   if (write(fh, buf, i) != i)
    break;
  }
 } else {
  fprintf(stderr, "Error: %i bits per sample is not supported!\n", bits);
  return 1;
 }

 roar_simple_close(fh);

 roardsp_fchain_uninit(&fc);

 return 0;
}

//ll
