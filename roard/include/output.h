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

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <roaraudio.h>

//#define ROAR_OUTPUT_BUFFER_SAMPLES 1024
//#define ROAR_OUTPUT_BUFFER_SAMPLES 1024
#ifdef DEBUG
// in case of debugging we use a big number of samples to make lager cycles
#define ROAR_OUTPUT_BUFFER_SAMPLES 2048
#else
//#define ROAR_OUTPUT_BUFFER_SAMPLES 441

// in normal case we use 100 cycles per sec, as we do not know the sample
// rate at compile time we guess it's normaly the default rate.
// on OpenBSD we need to set a lower freq, use 20 cycles per sec here as
// it seems to work.
// FIXME: find out what the problem is and how to fix
#ifdef ROAR_OS_OPENBSD
#define ROAR_OUTPUT_BUFFER_SAMPLES (ROAR_RATE_DEFAULT/20)
#else
#define ROAR_OUTPUT_BUFFER_SAMPLES (ROAR_RATE_DEFAULT/100)
#endif

#endif

#define ROAR_OUTPUT_WRITE_SIZE     1024

#define ROAR_OUTPUT_CALC_OUTBUFSIZE(x)   (ROAR_OUTPUT_BUFFER_SAMPLES * (x)->channels * ((x)->bits / 8) * ((float)(x)->rate/g_sa->rate))
#define ROAR_OUTPUT_CALC_OUTBUFSAMP(x,y) ((y) / ((x)->channels * ((x)->bits / 8)*((float)(x)->rate/g_sa->rate)))

void         * g_output_buffer;
unsigned int   g_output_buffer_len;

int output_buffer_init   (struct roar_audio_info * info);
int output_buffer_reinit (void);
int output_buffer_free   (void);

int output_buffer_flush  (DRIVER_USERDATA_T inst, int driver);

#endif

//ll
