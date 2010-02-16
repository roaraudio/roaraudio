//midi.c:

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

struct mio_hdl * mio_open   (const char * name, unsigned mode, int nbio_flag) {
 return sio_open(name, mode, nbio_flag);
}

void             mio_close  (struct mio_hdl * hdl) {
 return sio_close(hdl);
}

size_t           mio_write  (struct mio_hdl * hdl, const void * addr, size_t nbytes) {
 return sio_write(hdl, addr, nbytes);
}

size_t           mio_read   (struct mio_hdl * hdl, void * addr, size_t nbytes) {
 return sio_read(hdl, addr, nbytes);
}

int              mio_nfds   (struct mio_hdl * hdl) {
 return mio_nfds(hdl);
}

int              mio_pollfd (struct mio_hdl * hdl, struct pollfd * pfd, int events) {
 return sio_pollfd(hdl, pfd, events);
}

int              mio_revents(struct mio_hdl * hdl, struct pollfd * pfd) {
 return sio_revents(hdl, pfd);
}

int              mio_eof    (struct mio_hdl * hdl) {
 return sio_eof(hdl);
}

//ll
