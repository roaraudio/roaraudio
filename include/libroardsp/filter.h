//filter.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARDSP_FILTER_H_
#define _LIBROARDSP_FILTER_H_

#include "libroardsp.h"

#define ROARDSP_FILTER_NONE           0
#define ROARDSP_FILTER_AMP            1
#define ROARDSP_FILTER_LOWP           2
#define ROARDSP_FILTER_HIGHP          3
#define ROARDSP_FILTER_MODULATE       4
#define ROARDSP_FILTER_QUANTIFY       5
#define ROARDSP_FILTER_CLIP           6
#define ROARDSP_FILTER_ADD            7
#define ROARDSP_FILTER_DOWNMIX        8
#define ROARDSP_FILTER_DCBLOCK        9
#define ROARDSP_FILTER_SWAP          10
#define ROARDSP_FILTER_SPEEX_PREP    11
#define ROARDSP_FILTER_AGC           12

// filter CTLs:

#define ROARDSP_FCTL_FREQ             1 /* float   */
#define ROARDSP_FCTL_TIME             2
#define ROARDSP_FCTL_MUL              3 /* int32_t */
#define ROARDSP_FCTL_DIV              4 /* int32_t */
#define ROARDSP_FCTL_N                5 /* int32_t */
#define ROARDSP_FCTL_LIMIT            6 /* int32_t */
#define ROARDSP_FCTL_PHASE            7
#define ROARDSP_FCTL_Q                8 /* int32_t */
#define ROARDSP_FCTL_MODE             9 /* int32_t */
#define ROARDSP_FCTL_PACKET_SIZE     10 /* size_t  */

// consts for filter flags:
#define ROARDSP_FFLAG_NONE            0x0000
#define ROARDSP_FFLAG_FREE            0x0001

// consts for filter(chain) reset:
#define ROARDSP_RESET_NONE            0
#define ROARDSP_RESET_FULL            1
#define ROARDSP_RESET_STATE           2

// filter specific constants:
#define ROARDSP_DOWNMIX_LEFT          1
#define ROARDSP_DOWNMIX_RIGHT         2
#define ROARDSP_DOWNMIX_ARITHMETIC    3
#define ROARDSP_DOWNMIX_RMS           4

#define ROARDSP_DCBLOCK_NUMBLOCKS     100


#define ROARDSP_SPEEX_PREP_ON          0x0001
#define ROARDSP_SPEEX_PREP_OFF         0x0002
#define ROARDSP_SPEEX_PREP_MASK        (ROARDSP_SPEEX_PREP_ON|ROARDSP_SPEEX_PREP_OFF)

// Config Bit Vector
#define ROARDSP_SPEEX_PREP_CBV(opt,sw) ((sw)<<((opt)*2))
#define ROARDSP_SPEEX_PREP_CTB(opt,val) (((val) & ROARDSP_SPEEX_PREP_CBV((opt),ROARDSP_SPEEX_PREP_MASK)) >> ((opt)*2))

#define ROARDSP_SPEEX_PREP_DENOISE     0
#define ROARDSP_SPEEX_PREP_AGC         1
#define ROARDSP_SPEEX_PREP_VAD         2

#define ROARDSP_SPEEX_PREP_DENOISE_ON  ROARDSP_SPEEX_PREP_CBV(ROARDSP_SPEEX_PREP_DENOISE, ROARDSP_SPEEX_PREP_ON)
#define ROARDSP_SPEEX_PREP_DENOISE_OFF ROARDSP_SPEEX_PREP_CBV(ROARDSP_SPEEX_PREP_DENOISE, ROARDSP_SPEEX_PREP_OFF)
#define ROARDSP_SPEEX_PREP_AGC_ON      ROARDSP_SPEEX_PREP_CBV(ROARDSP_SPEEX_PREP_AGC, ROARDSP_SPEEX_PREP_ON)
#define ROARDSP_SPEEX_PREP_AGC_OFF     ROARDSP_SPEEX_PREP_CBV(ROARDSP_SPEEX_PREP_AGC, ROARDSP_SPEEX_PREP_OFF)
#define ROARDSP_SPEEX_PREP_VAD_ON      ROARDSP_SPEEX_PREP_CBV(ROARDSP_SPEEX_PREP_VAD, ROARDSP_SPEEX_PREP_ON)
#define ROARDSP_SPEEX_PREP_VAD_OFF     ROARDSP_SPEEX_PREP_CBV(ROARDSP_SPEEX_PREP_VAD, ROARDSP_SPEEX_PREP_OFF)


