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
ssize_t roar_vio_rtp_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_rtp_inst * self = vio->inst;
 size_t len_need = count + sizeof(struct roar_rtp_header); // this is a bit more than we need
                                                           // we ignore this at the moment
 size_t len_have;
 union {
  void     * vp;
  char     * cp;
  uint16_t * u16;
  uint32_t * u32;
 } data;
 size_t  dataoffset;
 ssize_t ret;
 int     i;

 if ( self->tx == NULL ) {
  if ( roar_buffer_new(&(self->tx), len_need) == -1 )
   return -1;

  len_have = len_need;
 } else {
  if ( roar_buffer_get_len(self->tx, &len_have) == -1 )
   return -1;

  if ( len_have < len_need ) {
   if ( roar_buffer_set_len(self->tx, len_need) == -1 ) {
    if ( roar_buffer_free(self->tx) == -1 )
     return -1;

    self->tx = NULL;
    return roar_vio_rtp_write(vio, buf, count); // restart ower self from the beginning with no buffer
   }
  }
 }

 if ( roar_buffer_get_data(self->tx, &(data.vp)) == -1 )
  return -1;

 memset(data.vp, 0, len_need);

 data.cp[0]   = 2;
 data.cp[0]  |= self->header.csrc_count   << 4;
 data.cp[1]  |= self->header.payload_type << 1;

 data.u16[1]  = self->header.seq_num;

 data.u32[1]  = self->header.ts;
 data.u32[2]  = self->header.ssrc;

 for (i = 0; i < self->header.csrc_count; i++) {
  data.u32[3+i] = self->header.csrc[i];
 }

 dataoffset   = 3*4 + self->header.csrc_count*4;

 memcpy(data.vp + dataoffset, buf, count);

 if ( (ret = roar_vio_write(self->vio, data.vp, count+dataoffset)) == -1 )
  return -1;

 return ret - dataoffset;
}

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

 if ( self->tx != NULL )
  if ( roar_buffer_free(self->tx) == -1 )
   ret = -1;

 roar_mm_free(self);

 return ret;
}

//ll
