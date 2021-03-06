//driver_jack.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

#ifdef ROAR_HAVE_LIBJACK
int driver_jack_open_vio  (struct roar_vio_calls * inst,
                           char * device,
                           struct roar_audio_info * info,
                           int fh,
                           struct roar_stream_server * sstream) {
 struct driver_jack * self;

 // we are not FH Safe, return error if fh != -1:
 if ( fh != -1 )
  return -1;

 // set up VIO:
 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->read     = driver_jack_read;
 inst->write    = driver_jack_write;
 inst->lseek    = NULL; // no seeking on this device
 inst->nonblock = driver_jack_nonblock;
 inst->sync     = driver_jack_sync;
 inst->ctl      = driver_jack_ctl;
 inst->close    = driver_jack_close;

 // set up internal struct:
 if ( (self = roar_mm_malloc(sizeof(struct driver_jack))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct driver_jack));

 inst->inst     = self;

 if ( (self->client = jack_client_new("roard")) == NULL ) {
  roar_mm_free(self);
  return -1;
 }

 // return -1 on error or 0 on no error.
 return 0;
}

ssize_t driver_jack_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_jack * self = vio->inst;
 // read up to count bytes into buf.
 // return the number of bits read.
 return -1;
}

ssize_t driver_jack_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_jack * self = vio->inst;
 // write up to count bytes from buf.
 // return the number of written bytes.
 return -1;
}

int     driver_jack_nonblock(struct roar_vio_calls * vio, int state) {
 struct driver_jack * self = vio->inst;
 // control if read and write calls should block untill all data is read or written.
 // state could be:
 //  ROAR_SOCKET_BLOCK    - Block untill the data is read or written
 //  ROAR_SOCKET_NONBLOCK - Return as soon as possible
 return -1;
}

int     driver_jack_sync    (struct roar_vio_calls * vio) {
 struct driver_jack * self = vio->inst;
 // init to sync data to device.
 // sync does not need to be complet when this function returns.
 return 0;
}

int     driver_jack_ctl     (struct roar_vio_calls * vio, int cmd, void * data) {
 struct driver_jack * self = vio->inst;
 // function for a lot control features.

 switch (cmd) {
  case ROAR_VIO_CTL_GET_NAME:
    if ( data == NULL )
     return -1;

    *(char**)data = "driver_jack";
    return 0;
   break;
  case ROAR_VIO_CTL_GET_FH:
  case ROAR_VIO_CTL_GET_READ_FH:
  case ROAR_VIO_CTL_GET_WRITE_FH:
  case ROAR_VIO_CTL_GET_SELECT_FH:
  case ROAR_VIO_CTL_GET_SELECT_READ_FH:
  case ROAR_VIO_CTL_GET_SELECT_WRITE_FH:
/* Return FH if possible:
    *(int*)data = FH...;
*/
    return -1;
   break;
  case ROAR_VIO_CTL_SET_NOSYNC:
    vio->sync = NULL;
    return 0;
   break;
   case ROAR_VIO_CTL_GET_AUINFO:
   case ROAR_VIO_CTL_SET_AUINFO:
    // get or set audio info, data is a struct roar_audio_info*.
    return -1;
   break;
   case ROAR_VIO_CTL_GET_DBLKSIZE:
   case ROAR_VIO_CTL_SET_DBLKSIZE:
     // get or set block size used, data is uint_least32_t*, number of bytes.
    return -1;
   break;
   case ROAR_VIO_CTL_GET_DBLOCKS:
   case ROAR_VIO_CTL_SET_DBLOCKS:
     // get or set number of blocks used, data is uint_least32_t*.
    return -1;
   break;
   case ROAR_VIO_CTL_SET_SSTREAM:
    // set server stream object for this stream, data is struct roar_stream_server*
    return -1;
   break;
   case ROAR_VIO_CTL_SET_SSTREAMID:
    // set stream ID for this stream, data is int*
    return -1;
   break;
   case ROAR_VIO_CTL_SET_VOLUME:
    // set volume for this device, data is struct roar_mixer_settings*
    return -1;
   break;
   case ROAR_VIO_CTL_GET_DELAY:
    // get delay of this stream, data is uint_least32_t*, in bytes
    // there is more about delay. please ask.
    return -1;
   break;
  default:
    return -1;
   break;
 }
}

int     driver_jack_close   (struct roar_vio_calls * vio) {
 struct driver_jack * self = vio->inst;
 // close and free everything in here...

 jack_client_close(self->client);

 roar_mm_free(self);

 return 0;
}

#endif

//ll