// types:

struct roardsp_filter {
 int    channels;
 int    bits;
 int    rate;
 void * inst;
 uint_least16_t flags;
 int (*calc  )(struct roardsp_filter * filter, void * data, size_t samples);
 int (*uninit)(struct roardsp_filter * filter);
 int (*ctl   )(struct roardsp_filter * filter, int cmd, void * data);
 int (*reset )(struct roardsp_filter * filter, int what);
};

#ifdef ROAR_HAVE_LIBM
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
#endif

struct roardsp_amp {
 int32_t  mul;
 int32_t  div;
};

struct roardsp_dcblock {
 int cur;
 int32_t dc[ROARDSP_DCBLOCK_NUMBLOCKS];
};

struct roardsp_swap {
 int map[ROAR_MAX_CHANNELS];
};

struct roardsp_agc {
 struct roardsp_filter * amp;
 uint32_t target_amp;
};

struct roardsp_speex_prep {
#ifdef _SPEEX_TYPES_H
 SpeexPreprocessState *preprocess;
 int frame_size;
#else
 char dummy[8];
#endif
};


// funcs:
int    roardsp_filter_str2id(char * str);
char * roardsp_filter_id2str(int id);
int    roardsp_filter_new   (struct roardsp_filter ** filter, struct roar_stream * stream, int id);
#define roardsp_filter_free(x) roardsp_filter_uninit((x))
int    roardsp_filter_init  (struct roardsp_filter *  filter, struct roar_stream * stream, int id);
int    roardsp_filter_uninit(struct roardsp_filter *  filter);
int    roardsp_filter_calc  (struct roardsp_filter *  filter, void * data, size_t len);
int    roardsp_filter_ctl   (struct roardsp_filter *  filter, int cmd, void * data);
int    roardsp_filter_reset (struct roardsp_filter *  filter, int what);


// filter:

#ifdef ROAR_HAVE_LIBM
int roardsp_lowp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_lowp_uninit(struct roardsp_filter * filter);
int roardsp_lowp_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_lowp_ctl   (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_lowp_reset (struct roardsp_filter * filter, int what);

int roardsp_highp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_highp_uninit(struct roardsp_filter * filter);
int roardsp_highp_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_highp_ctl   (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_highp_reset (struct roardsp_filter * filter, int what);
#endif

int roardsp_amp_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_amp_uninit(struct roardsp_filter * filter);
int roardsp_amp_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_amp_calc8 (struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_amp_ctl   (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_amp_reset (struct roardsp_filter * filter, int what);

int roardsp_add_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_add_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_add_reset (struct roardsp_filter * filter, int what);

int roardsp_quantify_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_quantify_uninit(struct roardsp_filter * filter);
int roardsp_quantify_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_quantify_ctl   (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_quantify_reset (struct roardsp_filter * filter, int what);

int roardsp_clip_calc16(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_clip_ctl   (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_clip_reset (struct roardsp_filter * filter, int what);

int roardsp_downmix_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_downmix_calc162(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_downmix_ctl    (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_downmix_reset  (struct roardsp_filter * filter, int what);

int roardsp_dcblock_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_dcblock_uninit (struct roardsp_filter * filter);
int roardsp_dcblock_calc16 (struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_dcblock_reset  (struct roardsp_filter * filter, int what);

int roardsp_swap_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_swap_uninit (struct roardsp_filter * filter);
int roardsp_swap_calc162(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_swap_ctl    (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_swap_reset  (struct roardsp_filter * filter, int what);

int roardsp_agc_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_agc_uninit (struct roardsp_filter * filter);
int roardsp_agc_ctl    (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_agc_reset  (struct roardsp_filter * filter, int what);

#ifdef _SPEEX_TYPES_H
#define ROAR_HAVE_SPEEX_FILTER
int roardsp_speex_prep_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id);
int roardsp_speex_prep_uninit (struct roardsp_filter * filter);
int roardsp_speex_prep_calc161(struct roardsp_filter * filter, void * data, size_t samples);
int roardsp_speex_prep_ctl    (struct roardsp_filter * filter, int cmd, void * data);
int roardsp_speex_prep_reset  (struct roardsp_filter * filter, int what);
#endif

#endif

//ll
