//para.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from OpenBSD's sndio.
 *  See 'Copyright for sndio' below for more information on
 *  code fragments taken from OpenBSD's sndio.
 *
 * --- Copyright for sndio ---
 * Copyright (c) 2008 Alexandre Ratchov <alex@caoua.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * --- End of Copyright for sndio ---
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
 */

#define ROAR_USE_OWN_SNDIO_HDL
#include "libroarsndio.h"

//#ifndef DEBUG
//#define DEBUG
//#endif

void   sio_initpar(struct sio_par * par) {
 if ( par == NULL )
  return;

 memset(par, 0, sizeof(struct sio_par));

 par->bits  = ROAR_BITS_DEFAULT;
 par->sig   = 1;
 par->le    = SIO_LE_NATIVE;
 par->msb   = 1;
 par->rchan = 0;
 par->pchan = ROAR_CHANNELS_DEFAULT;
 par->rate  = ROAR_RATE_DEFAULT;
 par->bufsz = ROAR_BITS_DEFAULT * ROAR_CHANNELS_DEFAULT * ROAR_RATE_DEFAULT / 800.0;
 par->round = 1;
 par->xrun  = SIO_IGNORE;

 return;
}

int    sio_setpar (struct sio_hdl * hdl, struct sio_par * par) {
 if ( hdl == NULL || par == NULL ) {
  ROAR_DBG("sio_setpar(*): Invalid handle or parameter pointer");
  return 0;
 }

 if ( par->bits == 0 || par->bits > ROAR_BITS_MAX ) {
  ROAR_DBG("sio_setpar(*): Invalid number of bits: %i Bit", par->bits);
  return 0;
 }

 if ( par->bps == 0 )
  par->bps = SIO_BPS(par->bits);

 if ( par->bps > ROAR_BITS_MAX/8 ) {
  ROAR_DBG("sio_setpar(*): Invalid number of bytes: %i Byte", par->bps);
  return 0;
 }

 if ( SIO_BPS(par->bits) > par->bps ) {
  ROAR_DBG("sio_setpar(*): Number of bits/8 > number of bytes: %i/8 > %i", par->bits, par->bps);
  return 0;
 }

 hdl->info.bits = par->bps * 8;

 switch ((par->sig << 4) | par->le) {
  case 0x00:
    hdl->info.codec = ROAR_CODEC_PCM_U_BE;
   break;
  case 0x01:
    hdl->info.codec = ROAR_CODEC_PCM_U_LE;
   break;
  case 0x10:
    hdl->info.codec = ROAR_CODEC_PCM_S_BE;
   break;
  case 0x11:
    hdl->info.codec = ROAR_CODEC_PCM_S_LE;
   break;
  default:
    ROAR_DBG("sio_setpar(*): Invalid codec: sig=%i, le=%i", par->sig, par->le);
    return 0;
 }

 if ( par->msb == 0 ) {
  ROAR_DBG("sio_setpar(*): LSM alignment not supported");
  return 0;
 }

 if ( par->rchan != 0 ) { /* not supported yet */
  ROAR_DBG("sio_setpar(*): Recording not supported");
  return 0;
 }

 if ( par->pchan == 0 || par->pchan > ROAR_MAX_CHANNELS ) {
  ROAR_DBG("sio_setpar(*): Invalid number of playback channels: %i", par->pchan);
  return 0;
 }

 hdl->info.channels = par->pchan;

 if ( par->rate == 0 ) {
  ROAR_DBG("sio_setpar(*): Invalid sample rate: %iHz", par->rate);
  return 0;
 }

 hdl->info.rate = par->rate;

 if ( par->xrun != SIO_IGNORE ) {
  ROAR_DBG("sio_setpar(*): Unsupported xrun mode: %i", par->xrun);
  return 0;
 }

 memcpy(&(hdl->para), par, sizeof(struct sio_par));

 return 1;
}

int    sio_getpar (struct sio_hdl * hdl, struct sio_par * par) {
 if ( hdl == NULL || par == NULL )
  return 0;

 memcpy(par, &(hdl->para), sizeof(struct sio_par));

 return 1;
}

int    sio_getcap (struct sio_hdl * hdl, struct sio_cap * cap) {
 struct roar_stream s;
 unsigned int i;
 unsigned int bytes;
 unsigned int sign;
 unsigned int le;
 unsigned int mask = 0;

 if ( cap == NULL )
  return 0;

 if ( roar_server_oinfo(&(hdl->con), &s) == -1 )
  return 0;

 i = 0;
 for (bytes = 1; bytes <= s.info.bits/8; bytes++) {
  for (sign = 0; sign  < 2; sign++) {
   for (le  = 0; le    < 2; le++) {
    cap->enc[i].bits =   8*bytes;
    cap->enc[i].bps  =     bytes;
    cap->enc[i].sig  =     sign;
    cap->enc[i].le   =     le;
    cap->enc[i].msb  =   1;
    mask |= 1 << i;
    i++;
   }
  }
 }

 // TODO: fix this (at least include server channels,
 //                 do nit include confusing setups)
 if ( s.info.channels > SIO_NCHAN ) {
  s.info.channels = SIO_NCHAN;
 }

 for (i = 0; i < s.info.channels; i++) {
  cap->rchan[i] = i+1;
  cap->pchan[i] = i+1;
 }

 cap->rate[0] = s.info.rate;

 cap->nconf = 1;

 cap->confs[0].enc   = mask;
 cap->confs[0].rchan = mask;
 cap->confs[0].pchan = mask;
 cap->confs[0].rate  = 0x0001;

 return 1;
}


//ll
