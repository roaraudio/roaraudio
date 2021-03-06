//rms.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARDSP_RMS_H_
#define _LIBROARDSP_RMS_H_

#include "libroardsp.h"

int64_t roar_rms2_1_8  (int8_t  * data, size_t samples);
int64_t roar_rms2_1_16 (int16_t * data, size_t samples);
int64_t roar_rms2_1_32 (int32_t * data, size_t samples);

int roar_rms2_1_8_2    (int8_t  * data, size_t samples, int64_t * rms);
int roar_rms2_1_16_2   (int16_t * data, size_t samples, int64_t * rms);
int roar_rms2_1_32_2   (int32_t * data, size_t samples, int64_t * rms);

int roar_rms2_1_8_n    (int8_t  * data, size_t samples, int64_t * rms, size_t n);
int roar_rms2_1_16_n   (int16_t * data, size_t samples, int64_t * rms, size_t n);
int roar_rms2_1_32_n   (int32_t * data, size_t samples, int64_t * rms, size_t n);

int roar_rms2_1_b_n    (void    * data, size_t samples, int64_t * rms, size_t n, size_t bits);

#endif

//ll
