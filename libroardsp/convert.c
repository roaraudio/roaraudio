//convert.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *      Copyright (C) Hans-Kristian 'maister' Arntzen - 2010
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

#include "libroardsp.h"
//#define free(p) {ROAR_WARN("free(%p) = ?", (p)); free((p)); ROAR_WARN("free(%p): OK", (p));}

int roar_conv_bits (void * out, void * in, int samples, int from, int to) {
 int format;

 if ( from == to ) {
  if ( in == out )
   return 0;

  memcpy(out, in, samples * from / 8);
  return 0;
 }

 format = ((from / 8) << 4) + (to / 8);

 switch (format) {
  case 0x12: return roar_conv_bits_8to16( out, in, samples);
  case 0x14: return roar_conv_bits_8to32( out, in, samples);
  case 0x21: return roar_conv_bits_16to8( out, in, samples);
  case 0x24: return roar_conv_bits_16to32(out, in, samples);
  case 0x34: return roar_conv_bits_24to32(out, in, samples);
  case 0x41: return roar_conv_bits_32to8( out, in, samples);
  case 0x42: return roar_conv_bits_32to16(out, in, samples);
  default:
   errno = ENOSYS;
   return -1;
 }

 return -1;
}

int roar_conv_bits_8to16 (void * out, void * in, int samples) {
 char    * ip = (char   *)in;
 int16_t * op = (int16_t*)out;
 int i;

 for (i = samples - 1; i >= 0; i--)
  op[i] = ip[i] << 8;

 return 0;
}

int roar_conv_bits_8to32  (void * out, void * in, int samples) {
 char    * ip = (char   *)in;
 int32_t * op = (int32_t*)out;
 int i;

 for (i = samples - 1; i >= 0; i--)
  op[i] = (int32_t) ip[i] << 24;

 return 0;
}

int roar_conv_bits_16to8 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*)in;
 char    * op = (char   *)out;
 int i;

 ROAR_DBG("roar_conv_bits_16to8(out=%p, in=%p, samples=%i) = ?", out, in, samples);

 for (i = 0; i < samples; i++)
  op[i] = ip[i] >> 8;

 return 0;
}

int roar_conv_bits_16to32 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*)in;
 int32_t * op = (int32_t*)out;
 int i;

 for (i = samples - 1; i >= 0; i--)
  op[i] = (int32_t) ip[i] << 16;

 return 0;
}

int roar_conv_bits_24to32 (void * out, void * in, int samples) {
 uint8_t * ip = (uint8_t*)in;
 int32_t * op = (int32_t*)out;
 int i;
 union {
  int32_t i;
  uint8_t c[4];
 } t;

 ROAR_DBG("roar_conv_bits_24to32(out=%p, in=%p, samples=%i) = ?", out, in, samples);

#if (BYTE_ORDER == BIG_ENDIAN) || (BYTE_ORDER == LITTLE_ENDIAN)
 t.i = 0;
#else
  ROAR_DBG("roar_conv_bits_24to32(out=%p, in=%p, samples=%i) = -1", out, in, samples);
  return -1;
#endif

 samples--;
 ip += 3 * samples;

 for (i = samples; i >= 0; i--) {
#if BYTE_ORDER == BIG_ENDIAN
  t.c[0] = *(ip--);
  t.c[1] = *(ip--);
  t.c[2] = *(ip--);
#elif BYTE_ORDER == LITTLE_ENDIAN
  t.c[2] = *(ip--);
  t.c[3] = *(ip--);
  t.c[1] = *(ip--);
#endif
  ROAR_DBG("roar_conv_bits_24to32(*): i=%i, t.i=0x%.8X", i, t.i);
  op[i] = t.i;
 }

 ROAR_DBG("roar_conv_bits_24to32(out=%p, in=%p, samples=%i) = 0", out, in, samples);
 return 0;
}

int roar_conv_bits_32to8 (void * out, void * in, int samples) {
 int32_t * ip = (int32_t*)in;
 char    * op = (char   *)out;
 int i;

 for (i = 0; i < samples; i++)
  op[i] = ip[i] >> 24;

 return 0;
}

int roar_conv_bits_32to16 (void * out, void * in, int samples) {
 int32_t * ip = (int32_t*)in;
 int16_t * op = (int16_t*)out;
 int i;

 for (i = 0; i < samples; i++)
  op[i] = ip[i] >> 16;

 return 0;
}

