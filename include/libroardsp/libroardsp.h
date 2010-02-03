//libroardsp.h:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _LIBROARDSP_H_
#define _LIBROARDSP_H_

#include <roaraudio.h>

__BEGIN_DECLS

// enable Speex preprocessing and better type handling if speex > 1.1.8
#ifdef ROAR_HAVE_LIBSPEEX
#include <speex/speex.h>
#ifdef _SPEEX_TYPES_H
#include <speex/speex_preprocess.h>
#endif
#endif

#include "interleave.h"
#include "midi.h"
#include "synth.h"
#include "poly.h"
#include "fader.h"
#include "mixer.h"
#include "amp.h"
#include "convert.h"
#include "midside.h"
#include "point.h"
#include "remove.h"
#include "rms.h"
#include "filter.h"
#include "filterchain.h"
#include "transcode.h"
#include "vio_transcode.h"

#ifdef ROAR_HAVE_LIBCELT
#include "transcode_celt.h"
#endif
#ifdef ROAR_HAVE_LIBSPEEX
#include "transcode_speex.h"
#endif

#ifdef ROAR_HAVE_LIBSAMPLERATE
#include <samplerate.h>
#endif

// defines
#define ROARDSP_MAX_FILTERS_PER_CHAIN 8

// types:

struct roardsp_filterchain {
 int filters;
 struct roardsp_filter * filter[ROARDSP_MAX_FILTERS_PER_CHAIN];
};

// funcs:
int roardsp_fchain_init  (struct roardsp_filterchain * chain);
int roardsp_fchain_uninit(struct roardsp_filterchain * chain);
int roardsp_fchain_add   (struct roardsp_filterchain * chain, struct roardsp_filter * filter);
int roardsp_fchain_calc  (struct roardsp_filterchain * chain, void * data, size_t len);
int roardsp_fchain_reset (struct roardsp_filterchain * chain, int what);
int roardsp_fchain_num   (struct roardsp_filterchain * chain);

// codecs:
int roardsp_conv_alaw2pcm16 (int16_t * out, char * in, size_t len);
int roardsp_conv_pcm162alaw (char * out, int16_t * in, size_t len);

int roardsp_conv_mulaw2pcm16 (int16_t * out, char * in, size_t len);
int roardsp_conv_pcm162mulaw (char * out, int16_t * in, size_t len);

__END_DECLS

#endif

//ll
