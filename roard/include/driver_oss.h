//driver_oss.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#ifndef _DRIVER_OSS_H_
#define _DRIVER_OSS_H_

struct driver_oss {
 char * device;
 int fh;
 int blocks;
 int blocksize;
 struct roar_audio_info info;
 int need_config;
 struct roar_stream_server * stream;
 int ssid;
 int fh_savemode;
};

int driver_oss_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);
int driver_oss_close(DRIVER_USERDATA_T   inst);
int driver_oss_sync(struct roar_vio_calls * vio);
int driver_oss_ctl(struct roar_vio_calls * vio, int cmd, void * data);

ssize_t driver_oss_write    (struct roar_vio_calls * vio, void *buf, size_t count);
int     driver_oss_nonblock (struct roar_vio_calls * vio, int state);
int     driver_oss_close_vio(struct roar_vio_calls * vio);
int     driver_oss_reopen_device(struct driver_oss * self);

#endif

//ll
