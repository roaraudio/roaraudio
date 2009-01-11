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

#include <roaraudio.h>  /* libroar */

#ifdef ROAR_HAVE_LIBM

#include <math.h>       /* sin() */
#include <stdio.h>      /* *printf*() */
#include <libroardsp/libroardsp.h>

int main (void) {
 int rate     = ROAR_RATE_DEFAULT;
 int bits     = 16;
 int channels = 1; /* mono */
 int codec    = ROAR_CODEC_DEFAULT;
 float freq   = 523.2;            /* middle C */
 float step; //   = M_PI*2*freq/rate; /* how much time per sample we have to encode ... */
 float t      = 0; /* current time */
 float length = 5; /* 5 sec */
 int16_t amp  = 32767;
 int      i;
 int32_t  tmp;
 int16_t  out[1024];
 int16_t  maxval, maxval_in;
 uint32_t sc, sc_tot = 0;
 int      count = 0;
 float    rms, rms_in = amp/sqrt(2);
 struct roardsp_filterchain fc[1];
 struct roardsp_filter      filt[3];
 struct roar_stream         stream[1];

 if ( roar_stream_new(stream, rate, channels, bits, codec) == -1 )
  return 2;

 if ( roardsp_filter_init(filt, stream, ROARDSP_FILTER_LOWP) == -1 ) {
  ROAR_ERR("main(*): roardsp_filter_init() failed: errno=%s(%i)", strerror(errno), errno);
  return 1;
 }

 freq = 1000;
 roardsp_filter_ctl(filt, ROARDSP_FCTL_FREQ, &freq);

 if ( roardsp_filter_init(filt+1, stream, ROARDSP_FILTER_HIGHP) == -1 ) {
  ROAR_ERR("main(*): roardsp_filter_init() failed: errno=%s(%i)", strerror(errno), errno);
  return 1;
 }

 freq = 1000;
 roardsp_filter_ctl(filt+1, ROARDSP_FCTL_FREQ, &freq);

 if ( roardsp_filter_init(filt+2, stream, ROARDSP_FILTER_AMP) == -1 ) {
  ROAR_ERR("main(*): roardsp_filter_init() failed: errno=%s(%i)", strerror(errno), errno);
  return 1;
 }

 tmp = 2;
 roardsp_filter_ctl(filt+2, ROARDSP_FCTL_DIV, &tmp);
 tmp = 3;
 roardsp_filter_ctl(filt+2, ROARDSP_FCTL_MUL, &tmp);


 roardsp_fchain_init(fc);
 roardsp_fchain_add(fc, filt);
// roardsp_fchain_add(fc, filt+1);
// roardsp_fchain_add(fc, filt+2);

 fprintf(stderr, "Starting analysis in frequency domain...\n");

 for (freq = 1/* 2*exp(1) */; freq < (float)rate/2; freq *= (1+exp(1)/100), length /= (1+exp(1)/100)) {
  step      = M_PI*2*freq/rate;
  maxval    = -amp;
  maxval_in = -amp;
  sc        = 0;
  rms       = 0;
  t         = 0;

  while (t < 2*M_PI*freq*length) {
   for (i = 0; i < 1024; i++) {
    out[i] = amp*sin(t);
    if ( out[i] > maxval_in )
     maxval_in = out[i];
    t += step;
   }

   roardsp_fchain_calc(fc, out, 1024);

   for (i = 0; i < 1024; i++) {
    sc++;
    rms += out[i] * out[i];
    if ( out[i] > maxval )
     maxval = out[i];
   }
  }

  count++;
  sc_tot += sc;

  rms /= sc;
  rms  = sqrt(rms);
//  rms /= amp;
  rms /= rms_in;

  printf("%f: %f %u %f\n", freq, (float)maxval/maxval_in, sc, rms);
 }

 fprintf(stderr, "Finished analysis in frequency domain: done tests on a total of %u samples on %i frequencies\n",
           sc_tot, count);

 roardsp_fchain_uninit(fc);

 return 0;
}

#else
int main (void) {
 fprintf(stderr, "Error: No Math library support compiled in.\n");
 return 1;
}
#endif

//ll
