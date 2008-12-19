//libroardsp.h:

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

#ifndef _LIBROARDSP_H_
#define _LIBROARDSP_H_

#include <roaraudio.h>

__BEGIN_DECLS

#include "midi.h"

// defines
#define ROARDSP_MAX_FILTERS_PER_CHAIN 8

#define ROARDSP_FILTER_NONE           0
#define ROARDSP_FILTER_AMP            1
#define ROARDSP_FILTER_LOWP           2
#define ROARDSP_FILTER_HIGHP          3
#define ROARDSP_FILTER_MODULATE       4
#define ROARDSP_FILTER_QUANTIFY       5
#define ROARDSP_FILTER_CLIP           6
#define ROARDSP_FILTER_ADD            7

// filter CTLs:

#define ROARDSP_FCTL_FREQ             1
#define ROARDSP_FCTL_TIME             2
#define ROARDSP_FCTL_MUL              3
#define ROARDSP_FCTL_DIV              4
#define ROARDSP_FCTL_N                5
#define ROARDSP_FCTL_LIMIT            6
#define ROARDSP_FCTL_PHASE            7
#define ROARDSP_FCTL_Q                8
#define ROARDSP_FCTL_MODE             9

// types:

struct roardsp_filter {
 int    channels;
 int    bits;
 int    rate;
 void * inst;
 int (*calc  )(struct roardsp_filter * filter, void * data, size_t samples);
 int (*uninit)(struct roardsp_filter * filter);
 int (*ctl   )(struct roardsp_filter * filter, int cmd, void * data);
};

struct roardsp_filterchain {
 int filters;
 struct roardsp_filter * filter[ROARDSP_MAX_FILTERS_PER_CHAIN];
};

struct roardsp_lowp {
 uint32_t freq; // in mHz (0Hz..4MHz)
 uint16_t a, b;
 int32_t  old[ROAR_MAX_CHANNELS];
};

struct roardsp_highp {
 uint32_t freq; // in mHz (0Hz..4MHz)
 int32_t  a, b, c;
 int32_t  oldout[ROAR_MAX_CHANNELS];
 int32_t  oldin[ROAR_MAX_CHANNELS];
};

struct roardsp_amp {
 int32_t  mul;
 int32_t  div;
};

// funcs:
int    roardsp_filter_str2id(char * str);
char * roardsp_filter_id2str(int id);
int    roardsp_filter_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int    roardsp_filter_uninit(struct roardsp_filter * filter);
int    roardsp_filter_calc  (struct roardsp_filter * filter, void * data, size_t len);
int    roardsp_filter_ctl   (struct roardsp_filter * filter, int cmd, void * data);

int roardsp_fchain_init  (struct roardsp_filterchain * chain);
int roardsp_fchain_uninit(struct roardsp_filterchain * chain);
int roardsp_fchain_add   (struct roardsp_filterchain * chain, struct roardsp_filter * filter);
int roardsp_fchain_calc  (struct roardsp_filterchain * chain, void * data, size_t len);
int roardsp_fchain_num   (struct roardsp_filterchain * chain);

// filter:

int roardsp_lowp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_lowp_uninit(struct roardsp_filter * filter);
int roardsp_lowp_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_lowp_ctl   (struct roardsp_filter * filter, int cmd, void * data);

int roardsp_highp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_highp_uninit(struct roardsp_filter * filter);
int roardsp_highp_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_highp_ctl   (struct roardsp_filter * filter, int cmd, void * data);

int roardsp_amp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_amp_uninit(struct roardsp_filter * filter);
int roardsp_amp_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_amp_calc8 (struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_amp_ctl   (struct roardsp_filter * filter, int cmd, void * data);

int roardsp_add_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_add_calc16(struct roardsp_filter * filter, void * data, size_t samples);

int roardsp_quantify_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_quantify_uninit(struct roardsp_filter * filter);
int roardsp_quantify_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_quantify_ctl   (struct roardsp_filter * filter, int cmd, void * data);

int roardsp_clip_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_clip_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_clip_ctl   (struct roardsp_filter * filter, int cmd, void * data);

// codecs:
int roardsp_conv_alaw2pcm16 (int16_t * out, char * in, size_t len);
int roardsp_conv_pcm162alaw (char * out, int16_t * in, size_t len);

int roardsp_conv_mulaw2pcm16 (int16_t * out, char * in, size_t len);
int roardsp_conv_pcm162mulaw (char * out, int16_t * in, size_t len);

__END_DECLS

#endif

//ll
