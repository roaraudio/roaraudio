//esdstream.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from EsounD.
 *  They are mostly copyrighted by Eric B. Mitchell (aka 'Ricdude)
 *  <ericmit@ix.netcom.com>. For more information see AUTHORS.esd.
 *
 *  This file is part of libroaresd a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroaresd.h"

#if BYTE_ORDER == BIG_ENDIAN
#define CODEC_DEF_8BIT ROAR_CODEC_PCM_U_BE
#elif BYTE_ORDER == LITTLE_ENDIAN
#define CODEC_DEF_8BIT ROAR_CODEC_PCM_U_LE
#else
#define CODEC_DEF_8BIT ROAR_CODEC_PCM_U_PDP
#endif

/* open a socket for playing, monitoring, or recording as a stream */
/* the *_fallback functions try to open /dev/dsp if there's no EsounD */
int esd_play_stream( esd_format_t format, int rate,
                     const char *host, const char *name ) {
 int channels;
 int bits;
 int codec = ROAR_CODEC_DEFAULT;

 if ( (format & ESD_BITS16) ) {
  bits  = 16;
 } else {
  bits  = 8;
  codec = CODEC_DEF_8BIT;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_play(rate, channels, bits, codec, (char*)host, (char*) name);
}

int esd_play_stream_fallback( esd_format_t format, int rate,
                              const char *host, const char *name ) {
 int r;

 if ( (r = esd_play_stream(format, rate, host, name)) != -1 ) {
  return r;
 }

 return esd_play_stream(format, rate, "+fork", name);
}



int esd_monitor_stream( esd_format_t format, int rate,
                        const char *host, const char *name ) {
 int channels;
 int bits;
 int codec = ROAR_CODEC_DEFAULT;

 ROAR_DBG("esd_monitor_stream(format=%x, rate=%i, host='%s', name='%s') = ?", format, rate, host, name);

 if ( (format & ESD_BITS16) ) {
  bits  = 16;
 } else {
  bits  = 8;
  codec = CODEC_DEF_8BIT;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_monitor(rate, channels, bits, codec, (char*)host, (char*)name);
}
/* int esd_monitor_stream_fallback( esd_format_t format, int rate ); */
int esd_record_stream( esd_format_t format, int rate,
                       const char *host, const char *name ) {
 int channels;
 int bits;
 int codec = ROAR_CODEC_DEFAULT;

 ROAR_DBG("esd_record_stream(format=%x, rate=%i, host='%s', name='%s') = ?", format, rate, host, name);

 if ( (format & ESD_BITS16) ) {
  bits  = 16;
 } else {
  bits  = 8;
  codec = CODEC_DEF_8BIT;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_record(rate, channels, bits, codec, (char*)host, (char*)name);
}
int esd_record_stream_fallback( esd_format_t format, int rate,
                                const char *host, const char *name ) {
 int r;

 if ( (r = esd_record_stream(format, rate, host, name)) != -1 ) {
  return r;
 }

 return esd_record_stream(format, rate, "+fork", name);
}
int esd_filter_stream( esd_format_t format, int rate,
                       const char *host, const char *name ) {
 int channels;
 int bits;
 int codec = ROAR_CODEC_DEFAULT;

 if ( (format & ESD_BITS16) ) {
  bits  = 16;
 } else {
  bits  = 8;
  codec = CODEC_DEF_8BIT;
 }

 if ( (format & ESD_MONO) ) {
  channels = 1;
 } else {
  channels = 2;
 }

 return roar_simple_filter(rate, channels, bits, codec, (char*)host, (char*)name);
}


//ll
