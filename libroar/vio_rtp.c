//vio_rtp.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
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
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

int roar_vio_open_rtp        (struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                              char * dstr, struct roar_vio_defaults * odef) {
 struct roar_rtp_inst * self = NULL;

 if ( calls == NULL || dst == NULL )
  return -1;

 if ( (self = roar_mm_malloc(sizeof(struct roar_rtp_inst))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct roar_rtp_inst));

 self->vio                 = dst;

 self->header.version      = 2;
 self->header.payload_type = ROAR_RTP_PT_UNKNOWN;

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst               = self;
// calls->read               = roar_vio_rtp_read;
// calls->write              = roar_vio_rtp_write;
// calls->lseek              = roar_vio_rtp_lseek;
 calls->nonblock           = roar_vio_rtp_nonblock;
 calls->sync               = roar_vio_rtp_sync;
 calls->ctl                = roar_vio_rtp_ctl;
 calls->close              = roar_vio_rtp_close;

 return 0;
}

ssize_t roar_vio_rtp_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t roar_vio_rtp_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   roar_vio_rtp_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);

int     roar_vio_rtp_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_rtp_inst * self = vio->inst;

 return roar_vio_nonblock(self->vio, state);
}

int     roar_vio_rtp_sync    (struct roar_vio_calls * vio) {
 struct roar_rtp_inst * self = vio->inst;

 return roar_vio_sync(self->vio);
}

int     roar_vio_rtp_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 struct roar_rtp_inst * self = vio->inst;

 if (vio == NULL || cmd == -1)
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NEXT:
    *(struct roar_vio_calls **)data = self->vio;
    return 0;
   break;
  case ROAR_VIO_CTL_SET_NEXT:
    self->vio = *(struct roar_vio_calls **)data;
    return 0;
   break;
 }

 return roar_vio_ctl(self->vio, cmd, data);
}

int     roar_vio_rtp_close   (struct roar_vio_calls * vio) {
 struct roar_rtp_inst * self = vio->inst;
 int ret;

 ret = roar_vio_close(self->vio);

 roar_mm_free(self);

 return ret;
}

//ll