int roar_conv_chans (void * out, void * in, int samples, int from, int to, int bits) {
 if ( from == to ) {
  if ( in == out )
   return 0;

  memcpy(out, in, samples * from * bits / 8);
  return 0;
 }

 switch (bits) {
  case 8:
   switch (from) {
    case 1:
     switch (to) {
      case  2: return roar_conv_chans_1to28(out, in, samples);
      default: return roar_conv_chans_1ton8(out, in, samples, to);
     }
     break;
    case 2:
     switch (to) {
      case  1: return roar_conv_chans_2to18(out, in, samples);
      default: return -1;
     }
     break;
    default:
     switch (to) {
      case  1: return roar_conv_chans_nto18(out, in, samples, from);
      default: return -1;
     }
   }
  break;
  case 16:
   switch (from) {
    case 1:
     switch (to) {
      case  2: return roar_conv_chans_1to216(out, in, samples);
      default: return roar_conv_chans_1ton16(out, in, samples, to);
     }
     break;
    case 2:
     switch (to) {
      case  1: return roar_conv_chans_2to116(out, in, samples);
      case  3: return roar_conv_chans_2to316(out, in, samples);
      case  4: return roar_conv_chans_2to416(out, in, samples);
      case  5: return roar_conv_chans_2to516(out, in, samples);
      case  6: return roar_conv_chans_2to616(out, in, samples);
      default: return -1;
     }
     break;
    case 4:
     switch (to) {
      case  1: return roar_conv_chans_nto116(out, in, samples, 4);
      case  2: return roar_conv_chans_4to216(out, in, samples);
      default: return -1;
     }
     break;
    default:
     switch (to) {
      case  1: return roar_conv_chans_nto116(out, in, samples, from);
      default: return -1;
     }
   }
  break;
  case 32:
   switch (from) {
    case 1:
     switch (to) {
      default: return roar_conv_chans_1ton32(out, in, samples, to);
     }
     break;
    default:
     switch (to) {
      case  1: return roar_conv_chans_nto132(out, in, samples, from);
      default: return -1;
     }
   }
  break;
  default: return -1;
 }

 return -1;
}

int roar_conv_chans_1ton8  (void * out, void * in, int samples, int to) {
 char * ip = (char*) in, * op = (char*) out;
 int i;
 int c;

 for (i = samples - 1; i >= 0; i--)
  for (c = to - 1; c >= 0; c--)
   op[i*to + c] = ip[i];

 return 0;
}

int roar_conv_chans_1to28  (void * out, void * in, int samples) {
 char * ip = (char*) in, * op = (char*) out;
 int i, h;

 samples--;

 for (i = (h = samples) * 2; i >= 0; i -= 2, h--) {
  op[i + 0] = ip[h];
  op[i + 1] = ip[h];
 }

 return 0;
}

int roar_conv_chans_1ton16 (void * out, void * in, int samples, int to) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i;
 int c;

 for (i = samples - 1; i >= 0; i--)
  for (c = to - 1; c >= 0; c--)
   op[i*to + c] = ip[i];

 return 0;
}

int roar_conv_chans_1ton32 (void * out, void * in, int samples, int to) {
 int32_t * ip = (int32_t*) in, * op = (int32_t*) out;
 int i;
 int c;

 for (i = samples - 1; i >= 0; i--)
  for (c = to - 1; c >= 0; c--)
   op[i*to + c] = ip[i];

 return 0;
}

int roar_conv_chans_1to216 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 samples--;

 for (i = (h = samples) * 2; i >= 0; i -= 2, h--) {
  op[i + 0] = ip[h];
  op[i + 1] = ip[h];
 }

 return 0;
}

int roar_conv_chans_nto18  (void * out, void * in, int samples, int from) {
 int8_t * ip = (int8_t*) in, * op = (int8_t*) out;
 int i;
 int c;
 register int s;

 samples /= from;

 for (i = 0; i < samples; i++) {
  s  = 0;

  for (c = 0; c < from; c++)
   s += ip[i*from + c];

  s /= from;
  op[i] = s;
 }

 return 0;
}

int roar_conv_chans_2to18  (void * out, void * in, int samples) {
 int8_t * ip = (int8_t*) in, * op = (int8_t*) out;
 int i, h;

 for (h = i = 0; i < samples; i += 2, h++)
  op[h] = ((int)ip[i + 0] + (int)ip[i + 1]) / 2;

 return 0;
}

int roar_conv_chans_nto116 (void * out, void * in, int samples, int from) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i;
 int c;
 register int s;

 samples /= from;

 for (i = 0; i < samples; i++) {
  s  = 0;

  for (c = 0; c < from; c++)
   s += ip[i*from + c];

  s /= from;
  op[i] = s;
 }

 return 0;
}

int roar_conv_chans_nto132 (void * out, void * in, int samples, int from) {
 int32_t * ip = (int32_t*) in, * op = (int32_t*) out;
 int i;
 int c;
 register int s;

 samples /= from;

 for (i = 0; i < samples; i++) {
  s  = 0;

  for (c = 0; c < from; c++)
   s += ip[i*from + c];

  s /= from;
  op[i] = s;
 }

 return 0;
}

