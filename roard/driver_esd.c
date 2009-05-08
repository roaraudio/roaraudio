//driver_esd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
#ifdef ROAR_HAVE_ESD

/*
 We could use inst as our fh directly. But esd works with unsigned at 8 bits and
 signed at 16 bits per sample. (why???) so we need to convert because we get signed at both,
 8 and 16 bits per sample. so we use inst as an array of two ints: 0: fh, 1: are we in 8 bit mode?
*/

int driver_esd_open_sysio(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
 esd_format_t format = ESD_STREAM | ESD_PLAY;
 char name[80] = "roard";
 int * di = malloc(sizeof(int)*2);

 if ( di == NULL )
  return -1;

 *inst = (DRIVER_USERDATA_T)di;

 format |= info->bits     == 16 ? ESD_BITS16 : ESD_BITS8;
 format |= info->channels ==  2 ? ESD_STEREO : ESD_MONO;

 di[1] = info->bits == 8;

 di[0] = esd_play_stream_fallback(format, info->rate, device, name);

 shutdown(di[0], SHUT_RD);

 if ( di[0] == -1 ) {
  free(di);
  *inst = NULL;
  return -1;
 }

 return 0;
}

int driver_esd_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh) {

 if ( fh != -1 )
  return -1;

 inst->read     = driver_esd_read;
 inst->write    = driver_esd_write;
 inst->nonblock = driver_esd_nonblock;
 inst->sync     = driver_esd_sync;

 return driver_esd_open_sysio(&(inst->inst), device, info);
}

int driver_esd_close(DRIVER_USERDATA_T   inst) {
 int fh;

 inst = ((struct roar_vio_calls *)inst)->inst;

 fh = *(int*)inst;

 free((void*)inst);

 return esd_close(fh);
}

int driver_esd_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

ssize_t driver_esd_write(struct roar_vio_calls * inst, void * buf, size_t len) {
 int * di = (int*)((struct roar_vio_calls *)inst)->inst;

 if ( di[1] )
  roar_conv_codec_s2u8(buf, buf, len);

 return write(di[0], buf, len);
}

ssize_t driver_esd_read(struct roar_vio_calls * inst, void * buf, size_t len) {
 return read(*(int*)((struct roar_vio_calls *)inst)->inst, buf, len);
}

int driver_esd_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

int driver_esd_nonblock(struct roar_vio_calls * vio, int state) {
 return roar_socket_nonblock(*(int*)vio->inst, state);
}

int driver_esd_sync    (struct roar_vio_calls * vio) {
 return ROAR_FDATASYNC(*(int*)vio->inst);
}

#endif
//ll
