//driver_pwmled.h:

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

#ifndef _DRIVER_PWMLED_H_
#define _DRIVER_PWMLED_H_

struct driver_pwmled {
 struct roar_vio_calls  vio;
 struct roar_lpwm_state state;
 unsigned int           rate;    // bit/s
 uint16_t               channel; // DMX Channel
};

int driver_pwmled_open_vio  (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh);
ssize_t driver_pwmled_write (struct roar_vio_calls * vio,  void *buf, size_t count);
int     driver_pwmled_ctl   (struct roar_vio_calls * vio,  int cmd, void * data);
int     driver_pwmled_close (struct roar_vio_calls * vio);

#endif

//ll