int roar_conv_chans_2to116  (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 ROAR_DBG("roar_conv_chans_2to116(out=%p, in=%p, samples=%i) = ?", out, in, samples);

 for (h = i = 0; i < samples; i += 2, h++) {
  ROAR_DBG("roar_conv_chans_2to116(out=%p, in=%p, samples=%i): op[%i] = (ip[%i] + ip[%i])/2", out, in, samples, h, i, i+1);
  op[h] = ((int)ip[i + 0] + (int)ip[i + 1]) / 2;
 }

 return 0;
}

int roar_conv_chans_2to38  (void * out, void * in, int samples);
int roar_conv_chans_2to316 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 samples -= 2;

 i  = samples;
 h  = (samples / 2) * 3;

 for (; i >= 0; i -= 2, h -= 3) {
  op[h+0] = ip[i+0];
  op[h+1] = ip[i+1];
  op[h+2] = ((int)ip[i + 0] + (int)ip[i + 1]) / 2;
 }

 return 0;
}

int roar_conv_chans_2to48  (void * out, void * in, int samples);
int roar_conv_chans_2to416 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 samples -= 2;

 i  = samples;
 h  = (samples / 2) * 4;

 for (; i >= 0; i -= 2, h -= 4) {
  op[h+0] = ip[i+0];
  op[h+1] = ip[i+1];
  op[h+2] = ip[i+0];
  op[h+3] = ip[i+1];
 }

 return 0;
}

int roar_conv_chans_2to58  (void * out, void * in, int samples);
int roar_conv_chans_2to516 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 samples -= 2;

 i  = samples;
 h  = (samples / 2) * 5;

 for (; i >= 0; i -= 2, h -= 5) {
  op[h+0] = ip[i+0];
  op[h+1] = ip[i+1];
  op[h+2] = ((int)ip[i + 0] + (int)ip[i + 1]) / 2;
  op[h+3] = ip[i+0];
  op[h+4] = ip[i+1];
 }

 return 0;
}
int roar_conv_chans_2to68  (void * out, void * in, int samples);
int roar_conv_chans_2to616 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 samples -= 2;

 i  = samples;
 h  = (samples / 2) * 6;

 for (; i >= 0; i -= 2, h -= 6) {
  op[h+0] = ip[i+0];
  op[h+1] = ip[i+1];
  op[h+2] = ((int)ip[i + 0] + (int)ip[i + 1]) / 2;
  op[h+3] = op[h+2];
  op[h+4] = ip[i+0];
  op[h+5] = ip[i+1];
 }

 return 0;
}

int roar_conv_chans_3to28  (void * out, void * in, int samples);
int roar_conv_chans_3to216 (void * out, void * in, int samples);
int roar_conv_chans_4to28  (void * out, void * in, int samples);
int roar_conv_chans_4to216 (void * out, void * in, int samples) {
 int16_t * ip = (int16_t*) in, * op = (int16_t*) out;
 int i, h;

 samples -= 4;

 for (i = h = 0; i < samples; i += 4, h += 2) {
  op[h+0] = ((int)ip[i + 0] + (int)ip[i + 2]) / 2;
  op[h+1] = ((int)ip[i + 1] + (int)ip[i + 3]) / 2;
 }

 return 0;
}

int roar_conv_chans_5to28  (void * out, void * in, int samples);
int roar_conv_chans_5to216 (void * out, void * in, int samples);
int roar_conv_chans_6to28  (void * out, void * in, int samples);
int roar_conv_chans_6to216 (void * out, void * in, int samples);



int roar_conv_rate (void * out, void * in, int samples, int from, int to, int bits, int channels) {
#ifdef ROAR_HAVE_LIBSAMPLERATE
 return roar_conv_rate_SRC(out, in, samples, from, to, bits, channels);
#else
 if ( bits == 8  )
  return roar_conv_rate_8(out, in, samples, from, to, channels);

 if ( bits == 16 )
  return roar_conv_rate_16(out, in, samples, from, to, channels);

 return -1;
#endif
}

int roar_conv_rate_8  (void * out, void * in, int samples, int from, int to, int channels) {
 return -1;
}

int roar_conv_rate_16 (void * out, void * in, int samples, int from, int to, int channels) {
 if ( from > to ) {
  switch (channels) {
   case 1:
     return roar_conv_rate_161zoh(out, in, samples, from, to);
   case 2:
     return roar_conv_rate_162zoh(out, in, samples, from, to);
   default:
     return -1;
  }
 } else {
  if ( channels == 1 ) {
   printf("roar_conv_rate_16(): samples=%i -> %i, rate=%i -> %i\n", samples*from/to, samples, from, to);
   return roar_conv_poly4_16s((int16_t*) out, (int16_t*) in, samples, samples*from/to, (float)from/to);
 //  return roar_conv_poly4_16((int16_t*) out, (int16_t*) in, samples*to/from, samples);
  }
 }

 return -1;
}

