//driver_dmx.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#include "roard.h"

int driver_dmx_open_vio  (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh) {

 if ( device == NULL ) {
  return -1;
 }

 if ( fh == -1 ) {
  if ( roar_vio_open_file(inst, device, O_WRONLY, 0644) == -1 )
   return -1;
 } else {
  if ( roar_vio_open_fh(inst, fh) == -1 )
   return -1;
 }

 inst->write = driver_dmx_write;
 inst->ctl   = driver_dmx_ctl;

 info->codec = ROAR_CODEC_DMX512;

 return 0;
}

ssize_t driver_dmx_write (struct roar_vio_calls * vio,  void *buf, size_t count) {
 if ( vio == NULL || buf == NULL )
  return -1;

 if ( count != 512 )
  return -1;

 if ( lseek(roar_vio_get_fh(vio), 0, SEEK_SET) == (off_t)-1 )
  return -1;

 return write(roar_vio_get_fh(vio), buf, count);
}

int driver_dmx_ctl(struct roar_vio_calls * vio, int cmd, void * data) {

 if ( vio == NULL )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_SET_SSTREAM:
    ROAR_STREAM(data)->dir = ROAR_DIR_LIGHT_OUT;
   break;
  default:
   return -1;
 }

 return 0;
}

//ll
