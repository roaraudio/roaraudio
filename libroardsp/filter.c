//filter.c:

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

struct _roardsp_filterlist {
 int id;
 char * name;
 int (*  init      )(struct roardsp_filter * filter, struct roar_stream * stream, int id);
 int (*uninit      )(struct roardsp_filter * filter);
 int (*ctl         )(struct roardsp_filter * filter, int cmd, void * data);
 int (*reset       )(struct roardsp_filter * filter, int what);
 int (*calc  [5][3])(struct roardsp_filter * filter, void * data, size_t samples);
} _roardsp_filterlist[] = {
 {ROARDSP_FILTER_AMP, "AMP", roardsp_amp_init, roardsp_amp_uninit, roardsp_amp_ctl, roardsp_amp_reset, {
           {NULL, NULL, NULL},{roardsp_amp_calc8, NULL, NULL},{roardsp_amp_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_ADD, "Add", roardsp_add_init, roardsp_amp_uninit, roardsp_amp_ctl, roardsp_add_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{roardsp_add_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
#ifdef ROAR_HAVE_LIBM
 {ROARDSP_FILTER_LOWP, "Lowpass", roardsp_lowp_init, roardsp_lowp_uninit, roardsp_lowp_ctl, roardsp_lowp_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{roardsp_lowp_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_HIGHP, "Highpass", roardsp_highp_init, roardsp_highp_uninit, roardsp_highp_ctl, roardsp_highp_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{roardsp_highp_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
#endif
 {ROARDSP_FILTER_QUANTIFY, "Quantifier", roardsp_quantify_init, NULL, roardsp_quantify_ctl, roardsp_quantify_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{roardsp_quantify_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_CLIP, "Clip", roardsp_quantify_init, NULL, roardsp_clip_ctl, roardsp_clip_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{roardsp_clip_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_DOWNMIX, "downmix", roardsp_quantify_init, NULL, roardsp_downmix_ctl, roardsp_downmix_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, roardsp_downmix_calc162},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_DCBLOCK, "DCBlock", roardsp_dcblock_init, NULL, NULL, roardsp_dcblock_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{roardsp_dcblock_calc16, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_SWAP, "Swap", roardsp_swap_init, roardsp_swap_uninit, roardsp_swap_ctl, roardsp_swap_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, roardsp_swap_calc162},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_AGC, "AGC", roardsp_agc_init, roardsp_agc_uninit, roardsp_agc_ctl, roardsp_agc_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_SPEEX_PREP, "SpeexPrep", roardsp_speex_prep_init, roardsp_speex_prep_uninit,
                                          roardsp_speex_prep_ctl,  roardsp_speex_prep_reset, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, roardsp_speex_prep_calc161, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {-1, NULL, NULL, NULL, NULL, NULL, {
      // ?                  8Bit               16Bit              24Bit              32Bit
      // 0B:n     1     2   1B:n     1     2   2B:n     1     2   3B:n     1    2    4B:n     1     2
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}}
};

int    roardsp_filter_str2id(char * str) {
 struct _roardsp_filterlist * l = _roardsp_filterlist;

 while ( l->id != -1 ) {
  if ( strcasecmp(l->name, str) == 0 )
   return l->id;
  l++;
 }

 return -1;
}

char * roardsp_filter_id2str(int id) {
 struct _roardsp_filterlist * l = _roardsp_filterlist;

 while ( l->id != -1 ) {
  if ( l->id == id )
   return l->name;
  l++;
 }

 return NULL;
}

int    roardsp_filter_new   (struct roardsp_filter ** filter, struct roar_stream * stream, int id) {
 struct roardsp_filter * n;
 int ret;

 if ( filter == NULL || stream == NULL )
  return -1;

 *filter = NULL; // just to be sure

 n = roar_mm_malloc(sizeof(struct roardsp_filter));

 if ( n == NULL )
  return -1;

 if ( (ret = roardsp_filter_init(n, stream, id)) == -1 ) {
  roar_mm_free(n);
  return -1;
 }

 n->flags |= ROARDSP_FFLAG_FREE;

 *filter = n;

 return ret;
}

int roardsp_filter_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct _roardsp_filterlist * l = _roardsp_filterlist;
 int bytes;
 int (*calc)(struct roardsp_filter * filter, void * data, size_t samples) = NULL;

 if ( filter == NULL || stream == NULL ) {
  ROAR_DBG("roardsp_filter_init(*) = -1 // filter or stream is NULL");
  return -1;
 }

 ROAR_DBG("roardsp_filter_init(filter=%p, stream=%p, id=%i) = ?", filter, stream, id);

 memset(filter, 0, sizeof(struct roardsp_filter));

 filter->channels = stream->info.channels;
 filter->bits     = stream->info.bits;
 filter->rate     = stream->info.rate;

 bytes            = stream->info.bits / 8;

 while ( l->id != id ) {
  if ( l->id == -1 )
   return -1;
  l++;
 }

 filter->uninit = l->uninit;
 filter->ctl    = l->ctl;
 filter->reset  = l->reset;

 if ( filter->channels < 3 )
  calc = l->calc[bytes][filter->channels];

 if ( calc == NULL )
  calc = l->calc[bytes][0]; // for n channels

 if ( calc == NULL ) {
  ROAR_DBG("roardsp_filter_init(*) = -1 // no calc code");
  return -1;
 }

 filter->calc = calc;

 if ( l->init ) {
  ROAR_DBG("roardsp_filter_init(*) = ? // execing init");
  return l->init(filter, stream, id);
 }

 ROAR_DBG("roardsp_filter_init(*) = 0 // no init");
 return 0;
}

int roardsp_filter_uninit(struct roardsp_filter * filter) {
 int ret = 0;

 if ( filter == NULL )
  return -1;

 if ( filter->uninit )
  ret = filter->uninit(filter);

 if ( filter->flags & ROARDSP_FFLAG_FREE ) {
  roar_mm_free(filter);
 } else  {
  memset(filter, 0, sizeof(struct roardsp_filter));
 }

 return ret;
}

int roardsp_filter_calc  (struct roardsp_filter * filter, void * data, size_t len) {
 int ret = 0;

 if ( filter == NULL )
  return -1;

 if ( data == NULL && len != 0 )
  return -1;

 if ( filter->calc )
  ret = filter->calc(filter, data, len);

 return ret;
}

int    roardsp_filter_ctl   (struct roardsp_filter * filter, int cmd, void * data) {
 if ( filter == NULL )
  return -1;

 if ( filter->ctl )
  return filter->ctl(filter, cmd, data);

 return -1;
}

int    roardsp_filter_reset (struct roardsp_filter * filter, int what) {
 if ( filter == NULL )
  return -1;

 if ( filter->reset )
  return filter->reset(filter, what);

 return -1;
}

//ll
