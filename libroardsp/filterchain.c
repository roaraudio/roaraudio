//filterchain.c:

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

int roardsp_fchain_init  (struct roardsp_filterchain * chain) {
 if ( chain == NULL )
  return -1;

 memset((void*)chain, 0, sizeof(struct roardsp_filterchain));
 return 0;
}

int roardsp_fchain_uninit(struct roardsp_filterchain * chain) {
 int i;
 int ret = 0;

 if ( chain == NULL )
  return -1;

 for (i = 0; i < chain->filters; i++) {
  if ( roardsp_filter_uninit(chain->filter[i]) == -1 )
   ret = -1;
 }

 if ( roardsp_fchain_init(chain) == -1 )
  ret = -1;

 return ret;
}

int roardsp_fchain_add   (struct roardsp_filterchain * chain, struct roardsp_filter * filter) {
 if ( chain == NULL )
  return -1;

 if ( chain->filters < ROARDSP_MAX_FILTERS_PER_CHAIN ) {
  chain->filter[chain->filters++] = filter;
  return 0;
 }

 return -1;
}

int roardsp_fchain_calc  (struct roardsp_filterchain * chain, void * data, size_t len) {
 int i;
 int ret = 0;

 if ( chain == NULL )
  return -1;

 for (i = 0; i < chain->filters; i++) {
  if ( roardsp_filter_calc(chain->filter[i], data, len) == -1 )
   ret = -1;
 }

 return ret;
}

int roardsp_fchain_reset (struct roardsp_filterchain * chain, int what) {
 int i;
 int ret = 0;
/*
 struct roardsp_filterchain backup[1];
*/

 if ( chain == NULL )
  return -1;

/*
 if ( what == ROARDSP_RESET_FULL ) {
  if ( roardsp_fchain_init(backup) == -1 )
   return -1;

  if ( roardsp_fchain_uninit(chain) == -1 )
   return -1;

  if ( roardsp_fchain_init(chain) == -1 )
   return -1;
 } else {
*/
  for (i = 0; i < chain->filters; i++) {
   if ( roardsp_filter_reset(chain->filter[i], what) == -1 )
    ret = -1;
  }
/*
 }
*/

 return ret;
}

int roardsp_fchain_num   (struct roardsp_filterchain * chain) {
 if ( chain == NULL )
  return -1;

 return chain->filters;
}

//ll
