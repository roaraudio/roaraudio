//highp.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroardsp.h"

#ifdef ROAR_HAVE_LIBM

int roardsp_highp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_highp * self = roar_mm_malloc(sizeof(struct roardsp_highp));
 float freq = 25;

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roardsp_highp));

 filter->inst = (void*) self;

 roardsp_highp_ctl(filter, ROARDSP_FCTL_FREQ, &freq);

 return 0;
}

int roardsp_highp_uninit(struct roardsp_filter * filter) {

 roar_mm_free(filter->inst);
 return 0;
}

int roardsp_highp_calc16(struct roardsp_filter * filter, void * data, size_t samples) {
 struct roardsp_highp * self = (struct roardsp_highp *) filter->inst;
 int16_t * samp = (int16_t *) data;
 register int32_t s, h;
 int i, c;
 int channels = filter->channels;

 if ( channels > ROAR_MAX_CHANNELS )
  return -1;

 samples /= channels;

 ROAR_DBG("roardsp_highp_calc16(*): filtering %i frames of %i channels...", samples, channels);

// *      output[N] = A0 * input[N] + A1 * input[N-1] + B1 * output[N-1]


 for (i = 0; i < samples; i++) {
  for (c = 0; c < channels; c++) {
   h = samp[i*channels + c];
   s = samp[i*channels + c] * self->a + self->oldin[c] * self->b + self->oldout[c] * self->c;

   s /= 65536;

   self->oldin[      c] = h;
   samp[i*channels + c] = s;
   self->oldout[     c] = s;
  }
 }

 return 0;
}

int roardsp_highp_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 struct roardsp_highp * self = (struct roardsp_highp *) filter->inst;
 float lp;
 float oldfreq;
 float newfreq;

 if ( cmd != ROARDSP_FCTL_FREQ )
  return -1;

 newfreq = *(float*)data;

 lp = exp(-2.0 * M_PI * newfreq / (float) filter->rate) * 65536;

 self->a =  (65536 + lp)/2.0;
 self->b = -(65536 + lp)/2.0;
 self->c =           lp;


 oldfreq = self->freq / 1000;
 self->freq = newfreq * 1000;

 *(float*)data = oldfreq;

 ROAR_DBG("roardsp_highp_ctl(*): oldfreq=%f, newfreq=%f", oldfreq, newfreq);
 ROAR_DBG("roardsp_highp_ctl(*): a=%i, b=%i, c=%i", self->a, self->b, self->c);

 return 0;
}

int roardsp_highp_reset (struct roardsp_filter * filter, int what) {
 struct roardsp_highp * self;
 float freq = 25;

 if ( filter == NULL )
  return -1;

 if ( filter->inst == NULL )
  return -1;

 self = filter->inst;

 switch (what) {
  case ROARDSP_RESET_NONE:
    return  0;
   break;
  case ROARDSP_RESET_FULL:
    roardsp_highp_ctl(filter, ROARDSP_FCTL_FREQ, &freq);
  case ROARDSP_RESET_STATE:
    memset(self->oldin,  0, sizeof(int32_t)*ROAR_MAX_CHANNELS);
    memset(self->oldout, 0, sizeof(int32_t)*ROAR_MAX_CHANNELS);
    return  0;
   break;
  default:
    return -1;
 }

 return -1;
}

#endif

//ll
