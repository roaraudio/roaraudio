//stream.c:

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

#define _i(x) (hdl->info.x)
int    sio_start  (struct sio_hdl * hdl) {
 if ( hdl == NULL )
  return 0;

 if ( hdl->fh != -1 )
  return 0;

 if ( (hdl->fh = roar_simple_play(_i(rate), _i(channels), _i(bits), _i(codec), hdl->device, "libroarsndio")) == -1 )
  return 0;

 return 1;
}
#undef _i

int    sio_stop   (struct sio_hdl * hdl) {

 if ( hdl == NULL )
  return 0;

 if ( hdl->fh == -1 )
  return 0;

 close(hdl->fh);

 hdl->fh = -1;

 return 1;
}

size_t sio_read   (struct sio_hdl * hdl, void * addr, size_t nbytes) {
 ssize_t ret;

 if ( hdl == NULL )
  return 0;

 if ( hdl->fh == -1 )
  return 0;

 if ( (ret = read(hdl->fh, addr, nbytes)) < 0 )
  return 0;

 return ret;
}
size_t sio_write  (struct sio_hdl * hdl, void * addr, size_t nbytes) {
 ssize_t ret;

 if ( hdl == NULL )
  return 0;

 if ( hdl->fh == -1 )
  return 0;

 if ( (ret = write(hdl->fh, addr, nbytes)) < 0 )
  return 0;

 return ret;
}

//ll
