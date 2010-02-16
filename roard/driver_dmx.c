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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifndef ROAR_WITHOUT_DCOMP_DMX

int driver_dmx_open_vio  (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {

 if ( fh == -1 ) {
  if ( device == NULL )
   device = getenv("DMX");

  if ( device == NULL )
   device = "/dev/dmx";

  if ( roar_vio_open_file(inst, device, O_WRONLY, 0644) == -1 )
   return -1;
 } else {
  if ( roar_vio_open_fh(inst, fh) == -1 )
   return -1;
 }

 inst->write = driver_dmx_write;
 inst->ctl   = driver_dmx_ctl;

 info->codec = ROAR_CODEC_DMX512;

 if ( sstream != NULL )
  driver_dmx_ctl(inst, ROAR_VIO_CTL_SET_SSTREAM, sstream);

 return 0;
}

ssize_t driver_dmx_write (struct roar_vio_calls * vio,  void *buf, size_t count) {
 if ( vio == NULL || buf == NULL )
  return -1;

 if ( count != 512 )
  return -1;

 if ( roar_vio_basic_lseek(vio, 0, SEEK_SET) == (off_t)-1 )
  return -1;

 return roar_vio_basic_write(vio, buf, count);
}

int driver_dmx_ctl(struct roar_vio_calls * vio, int cmd, void * data) {

 if ( vio == NULL )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_SET_SSTREAM:
    ROAR_STREAM(data)->dir = ROAR_DIR_LIGHT_OUT;
    ROAR_STREAM_SERVER(data)->codec_orgi = ROAR_CODEC_DMX512;
   break;
  default:
   return -1;
 }

 return 0;
}

#endif

//ll
