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

int roardsp_filter_init  (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 if ( filter == NULL )
  return -1;

 memset(filter, 0, sizeof(struct roardsp_filter));

 filter->channels = stream->info.channels;
 filter->bits     = stream->info.bits;

 return -1;
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

//ll
