//volume.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#define ROAR_USE_OWN_SNDIO_HDL
#include "libroarsndio.h"

int    sio_setvol (struct sio_hdl * hdl, unsigned vol) {
 struct roar_mixer_settings mixer;
 int i;

 if ( hdl == NULL )
  return 0;

 if ( vol > SIO_MAXVOL )
  return 0;

 mixer.scale   = SIO_MAXVOL;
 mixer.rpg_mul = 1;
 mixer.rpg_div = 1;

 for (i = 0; i < hdl->info.channels; i++)
  mixer.mixer[i] = vol;

 if ( roar_set_vol(&(hdl->con), roar_stream_get_id(&(hdl->stream)), &mixer, hdl->info.channels) == -1 )
  return 0;

 if ( hdl->on_vol != NULL )
  hdl->on_vol(hdl->on_vol_arg, vol);

 return 1;
}

void   sio_onvol  (struct sio_hdl * hdl, void (*cb)(void * arg, unsigned vol), void * arg) {
 if ( hdl == NULL )
  return;

 hdl->on_vol     = cb;
 hdl->on_vol_arg = arg;
}

//ll