int roar_conv_rate_161zoh(void * out, void * in, int samples, int from, int to) {
 int16_t * ip = in;
 int16_t * op = out;
 float t = 0;
 float step = (float)to/from;
 int i;

 for (i= 0; i < samples; i++) {
  op[(int)t] = ip[i];
  t += step;
 }

 return 0;
}

int roar_conv_rate_162zoh(void * out, void * in, int samples, int from, int to) {
 int16_t * ip = in;
 int16_t * op = out;
 float t = 0;
 float step = (float)to/from;
 int i;

 ROAR_DBG("roar_conv_rate_162zoh(*): samples=%i", samples);
 samples /= 2;
// samples -= 1;
 ROAR_DBG("roar_conv_rate_162zoh(*): samples=%i", samples);

 for (i= 0; i < samples; i++) {
  ROAR_DBG("roar_conv_rate_162zoh(*): t=%f, i=%i // op[%i] = ip[%i]", t, i, 2*(int)t, 2*i);
  op[2*(int)t    ] = ip[2*i    ];
  op[2*(int)t + 1] = ip[2*i + 1];
  t += step;
 }

 return 0;
}

int roar_conv_rate_SRC   (void * out, void * in, int samples, int from, int to, int bits, int channels) {
#ifdef ROAR_HAVE_LIBSAMPLERATE
 double radio = (double) to / (double) from;
 int outsamples = radio * samples;
 float * inf  = roar_mm_malloc(samples*sizeof(float));
 float * outf = roar_mm_malloc(outsamples*sizeof(float));
 int i;
 SRC_DATA srcdata;

 ROAR_DBG("roar_conv_rate_SRC(*): radio=%lf, samples=%i, outsamples=%i", radio, samples, outsamples);

 if ( inf == NULL ) {
  if ( outf != NULL )
   roar_mm_free(outf);

  return -1;
 }

 if ( outf == NULL ) {
  if ( inf != NULL )
   roar_mm_free(inf);

  return -1;
 }

 switch (bits) {
  case  8:
    for (i = 0; i < samples; i++)
     inf[i] = *(((int8_t *)in)+i) / 128.0;
   break;
  case 16:
    for (i = 0; i < samples; i++)
     inf[i] = *(((int16_t*)in)+i) / 32768.0;
   break;
  case 32:
    for (i = 0; i < samples; i++)
     inf[i] = *(((int32_t*)in)+i) / 2147483648.0;
   break;
  default:
    roar_mm_free(outf);
    roar_mm_free(inf);
    return -1;
 }

 srcdata.data_in       = inf;
 srcdata.data_out      = outf;
 srcdata.input_frames  = samples/channels;
 srcdata.output_frames = outsamples/channels;
 srcdata.src_ratio     = radio;

 if ( src_simple(&srcdata, SRC_ZERO_ORDER_HOLD, channels) != 0 ) {
  roar_mm_free(outf);
  roar_mm_free(inf);
  return -1;
 }

 switch (bits) {
  case  8:
    for (i = 0; i < outsamples; i++)
     *(((int8_t *)out)+i) = outf[i] * 128.0;
   break;
  case 16:
    for (i = 0; i < outsamples; i++)
     *(((int16_t*)out)+i) = outf[i] * 32768.0;
   break;
  case 32:
    for (i = 0; i < outsamples; i++)
     *(((int32_t*)out)+i) = outf[i] * 2147483648.0;
   break;
   // no errors here, they are handled above
 }

 roar_mm_free(outf);
 roar_mm_free(inf);

 return 0;
#else
 return -1;
#endif
}

int roar_conv_rate2      (void * out, void * in, int outsamples, int samples, int bits, int channels) {
 ROAR_DBG("roar_conv_rate2(out=%p, in=%p, samples=%i, outsamples=%i, bits=%i, channels=%i) = ?", out, in, samples, outsamples, bits, channels);
 switch (bits) {
  case  8:
    return roar_conv_poly3_8(out, in, outsamples, samples, channels);
   break;
  case 16:
    return roar_conv_poly3_16(out, in, outsamples, samples, channels);
   break;
  case 32:
    return roar_conv_poly3_32(out, in, outsamples, samples, channels);
   break;
 }
 return -1;
}

