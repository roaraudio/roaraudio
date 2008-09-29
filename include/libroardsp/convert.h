//convert.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

#ifndef _CONVERT_H_
#define _CONVERT_H_

#include "libroar.h"

int roar_conv_bits       (void * out, void * in, int samples, int from, int to);

                                                                 // implemnted?
int roar_conv_bits_8to16  (void * out, void * in, int samples);  // yes
int roar_conv_bits_8to24  (void * out, void * in, int samples);
int roar_conv_bits_8to32  (void * out, void * in, int samples);  // yes
int roar_conv_bits_16to8  (void * out, void * in, int samples);  // yes
int roar_conv_bits_16to24 (void * out, void * in, int samples);
int roar_conv_bits_16to32 (void * out, void * in, int samples);  // yes
int roar_conv_bits_24to8  (void * out, void * in, int samples);
int roar_conv_bits_24to16 (void * out, void * in, int samples);
int roar_conv_bits_24to32 (void * out, void * in, int samples);
int roar_conv_bits_32to8  (void * out, void * in, int samples);  // yes
int roar_conv_bits_32to16 (void * out, void * in, int samples);  // yes
int roar_conv_bits_32to24 (void * out, void * in, int samples);

int roar_conv_chans (void * out, void * in, int samples, int from, int to, int bits);

                                                                           // implmented?  working
int roar_conv_chans_1ton8  (void * out, void * in, int samples, int to);   // yes          ?
int roar_conv_chans_1ton16 (void * out, void * in, int samples, int to);   // yes          yes
int roar_conv_chans_1to28  (void * out, void * in, int samples);           // yes          yes
int roar_conv_chans_1to216 (void * out, void * in, int samples);           // yes          yes
int roar_conv_chans_nto18  (void * out, void * in, int samples, int from); // yes          ?
int roar_conv_chans_nto116 (void * out, void * in, int samples, int from); // yes          ?
int roar_conv_chans_2to18  (void * out, void * in, int samples);           // yes          yes
int roar_conv_chans_2to116 (void * out, void * in, int samples);           // yes          yes

int roar_conv_rate    (void * out, void * in, int samples, int from, int to, int bits, int channels);
int roar_conv_rate_8  (void * out, void * in, int samples, int from, int to, int channels);
int roar_conv_rate_16 (void * out, void * in, int samples, int from, int to, int channels);

int raor_conv_codec (void * out, void * in, int samples, int from, int to, int bits);

int roar_conv_codec_s2u8  (void * out, void * in, int samples);
int roar_conv_codec_s2u16 (void * out, void * in, int samples);
int roar_conv_codec_s2u32 (void * out, void * in, int samples);
int roar_conv_codec_u2s8  (void * out, void * in, int samples);
int roar_conv_codec_u2s16 (void * out, void * in, int samples);
int roar_conv_codec_u2s32 (void * out, void * in, int samples);

int roar_conv_endian_16   (void * out, void * in, int samples);

int roar_conv       (void * out, void * in, int samples, struct roar_audio_info * from, struct roar_audio_info * to);


int roar_conv_poly4_16  (int16_t * out, int16_t * in, size_t olen, size_t ilen);
int roar_conv_poly4_16s (int16_t * out, int16_t * in, size_t olen, size_t ilen, float step);

#endif

//ll
