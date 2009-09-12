//vio_transcode.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of libroardsp a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroardsp is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "libroardsp.h"

int     roar_vio_open_xcode    (struct roar_vio_calls * calls, int encoder, struct roar_audio_info * info,
                                struct roar_vio_calls * dst) {
 struct roar_xcoder * xcoder = malloc(sizeof(struct roar_xcoder));

 if ( xcoder == NULL )
  return -1;

 if ( calls == NULL || info == NULL || dst == NULL ) {
  free(xcoder);
  return -1;
 }

 if ( roar_xcoder_init(xcoder, encoder, info, dst) == -1 ) {
  free(xcoder);
  return -1;
 }

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst   = (void*)xcoder;

 calls->close  = roar_vio_xcode_close;

 if ( encoder ) {
  calls->write = roar_vio_xcode_proc;
 } else {
  calls->read  = roar_vio_xcode_proc;
 }

 return 0;
}

ssize_t roar_vio_xcode_proc    (struct roar_vio_calls * vio, void *buf, size_t count) {
#ifdef DEBUG
 int ret;
#endif

 if ( vio == NULL )
  return -1;

 if ( buf == NULL && count != 0 )
  return -1;

#ifdef DEBUG
 ret = roar_xcoder_proc(vio->inst, buf, count);

 ROAR_DBG("roar_vio_xcode_proc(vio=%p, buf=%p, count=%lu): ret=%i", vio, buf, (unsigned long) count, ret);

 return !ret ? count : -1;
#else
 return !roar_xcoder_proc(vio->inst, buf, count) ? count : -1;
#endif
}

off_t   roar_vio_xcode_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_xcode_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_xcode_sync    (struct roar_vio_calls * vio);
int     roar_vio_xcode_ctl     (struct roar_vio_calls * vio, int cmd, void * data);

int     roar_vio_xcode_close   (struct roar_vio_calls * vio) {
 int ret = 0;

 if ( vio == NULL )
  return -1;

 if ( roar_xcoder_close(vio->inst) == -1 )
  ret = -1;

 if ( vio->inst != NULL )
  free(vio->inst);

 return ret;
}

int     roar_vio_open_bixcode    (struct roar_vio_calls * calls, struct roar_audio_info * info,
                                  struct roar_vio_calls * dst) {
 struct roar_bixcoder * bixcoder = malloc(sizeof(struct roar_bixcoder));

 if ( bixcoder == NULL )
  return -1;

 if ( calls == NULL || info == NULL || dst == NULL ) {
  free(bixcoder);
  return -1;
 }

 if ( roar_bixcoder_init(bixcoder, info, dst) == -1 ) {
  free(bixcoder);
  return -1;
 }

 memset(calls, 0, sizeof(struct roar_vio_calls));

 calls->inst   = (void*)bixcoder;

 calls->close  = roar_vio_bixcode_close;

 calls->read  = roar_vio_bixcode_read;
 calls->write = roar_vio_bixcode_write;

 return 0;
}

ssize_t roar_vio_bixcode_read    (struct roar_vio_calls * vio, void *buf, size_t count) {

 if ( vio == NULL )
  return -1;

 if ( buf == NULL && count != 0 )
  return -1;

 return !roar_bixcoder_read(vio->inst, buf, count) ? count : -1;
}

ssize_t roar_vio_bixcode_write   (struct roar_vio_calls * vio, void *buf, size_t count) {

 if ( vio == NULL )
  return -1;

 if ( buf == NULL && count != 0 )
  return -1;

 return !roar_bixcoder_write(vio->inst, buf, count) ? count : -1;
}

off_t   roar_vio_bixcode_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     roar_vio_bixcode_nonblock(struct roar_vio_calls * vio, int state);
int     roar_vio_bixcode_sync    (struct roar_vio_calls * vio);
int     roar_vio_bixcode_ctl     (struct roar_vio_calls * vio, int cmd, void * data);

int     roar_vio_bixcode_close   (struct roar_vio_calls * vio) {
 int ret = 0;

 if ( vio == NULL )
  return -1;

 if ( roar_bixcoder_close(vio->inst) == -1 )
  ret = -1;

 if ( vio->inst != NULL )
  free(vio->inst);

 return ret;
}

//ll