int roar_conv_signedness  (void * out, void * in, int samples, int from, int to, int bits) {

 if ( from != to ) {
  if ( from && !to ) {
   switch (bits) {
    case  8: roar_conv_codec_s2u8( out, in, samples); break;
    case 16: roar_conv_codec_s2u16(out, in, samples); break;
    case 32: roar_conv_codec_s2u32(out, in, samples); break;
    default:
     errno = ENOSYS;
     return -1;
   }
  } else if ( !from && to ) {
   switch (bits) {
    case  8: roar_conv_codec_u2s8( out, in, samples); break;
    case 16: roar_conv_codec_u2s16(out, in, samples); break;
    case 32: roar_conv_codec_u2s32(out, in, samples); break;
    default:
     errno = ENOSYS;
     return -1;
   }
  } else {
   return -1;
  }
 } else {
  if ( out == in )
   return 0;

  memcpy(out, in, samples * bits / 8);
  return 0;
 }

 return 0;
}

int roar_conv_codec (void * out, void * in, int samples, int from, int to, int bits) {
 int inbo = ROAR_CODEC_BYTE_ORDER(from), outbo = ROAR_CODEC_BYTE_ORDER(to);
 int ins  = ROAR_CODEC_IS_SIGNED(from),  outs  = ROAR_CODEC_IS_SIGNED(to);
 void * nin = in;


 ROAR_DBG("roar_conv_codec(out=%p, in=%p, samples=%i, from=%i(%s), to=%i(%s), bits=%i) = ?",
              out, in, samples, from, roar_codec2str(from), to, roar_codec2str(to), bits);

 roar_conv_endian(out, in, samples, inbo, outbo, bits);
 nin = out;

 return roar_conv_signedness(out, in, samples, ins, outs, bits);
}

int roar_conv_codec_s2u8 (void * out, void * in, int samples) {
 char * ip = in;
 unsigned char * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] + 128;

 return 0;
}

int roar_conv_codec_s2u16 (void * out, void * in, int samples) {
 int16_t  * ip = in;
 uint16_t * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] + 32768;

 return 0;
}

int roar_conv_codec_s2u32 (void * out, void * in, int samples) {
 int32_t  * ip = in;
 uint32_t * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] + 2147483648U;

 return 0;
}

int roar_conv_codec_u2s8 (void * out, void * in, int samples) {
 unsigned char * ip = in;
          char * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] - 128;

 return 0;
}

int roar_conv_codec_u2s16 (void * out, void * in, int samples) {
 uint16_t  * ip = in;
 int16_t   * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] - 32768;

 return 0;
}

int roar_conv_codec_u2s32 (void * out, void * in, int samples) {
 uint32_t  * ip = in;
 int32_t   * op = out;
 int i;

 for(i = 0; i < samples; i++)
  op[i] = ip[i] - 2147483648U;

 return 0;
}


int roar_conv_endian      (void * out, void * in, int samples, int from, int to, int bits) {

 if ( bits == 8 ) {
  from = to = ROAR_CODEC_NATIVE_ENDIAN;

 } else if ( bits == 16 ) {
  if ( from  == ROAR_CODEC_PDP )
   from = ROAR_CODEC_LE;
  if ( to    == ROAR_CODEC_PDP )
   to   = ROAR_CODEC_LE;
 }

 ROAR_DBG("roar_conv_endian(out=%p, in=%p, samples=%i, from=%i, to=%i, bits=%i) = ?", out, in, samples, from, to, bits);

 if ( from == to ) {
  if ( in != out ) {
   memcpy(out, in, samples * bits / 8);
  }
  return 0;
 } else {
  switch (bits) {
   case 16:
     // in this case we can only have LE vs. BE, so, only need to swap:
     ROAR_DBG("roar_conv_endian(*): Doing 16 bit byteswap");
     return roar_conv_endian_16(out, in, samples);
    break;
   case 24:
     if ( (from == ROAR_CODEC_LE || from == ROAR_CODEC_BE) && (to == ROAR_CODEC_LE || to == ROAR_CODEC_BE) ) {
      return roar_conv_endian_24(out, in, samples);
     } else { // what the hell is PDP eddines in 24 bit mode?
      return -1;
     }
    break;
   case 32:
    if ( (from == ROAR_CODEC_LE || from == ROAR_CODEC_BE) && (to == ROAR_CODEC_LE || to == ROAR_CODEC_BE) ) {
     return roar_conv_endian_32(out, in, samples);
    } else { // need to handle 32 PDP eddines here?
     if ( from == ROAR_CODEC_BE )
      if ( roar_conv_endian_32(out, in, samples) == -1 )
       return -1;

     if ( roar_conv_endian_16(out, in, samples*2) == -1 )
      return -1;

     if ( to == ROAR_CODEC_BE )
      if ( roar_conv_endian_32(out, in, samples) == -1 )
       return -1;

     return 0;
    }
    break;
   default:
     return -1;
    break;
  }
 }

 return -1;
}


