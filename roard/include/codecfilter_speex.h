//codecfilter_speex.h.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _CODECFILTER_SPEEX_H_
#define _CODECFILTER_SPEEX_H_

#include <roaraudio.h>
#include <speex/speex.h>
#include <speex/speex_stereo.h>
#include <speex/speex_callbacks.h>

struct codecfilter_speex_inst {
 void * encoder;
 void * decoder;
 SpeexBits bits;
 SpeexStereoState stereo_state;
 SpeexCallback    stereo_callback;
 int              stereo;
 struct roar_stream_server * stream;
 int frame_size;
 void     * cd; /* current data */
 char       cc[ROAR_SPEEX_MAX_CC]; /* buffer for read() and write() */
 void     * i_rest; /* rest... */
 int fi_rest;
 char * o_rest;
 int fo_rest; /* how much is in rest? */

/*
 char * ibuf;
 char * obuf;
 int out_size;
 char * i_rest;
 int s_buf;
 int fi_rest; /-* how much is in rest? *-/
*/
};

int cf_speex_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_speex_close(CODECFILTER_USERDATA_T   inst);

int cf_speex_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_speex_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

#endif

//ll
