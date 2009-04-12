//driver_sndio.h:

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

#ifndef _DRIVER_SNDIO_H_
#define _DRIVER_SNDIO_H_

struct driver_sndio {
 char * device;
 struct sio_hdl * handle;
 struct roar_audio_info info;
 int need_reopen;
 int need_config;
 struct roar_stream_server * stream;
 int ssid;
};

int     driver_sndio_open         (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh);
int     driver_sndio_close_vio    (struct roar_vio_calls * vio);
int     driver_sndio_open_device  (struct driver_sndio * self) {
int     driver_sndio_config_device(struct driver_sndio * self) {
int     driver_sndio_reopen_device(struct driver_sndio * self);
ssize_t driver_sndio_write        (struct roar_vio_calls * vio, void *buf, size_t count);
int     driver_sndio_sync         (struct roar_vio_calls * vio);
int     driver_sndio_ctl          (struct roar_vio_calls * vio, int cmd, void * data);


#endif

//ll