int roar_conv_endian_16   (void * out, void * in, int samples) {
 char          * ip = in;
 char          * op = out;
 register char   c;
 int             i;

 samples *= 2;

 if ( out != in ) {
//  printf("out != in\n");
  for(i = 0; i < samples; i += 2) {
//   printf("op[%i] = ip[%i]\nop[%i] = ip[%i]\n", i, i+1, i+1, i);
   op[i  ] = ip[i+1];
   op[i+1] = ip[i  ];
  }
 } else {
//  printf("out == in\n");
  for(i = 0; i < samples; i += 2) {
   c       = ip[i+1];
   op[i+1] = ip[i  ];
   op[i  ] = c;
  }
 }

 return 0;
}

int roar_conv_endian_24   (void * out, void * in, int samples) {
 char          * ip = in;
 char          * op = out;
 register char   c;
 int             i;

 samples *= 3;

 if ( out != in ) {
//  printf("out != in\n");
  for(i = 0; i < samples; i += 3) {
//   printf("op[%i] = ip[%i]\nop[%i] = ip[%i]\n", i, i+1, i+1, i);
   op[i  ] = ip[i+2];
   op[i+2] = ip[i  ];
  }
 } else {
//  printf("out == in\n");
  for(i = 0; i < samples; i += 3) {
   c       = ip[i+2];
   op[i+2] = ip[i  ];
   op[i  ] = c;
  }
 }

 return 0;
}

int roar_conv_endian_32   (void * out, void * in, int samples) {
 int32_t       * ip = in;
 int32_t       * op = out;
 union {
  int32_t val;
  char    data[4];
 }               c, h;
 int             i;

 // may the holly optimizer save our souls!

 ROAR_DBG("roar_conv_endian_32(out=%p, in=%p, samples=%i) = ?", out, in, samples);

 for (i = 0; i < samples; i++) {
  c.val     = ip[i];
  h.data[0] = c.data[3];
  h.data[1] = c.data[2];
  h.data[2] = c.data[1];
  h.data[3] = c.data[0];
  op[i]     = h.val;
 }

 return 0;
}

int roar_conv       (void * out, void * in, int samples, struct roar_audio_info * from, struct roar_audio_info * to) {
 void * ip = in;
 void * real_out = NULL;
 size_t from_size, to_size;

 // TODO: decide how to work around both in and out beeing to small to hold all
 //       data between the steps.
 //       for the moment: guess out >= in

 from_size = (from->bits * samples) / 8;
 to_size   = (  to->bits * samples * to->rate * to->channels) / (8 * from->rate * from->channels);

 ROAR_DBG("roar_conv(*): size: %i->%i", from_size, to_size);

 if ( to_size < from_size ) {
  real_out = out;

  if ( (out = roar_mm_malloc(from_size)) == NULL )
   return -1;

  ROAR_DBG("roar_conv(*): roar_mm_malloc(%i)=%p", (int)from_size, out);
 }

 ROAR_DBG("roar_conv(*): bo conv: %i->%i(native)", ROAR_CODEC_BYTE_ORDER(from->codec), ROAR_CODEC_NATIVE_ENDIAN);

 if ( ROAR_CODEC_BYTE_ORDER(from->codec) != ROAR_CODEC_NATIVE_ENDIAN ) {
  ROAR_DBG("roar_conv(*): doing bo input conv");
  if ( roar_conv_endian(out, ip, samples, ROAR_CODEC_BYTE_ORDER(from->codec), ROAR_CODEC_NATIVE_ENDIAN, from->bits) == -1 ) {
   if ( to_size < from_size )
    roar_mm_free(out);
   return -1;
  } else {
   ip = out;
  }
 }

 if ( from->bits != to->bits ) {
  if ( roar_conv_bits(out, ip, samples, from->bits, to->bits) == -1 ) {
   if ( to_size < from_size )
    roar_mm_free(out);
   return -1;
  } else {
   ip = out;
  }
 }

 if ( ROAR_CODEC_IS_SIGNED(from->codec) != ROAR_CODEC_IS_SIGNED(to->codec) ) {
  if ( roar_conv_signedness(out, ip, samples, ROAR_CODEC_IS_SIGNED(from->codec), ROAR_CODEC_IS_SIGNED(to->codec), to->bits) == -1 ) {
   if ( to_size < from_size )
    roar_mm_free(out);
   return -1;
  } else {
   ip = out;
  }
 }

/*
 if ( from->codec != to->codec ) {
  if ( roar_conv_codec (out, ip, samples, from->codec, to->codec, to->bits) == -1 )
   return -1;
  else
   ip = out;
 }
*/

 if ( from->rate != to->rate ) {
  if ( roar_conv_rate(out, ip, samples, from->rate, to->rate, to->bits, from->channels) == -1 ) {
   ROAR_DBG("roar_conv(*): failed to convert rate %i->%i (%ich%ibits)", from->rate, to->rate, to->bits, from->channels);
   if ( to_size < from_size )
    roar_mm_free(out);
   return -1;
  } else {
   ip = out;
   samples = (samples * to->rate) / from->rate;
  }
 }

 if ( from->channels != to->channels ) {
  if ( roar_conv_chans(out, ip, samples, from->channels, to->channels, to->bits) == -1 ) {
   if ( to_size < from_size )
    roar_mm_free(out);
   return -1;
  } else {
   ip = out;
  }
 }

 if ( ROAR_CODEC_BYTE_ORDER(to->codec) != ROAR_CODEC_NATIVE_ENDIAN ) {
  if ( roar_conv_endian(out, ip, samples, ROAR_CODEC_NATIVE_ENDIAN, ROAR_CODEC_BYTE_ORDER(to->codec), to->bits) == -1 ) {
   if ( to_size < from_size )
    roar_mm_free(out);
   return -1;
  } else {
   ip = out;
  }
 }

 if ( to_size < from_size ) {
  ROAR_DBG("roar_conv(*): memcpy(%p, %p, %i) = ?", real_out, out, (int)to_size);
  memcpy(real_out, out, to_size);
  roar_mm_free(out);
  ROAR_DBG("roar_conv(*): free(%p): OK!", out);
 }

 return 0;
}

