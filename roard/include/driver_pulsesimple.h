//driver_pulsesimple.h:

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

#ifndef _DRIVER_PULSESIMPLE_H_
#define _DRIVER_PULSESIMPLE_H_

struct driver_pulsesimple {
 pa_simple * handle;
/*
 int    dir;
 char * device;
 struct roar_stream_server * stream;
 int ssid;
*/
};

int     driver_pulsesimple_open         (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);
int     driver_pulsesimple_close        (struct roar_vio_calls * vio);
ssize_t driver_pulsesimple_write        (struct roar_vio_calls * vio, void *buf, size_t count);
int     driver_pulsesimple_sync         (struct roar_vio_calls * vio);
int     driver_pulsesimple_ctl          (struct roar_vio_calls * vio, int cmd, void * data);


#endif

//ll
