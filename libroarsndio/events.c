//events.c:

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

#define _CHECK() if ( hdl == NULL ) return 0

int    sio_nfds   (struct sio_hdl * hdl) {
#ifdef ROAR_HAVE_H_POLL
 int fh = -1;

 _CHECK();

 if ( hdl->stream_opened != 1 )
  return 0;

 if ( roar_vio_ctl(&(hdl->svio), ROAR_VIO_CTL_GET_FH, &fh) == -1 )
  return 0;

 if ( fh == -1 )
  return 0;

 return 1;
#else
 return 0;
#endif
}

int    sio_pollfd (struct sio_hdl * hdl, struct pollfd * pfd, int events) {
#ifdef ROAR_HAVE_H_POLL
 int num;
 int fh;

 _CHECK();

 if ( (num = sio_nfds(hdl)) == 0 )
  return 0;

 // not supportet currently:
 if ( num > 1 )
  return 0;

 memset(pfd, 0, num*sizeof(struct pollfd));

 // if stream is ok is tested by sio_nfds()

 if ( roar_vio_ctl(&(hdl->svio), ROAR_VIO_CTL_GET_FH, &fh) == -1 )
  return 0;

 if ( fh == -1 )
  return 0;

 pfd->fd      = fh;
 pfd->events  = events;
 pfd->revents = 0;

 return num;
#else
 return 0;
#endif
}

int    sio_revents(struct sio_hdl * hdl, struct pollfd * pfd) {
#ifdef ROAR_HAVE_H_POLL
 short revents = 0;
 int num;
 int i;

 _CHECK();

 if ( (num = sio_nfds(hdl)) == 0 )
  return 0;

 for (i = 0; i < num; i++)
  revents |= pfd[i].revents;

 return revents;
#else
 return 0;
#endif
}


//ll
