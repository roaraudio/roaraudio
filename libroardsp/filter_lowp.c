//lowp.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - August 2008
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroardsp.h"

int roardsp_lowp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_lowp * self = malloc(sizeof(struct roardsp_lowp));
 float freq = filter->rate/2;

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roardsp_lowp));

 filter->inst = (void*) self;

 roardsp_lowp_ctl(filter, ROARDSP_FCTL_FREQ, &freq);

 return 0;
}

int roardsp_lowp_uninit(struct roardsp_filter * filter) {

 free(filter->inst);
 return 0;
}

int roardsp_lowp_calc16  (struct roardsp_filter * filter, void * data, size_t samples) {
 struct roardsp_lowp * self = (struct roardsp_lowp *) filter->inst;
 int16_t * samp = (int16_t *) data;
 register int32_t s;
 int i, c;
 int channels = filter->channels;

 if ( channels > ROAR_MAX_CHANNELS )
  return -1;

 samples /= channels;

 ROAR_DBG("roardsp_lowp_calc16(*): filtering %i frames of %i channels...", samples, channels);

//  *      output[N] = input[N] * A + output[N-1] * B

 for (i = 0; i < samples; i++) {
  for (c = 0; c < channels; c++) {
   s = samp[i*channels + c] * self->a + self->old[c] * self->b;

   s /= 65536;

   samp[i*channels + c] = s;
   self->old[        c] = s;
  }
 }

 return 0;
}

int roardsp_lowp_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 struct roardsp_lowp * self = (struct roardsp_lowp *) filter->inst;
 float lp;
 float oldfreq;
 float newfreq;

 if ( cmd != ROARDSP_FCTL_FREQ )
  return -1;

 newfreq = *(float*)data;

 lp = exp(-2 * M_PI * newfreq / filter->rate) * 65536;

 self->b = lp;
 self->a = 65536 - lp;

 oldfreq = self->freq / 1000;
 self->freq = newfreq * 1000;

 *(float*)data = oldfreq;

 ROAR_DBG("roardsp_lowp_ctl(); oldfreq=%f, newfreq=%f", oldfreq, newfreq);

 return 0;
}

//ll