int roar_conv2(void * out, void * in,
               size_t inlen,
               struct roar_audio_info * from, struct roar_audio_info * to,
               size_t bufsize) {
 size_t samples;
// size_t needed_buffer;
 void   * cin = in;
 struct roar_audio_info cinfo;
 int    need_signed = 0;
 size_t outsamples;

 memcpy(&cinfo, from, sizeof(cinfo));

 ROAR_DBG("roar_conv2(out=%p, in=%p, inlen=%lu, from=%p{...}, to=%p{...}, bufsize=%lu", out, in, inlen, from, to, bufsize);

/*
 if ( in != out ) {
  memset(out, 0xA0, bufsize);
 } else {
  ROAR_WARN("roar_conv2(*): in==out!");
  memset(out+inlen, 0xA0, bufsize-inlen);
 }
*/

 // calculate number of input samples:
 samples = (inlen * 8) / (from->bits);

 ROAR_DBG("roar_conv2(*): input samples: %i", samples);

#if 0
 // calculate size per frame
 needed_buffer  = ROAR_MAX(from->channels, to->channels) * ROAR_MAX(from->bits, to->bits) / 8;

 needed_buffer *= samples;
 needed_buffer /= from->channels;

 if ( from->rate < to->rate )
  needed_buffer *= (float)to->rate/(float)from->rate;

 ROAR_DBG("roar_conv2(*): needed_buffer=%u, bufsize=%u", needed_buffer, bufsize);

 // check if we have enogth RAM to convert
 if ( needed_buffer > bufsize )
  return -1;
#endif

 if ( from->rate != to->rate || from->channels != to->channels )
  need_signed = 1;

  ROAR_DBG("roar_conv2(*): need_signed=%i", need_signed);

 if ( ROAR_CODEC_BYTE_ORDER(from->codec) != ROAR_CODEC_NATIVE_ENDIAN ) {
  ROAR_DBG("roar_conv2(*): doing bo input conv");
  if ( roar_conv_endian(out, cin, samples,
       ROAR_CODEC_BYTE_ORDER(from->codec), ROAR_CODEC_NATIVE_ENDIAN, from->bits) == -1 ) {
   return -1;
  }
  cin = out;
 }

 if ( to->bits > from->bits ) {
  ROAR_DBG("roar_conv2(*): bits: %i->%i", from->bits, to->bits);
  if ( roar_conv_bits(out, cin, samples, from->bits, to->bits) == -1 )
   return -1;

  cin        = out;
  cinfo.bits = to->bits;
 }

 if ( need_signed && ! ROAR_CODEC_IS_SIGNED(from->codec) ) {
  ROAR_DBG("roar_conv2(*): sign: unsigned->signed");
  if ( roar_conv_signedness(out, cin, samples,
                            ROAR_CODEC_IS_SIGNED(from->codec), ROAR_CODEC_IS_SIGNED(to->codec),
                            cinfo.bits) == -1 )
   return -1;

  cin            = out;
  cinfo.codec    = ROAR_CODEC_PCM_S_LE; // just a signed PCM, which is of no intrest
 }

 if ( to->channels > from->channels ) {
  ROAR_DBG("roar_conv2(*): channels: %i->%i", from->channels, to->channels);
  if ( roar_conv_chans(out, cin, samples, from->channels, to->channels, cinfo.bits) == -1 )
   return -1;

  cin            = out;
  samples        = (samples * to->channels) / cinfo.channels;
  cinfo.channels = to->channels;
 }

//--//
 if ( from->rate != to->rate ) {
  outsamples = bufsize;

  ROAR_DBG("roar_conv2(*): outsamples=%llu", (long long unsigned int)outsamples);

  if ( cinfo.channels != to->channels ) {
   outsamples *= cinfo.channels;
   outsamples /= to->channels;
  }

  if ( cinfo.bits != to->bits ) {
   outsamples *= cinfo.bits;
   outsamples /= to->bits;
  }

  outsamples /= cinfo.bits/8;

  ROAR_DBG("roar_conv2(*): outsamples=%llu", (long long unsigned int)outsamples);

  if ( roar_conv_rate2(out, cin, outsamples, samples, cinfo.bits, cinfo.channels) == -1 )
   return -1;

  cin            = out;
  samples        = outsamples;
  cinfo.rate     = to->rate;
 }

 if ( cinfo.channels != to->channels ) {
  ROAR_DBG("roar_conv2(*): channels: %i->%i", cinfo.channels, to->channels);
  if ( roar_conv_chans(out, cin, samples, cinfo.channels, to->channels, cinfo.bits) == -1 )
   return -1;

  cin            = out;
  samples        = (samples * to->channels) / cinfo.channels;
  cinfo.channels = to->channels;
 }

 if ( ROAR_CODEC_IS_SIGNED(cinfo.codec) != ROAR_CODEC_IS_SIGNED(to->codec) ) {
  ROAR_DBG("roar_conv2(*): sign: ?(%i)->?(%i)", ROAR_CODEC_IS_SIGNED(cinfo.codec), ROAR_CODEC_IS_SIGNED(to->codec));
  if ( roar_conv_signedness(out, cin, samples,
                            ROAR_CODEC_IS_SIGNED(cinfo.codec), ROAR_CODEC_IS_SIGNED(to->codec),
                            cinfo.bits) == -1 )
   return -1;

  cin            = out;
  cinfo.codec    = to->codec;
 }

 if ( cinfo.bits != to->bits ) {
  ROAR_DBG("roar_conv2(*): bits: %i->%i", cinfo.bits, to->bits);
  if ( roar_conv_bits(out, cin, samples, cinfo.bits, to->bits) == -1 )
   return -1;

  cin        = out;
  cinfo.bits = to->bits;
 }

 if ( ROAR_CODEC_BYTE_ORDER(to->codec) != ROAR_CODEC_NATIVE_ENDIAN ) {
  ROAR_DBG("roar_conv2(*): doing bo output conv");
  if ( roar_conv_endian(out, cin, samples,
       ROAR_CODEC_NATIVE_ENDIAN, ROAR_CODEC_BYTE_ORDER(to->codec), to->bits) == -1 ) {
   return -1;
  }
  cin = out;
 }

 ROAR_DBG("roar_conv2(*): samples=%llu", (long long unsigned int)samples);

 ROAR_DBG("roar_conv2(*) = 0");
 return 0;
}

