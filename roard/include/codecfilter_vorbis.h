//codecfilter_vorbis.h:

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

#ifndef _CODECFILTER_VORBIS_H_
#define _CODECFILTER_VORBIS_H_

#include <roaraudio.h>

#if defined(ROAR_HAVE_LIBVORBISFILE) && defined(ROAR_REQUIRE_LIBVORBISFILE)

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <math.h>

#ifdef ROAR_HAVE_LIBVORBISENC
#include <vorbis/vorbisenc.h>
#endif

struct codecfilter_vorbis_inst {
 int current_section;
 int last_section;
 int opened;
 struct roar_stream_server * stream;
 OggVorbis_File vf;
 int got_it_running;
#ifdef ROAR_HAVE_LIBVORBISENC
 int encoding;
 struct {
  float v_base_quality;
  ogg_stream_state os;
  ogg_page         og;
  ogg_packet       op;

  vorbis_dsp_state vd;
  vorbis_block     vb;
  vorbis_info      vi;
  vorbis_comment   vc;
 } encoder;
#endif
};

size_t cf_vorbis_vfvio_read (void *ptr, size_t size, size_t nmemb, void *datasource);

int cf_vorbis_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_vorbis_close(CODECFILTER_USERDATA_T   inst);

int cf_vorbis_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_vorbis_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);

int cf_vorbis_update_stream (struct codecfilter_vorbis_inst * self);

#endif

#endif

//ll
