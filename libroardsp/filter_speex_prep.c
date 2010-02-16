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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "libroardsp.h"

#ifdef _SPEEX_TYPES_H

#if defined(ROAR_HAVE_LIBSPEEX) && !defined(ROAR_HAVE_LIBSPEEXDSP)
#define _SPEEX_API_OLD
#define _SPEEX_INT int
#elif defined(ROAR_HAVE_LIBSPEEX) && defined(ROAR_HAVE_LIBSPEEXDSP)
#define _SPEEX_API_NEW
#define _SPEEX_INT spx_int32_t
#endif

#define _on  1
#define _off 2

#define _CBVM(opt) (ROARDSP_SPEEX_PREP_CBV((opt), ROARDSP_SPEEX_PREP_MASK))

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

 filter->inst = self;

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
 struct roardsp_speex_prep * self = filter->inst;

 if ( self->preprocess == NULL )
  return -1;

 if ( samples != ((self->frame_size * filter->bits) / 8) )
  return -1;

#ifdef _SPEEX_API_OLD
 speex_preprocess(self->preprocess, data, NULL);
#elif defined(_SPEEX_API_NEW)
 speex_preprocess_run(self->preprocess, data);
#else
 return -1;
#endif

 return 0;
}

int roardsp_speex_prep_ctl    (struct roardsp_filter * filter, int cmd, void * data) {
 struct roardsp_speex_prep * self = filter->inst;
 union {
  size_t  size;
  int32_t i32;
 } * val = data;
 _SPEEX_INT si;

 switch (cmd) {
  case ROARDSP_FCTL_MODE:
    if ( self->preprocess == NULL )
     return -1;

    ROAR_DBG("roardsp_speex_prep_ctl(*): val->i32 = 0x%.8x", val->i32);
    ROAR_DBG("roardsp_speex_prep_ctl(*): _CBVM(ROARDSP_SPEEX_PREP_DENOISE) = 0x%.8x", _CBVM(ROARDSP_SPEEX_PREP_DENOISE));

    if ( val->i32 & _CBVM(ROARDSP_SPEEX_PREP_DENOISE) ) {
     ROAR_DBG("roardsp_speex_prep_ctl(*): ROARDSP_SPEEX_PREP_CTB(ROARDSP_SPEEX_PREP_DENOISE, val->i32) = 0x%.8x", ROARDSP_SPEEX_PREP_CTB(ROARDSP_SPEEX_PREP_DENOISE, val->i32));
     switch (ROARDSP_SPEEX_PREP_CTB(ROARDSP_SPEEX_PREP_DENOISE, val->i32)) {
      case ROARDSP_SPEEX_PREP_ON:  si = _on;  break;
      case ROARDSP_SPEEX_PREP_OFF: si = _off; break;
      default: return -1;
     }
     speex_preprocess_ctl(self->preprocess, SPEEX_PREPROCESS_SET_DENOISE, &si);
     val->i32 -= val->i32 & _CBVM(ROARDSP_SPEEX_PREP_DENOISE);
    }

    if ( val->i32 & _CBVM(ROARDSP_SPEEX_PREP_AGC) ) {
     switch (ROARDSP_SPEEX_PREP_CTB(ROARDSP_SPEEX_PREP_DENOISE, val->i32)) {
      case ROARDSP_SPEEX_PREP_ON:  si = _on;  break;
      case ROARDSP_SPEEX_PREP_OFF: si = _off; break;
      default: return -1;
     }
     speex_preprocess_ctl(self->preprocess, SPEEX_PREPROCESS_SET_AGC, &si);
     val->i32 -= val->i32 & _CBVM(ROARDSP_SPEEX_PREP_AGC);
    }

    if ( val->i32 & _CBVM(ROARDSP_SPEEX_PREP_VAD) ) {
     switch (ROARDSP_SPEEX_PREP_CTB(ROARDSP_SPEEX_PREP_DENOISE, val->i32)) {
      case ROARDSP_SPEEX_PREP_ON:  si = _on;  break;
      case ROARDSP_SPEEX_PREP_OFF: si = _off; break;
      default: return -1;
     }
     speex_preprocess_ctl(self->preprocess, SPEEX_PREPROCESS_SET_VAD, &si);
     val->i32 -= val->i32 & _CBVM(ROARDSP_SPEEX_PREP_VAD);
    }

    // any other options left? error:
    if ( val->i32 )
     return -1;

    return 0;
   break;
  case ROARDSP_FCTL_PACKET_SIZE:
    self->frame_size = val->size;

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
