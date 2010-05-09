//codecfilter_celt.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _CODECFILTER_CELT_H_
#define _CODECFILTER_CELT_H_

#include <roaraudio.h>

#ifdef ROAR_HAVE_LIBCELT

#include <celt/celt.h>
#include <celt/celt_header.h>

struct codecfilter_celt_inst {
 struct roar_stream_server * stream;
 CELTMode * mode;
 CELTEncoder * encoder;
 CELTDecoder * decoder;
 int frame_size;
 int lookahead;
 int out_size;
 char * ibuf;
 char * obuf;
 char * i_rest;
 char * o_rest;
 int s_buf;
 int fi_rest; /* how much is in rest? */
 int fo_rest; /* how much is in rest? */
 int opened_encoder;
 int opened_decoder;
};

int cf_celt_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_celt_close(CODECFILTER_USERDATA_T   inst);

int cf_celt_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_celt_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

int cf_celt_delay(CODECFILTER_USERDATA_T   inst, uint_least32_t * delay);

#endif

#endif

//ll
