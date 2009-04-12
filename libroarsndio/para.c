//para.c:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 */

#include "libroarsndio.h"

void   sio_initpar(struct sio_par * par) {
 if ( par == NULL )
  return;

 memset(par, 0, sizeof(struct sio_par));

 par->bits  = ROAR_BITS_DEFAULT;
 par->sig   = 1;
 par->le    = SIO_LE_NATIVE;
 par->msb   = 1;
 par->rchan = 0;
 par->pchan = ROAR_CHANNELS_DEFAULT;
 par->rate  = ROAR_RATE_DEFAULT;
 par->bufsz = ROAR_BITS_DEFAULT * ROAR_CHANNELS_DEFAULT * ROAR_RATE_DEFAULT / 800.0;
 par->round = 1;
 par->xrun  = SIO_IGNORE;

 return;
}

int    sio_setpar (struct sio_hdl * hdl, struct sio_par * par) {
 return 0;
}

int    sio_getpar (struct sio_hdl * hdl, struct sio_par * par) {
 return 0;
}

int    sio_getcap (struct sio_hdl * hdl, struct sio_cap * cap) {
 return 0;
}


//ll
