//filter_speex_prep.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifdef _SPEEX_TYPES_H
// TODO: check parameters we allready know:
int roardsp_speex_prep_init   (struct roardsp_filter * filter, struct roar_stream * stream, int id) {
 struct roardsp_speex_prep * self;

 if ( filter->channels != 1 )
  return -1;

 if ( filter->bits != 16 )
  return -1;

 self = roar_mm_malloc(sizeof(struct roardsp_speex_prep));

 if ( self == NULL )
  return -1;

 memset(self, 0, sizeof(struct roardsp_speex_prep));

 return 0;
}

int roardsp_speex_prep_uninit (struct roardsp_filter * filter) {
 struct roardsp_speex_prep * self = filter->inst;

 if ( self->preprocess != NULL )
  speex_preprocess_state_destroy(self->preprocess);

 roar_mm_free(self);

 return 0;
}

int roardsp_speex_prep_calc161(struct roardsp_filter * filter, void * data, size_t samples) {
 return -1;
}

int roardsp_speex_prep_ctl    (struct roardsp_filter * filter, int cmd, void * data) {
 struct roardsp_speex_prep * self = filter->inst;
 size_t * val;

 switch (cmd) {
  case ROARDSP_FCTL_PACKET_SIZE:
    val = data;
    self->frame_size = *val;

    self->preprocess = speex_preprocess_state_init(self->frame_size, filter->rate);
    if ( self->preprocess == NULL )
     return -1;

    return 0;
   break;
 }

 return -1;
}

int roardsp_speex_prep_reset  (struct roardsp_filter * filter, int what) {
 if ( what == ROARDSP_RESET_NONE )
  return 0;

 return -1;
}
#endif

//ll
