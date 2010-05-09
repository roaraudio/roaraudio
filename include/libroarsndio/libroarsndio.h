//libroarsndio.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from EsounD.
 *  They are mostly copyrighted by Eric B. Mitchell (aka 'Ricdude)
 *  <ericmit@ix.netcom.com>. For more information see AUTHORS.esd.
 *
 *  This file is part of libroaresd a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 */

#ifndef _LIBROARSNDIO_H_
#define _LIBROARSNDIO_H_

#include <roaraudio.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(ROAR_HAVE_LIBSNDIO) && !defined(_ROAR_EMUL_LIBSNDIO) && !defined(ROAR_USE_OWN_SNDIO_HDL)
#include <sndio.h>
#else
#define sio_hdl roar_sio_hdl
#define mio_hdl roar_sio_hdl
#include "sndiosym.h"
#endif

#if !defined(ROAR_HAVE_LIBSNDIO) || defined(ROAR_USE_OWN_SNDIO_HDL)
#ifdef sio_hdl
#undef sio_hdl
#endif
#define sio_hdl roar_sio_hdl

#ifdef mio_hdl
#undef mio_hdl
#endif
#define mio_hdl roar_sio_hdl
#endif

struct roar_sio_hdl {
 char                   * device;
// int              fh;
 int                      stream_opened;
 int                      dir;
 int                      nonblock;
 int                      ioerror;
 struct roar_vio_calls    svio;
 struct roar_connection   con;
 struct roar_stream       stream;
 struct roar_audio_info   info;
 struct sio_par           para;
 void                   (*on_move)(void * arg, int delta);
 void                   * on_move_arg;
 void                   (*on_vol )(void * arg, unsigned vol);
 void                   * on_vol_arg;
};

#ifdef __cplusplus
}
#endif

#endif

//ll
