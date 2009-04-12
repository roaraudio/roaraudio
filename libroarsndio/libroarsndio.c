//libroarsndio.c:

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

#define ROAR_USE_OWN_SNDIO_HDL
#include "libroarsndio.h"

struct sio_hdl * sio_open(char * name, unsigned mode, int nbio_flag) {
 struct sio_hdl * hdl = NULL;

 if ( mode != SIO_PLAY ) /* currently we only support playback */
  return NULL;

 if ( (hdl = malloc(sizeof(struct sio_hdl))) == NULL )
  return NULL;

 memset(hdl, 0, sizeof(struct sio_hdl));

 sio_initpar(&(hdl->para));

 hdl->fh = -1;

 if ( name != NULL )
  hdl->device = strdup(name);

 return hdl;
}

void   sio_close  (struct sio_hdl * hdl) {
 if ( hdl == NULL )
  return;

 if ( hdl->fh == -1 )
  close(hdl->fh);

 free(hdl);
}

int    sio_eof    (struct sio_hdl * hdl) {
 return 0;
}

void   sio_onmove (struct sio_hdl * hdl, void (*cb)(void * arg, int delta), void * arg) {
 if ( hdl == NULL )
  return;

 hdl->on_move     = cb;
 hdl->on_move_arg = arg;
}

void   sio_onvol  (struct sio_hdl * hdl, void (*cb)(void * arg, unsigned vol), void * arg) {
 if ( hdl == NULL )
  return;

 hdl->on_vol     = cb;
 hdl->on_vol_arg = arg;
}

//ll
