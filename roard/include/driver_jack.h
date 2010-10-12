//driver_jack.h:

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

#ifndef _DRIVER_JACK_H_
#define _DRIVER_JACK_H_

struct driver_jack {
 jack_client_t * client;
};

int driver_jack_open_vio  (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);

ssize_t driver_jack_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t driver_jack_write   (struct roar_vio_calls * vio, void *buf, size_t count);
int     driver_jack_nonblock(struct roar_vio_calls * vio, int state);
int     driver_jack_sync    (struct roar_vio_calls * vio);
int     driver_jack_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     driver_jack_close   (struct roar_vio_calls * vio);

#endif

//ll