int roar_conv_poly4_16 (int16_t * out, int16_t * in, size_t olen, size_t ilen) {
 return roar_conv_poly4_16s(out, in, olen, ilen, (float)ilen/olen);
}

int roar_conv_poly4_16s (int16_t * out, int16_t * in, size_t olen, size_t ilen, float step) {
 float poly[4];
 float data[4];
 float t    = 0;
 int16_t * ci = in;
 int io, ii = 0;
 int i;

 printf("step=%f\n", step);

 // we can not make a poly4 with less than 4 points ;)
 if ( ilen < 4 )
  return -1;

 for (i = 0; i < 4; i++)
  data[i] = ci[i];
 roar_math_mkpoly_4x4(poly, data);
/*
 printf("new poly: data[4] = {%f, %f, %f, %f}, poly[4] = {%f, %f, %f, %f}\n",
         data[0], data[1], data[2], data[3],
         poly[0], poly[1], poly[2], poly[3]
       );
*/

 //0 1 2 3

 for (io = 0; io < olen; io++) {
//  printf("t=%f\n", t);
  out[io] = roar_math_cvpoly_4x4(poly, t);
  t += step;
  if ( t > 2 ) { // we need a new ploynome
 //  printf("t > 2, need new data\n");
   if ( (ii + 4) < ilen ) { // else: end of block.
    t -= 1;
//    printf("new data: ii=%i\n", ii);
    ii++;
    ci++;
    for (i = 0; i < 4; i++)
     data[i] = ci[i];
    roar_math_mkpoly_4x4(poly, data);
/* 
   printf("new poly: data[4] = {%f, %f, %f, %f}, poly[4] = {%f, %f, %f, %f}\n",
           data[0], data[1], data[2], data[3],
           poly[0], poly[1], poly[2], poly[3]
          );
*/
   }
  }
 }

 printf("io=%i\n", io);

 return 0;
}

//ll
