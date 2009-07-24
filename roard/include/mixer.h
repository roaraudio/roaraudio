//output.h:

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

#ifndef _MIXER_H_
#define _MIXER_H_

#include <roaraudio.h>

int mix_clients       (void * output, int bits, void ** input, int samples);
int mix_clients_8bit  (void * output, void ** input, int samples);
int mix_clients_16bit (void * output, void ** input, int samples);
int mix_clients_24bit (void * output, void ** input, int samples);
int mix_clients_32bit (void * output, void ** input, int samples);

int change_vol       (void * output, int bits, void * input, int samples, int channels, struct roar_mixer_settings * set);
int change_vol_8bit  (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set);
int change_vol_16bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set);
int change_vol_24bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set);
int change_vol_32bit (void * output, void * input, int samples, int channels, struct roar_mixer_settings * set);

int need_vol_change  (int channels, struct roar_mixer_settings * set);

#endif

//ll
