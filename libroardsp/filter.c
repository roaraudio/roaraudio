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
 int (*calc  [5][3])(struct roardsp_filter * filter, void * data, size_t samples);
} _roardsp_filterlist[] = {
 {ROARDSP_FILTER_AMP, "AMP", NULL, NULL, NULL, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_LOWP, "Lowpass", NULL, NULL, NULL, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {ROARDSP_FILTER_HIGHP, "Highpass", NULL, NULL, NULL, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}},
 {-1, NULL, NULL, NULL, NULL, {
           {NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL},{NULL, NULL, NULL}}}
};

int    roardsp_filter_str2id(char * str) {
 struct _roardsp_filterlist * l = _roardsp_filterlist;

 while ( l->id != -1 ) {
  if ( strcasecmp(l->name, str) == 0 )
   return l->id;
 }

 return -1;
}

char * roardsp_filter_id2str(int id) {
 struct _roardsp_filterlist * l = _roardsp_filterlist;

 while ( l->id != -1 ) {
  if ( l->id == id )
   return l->name;
 }

 return NULL;
}

int roardsp_filter_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct _roardsp_filterlist * l = _roardsp_filterlist;
 int bytes;
 int (*calc)(struct roardsp_filter * filter, void * data, size_t samples) = NULL;

 if ( filter == NULL )
  return -1;

 memset(filter, 0, sizeof(struct roardsp_filter));

 filter->channels = stream->info.channels;
 filter->bits     = stream->info.bits;

 bytes            = stream->info.bits / 8;

 while ( l->id != id )
  if ( l->id == -1 )
   return -1;

 filter->uninit = l->uninit;
 filter->ctl    = l->ctl;

 if ( filter->channels < 3 )
  calc = l->calc[bytes][filter->channels];

 if ( calc == NULL )
  calc = l->calc[bytes][0]; // for n channels

 if ( calc == NULL )
  return -1;

 if ( l->init )
  return l->init(filter, stream, id);

 return 0;
}

int roardsp_filter_uninit(struct roardsp_filter * filter) {
 int ret = 0;

 if ( filter == NULL )
  return -1;

 if ( filter->uninit )
  ret = filter->uninit(filter);

 memset(filter, 0, sizeof(struct roardsp_filter));

 return ret;
}

int roardsp_filter_calc  (struct roardsp_filter * filter, void * data, size_t len) {
 int ret = 0;

 if ( filter == NULL )
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

//ll
