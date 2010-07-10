//libroarsndio.c:

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

static char * sndio_to_roar_names (char * name) {
 char * unitoffset = NULL;
 char * optsoffset = NULL;
 int    unit;

 if ( name == NULL )
  return NULL;

 if ( !strncmp(name, "sun:", 4) ) {
  unitoffset = name + 4;
 } else if ( !strncmp(name, "aucat:", 6) ) {
  unitoffset = name + 6;
 } else if ( !strncmp(name, "rmidi:", 6) ) {
  unitoffset = name + 6;
 } else if ( !strncmp(name, "midithru:", 9) ) {
  unitoffset = name + 9;
 } else {
  return name;
 }

 if ( (optsoffset = strstr(unitoffset, ".")) != NULL ) {
  // TODO: add some code to strip the options of the end
  return name;
 } else {
  unit = atoi(unitoffset);
  switch (unit) {
   // use defaulst
   case 0: return NULL; break;
   // use UNIX user sock, TODO: need some code here...
   case 1: return NULL; break;
   // use UNIX global sock:
   case 2: return ROAR_DEFAULT_SOCK_GLOBAL; break;
   // use DECnet localnode:
   case 3: return "::"; break;
   // use IPv4 localhost:
   case 4: return ROAR_DEFAULT_INET4_HOST; break;
   // reserved for DECnet Phase V:
   case 5: return name; break;
   // use IPv6 localhost:
   case 6: return ROAR_DEFAULT_INET6_HOST; break;
   // default:
   default:
     return name;
  }
 }

 return name;
}

struct sio_hdl * sio_open(const char * name, unsigned mode, int nbio_flag) {
 struct sio_hdl * hdl = NULL;
 int is_midi = 0;

 if ( (hdl = roar_mm_malloc(sizeof(struct sio_hdl))) == NULL )
  return NULL;

 memset(hdl, 0, sizeof(struct sio_hdl));

 switch (mode) {
  case SIO_PLAY:
    hdl->dir = ROAR_DIR_PLAY;
   break;
  case MIO_OUT:
    is_midi = 1;
    hdl->dir = ROAR_DIR_MIDI_IN;
   break;
  case MIO_IN:
    is_midi = 1;
    hdl->dir = ROAR_DIR_MIDI_OUT;
   break;

  // unsupported:
  case SIO_REC:
  case SIO_PLAY|SIO_REC:
  case MIO_OUT|MIO_IN:

  // illigal:
  default:
    roar_mm_free(hdl);
    return NULL;
 }

 if ( name == NULL ) {
  if ( is_midi ) {
   name = getenv("MIDIDEVICE");
  } else {
   name = getenv("AUDIODEVICE");
  }
 }

 name = sndio_to_roar_names((char*) name);

 if ( roar_simple_connect(&(hdl->con), (char*) name, "libroarsndio") == -1 ) {
  roar_mm_free(hdl);
  return NULL;
 }

 sio_initpar(&(hdl->para));

 hdl->stream_opened = 0;

 if ( name != NULL )
  hdl->device = strdup(name);

 if ( is_midi ) {
  hdl->info.codec    = ROAR_CODEC_MIDI;
  hdl->info.bits     = ROAR_MIDI_BITS;
  hdl->info.channels = ROAR_MIDI_CHANNELS_DEFAULT;
  hdl->info.rate     = ROAR_MIDI_TICKS_PER_BEAT;
  if ( !sio_start(hdl) ) {
   sio_close(hdl);
   return NULL;
  }
 }

 hdl->nonblock = nbio_flag;

 return hdl;
}

void   sio_close  (struct sio_hdl * hdl) {
 if ( hdl == NULL )
  return;

 sio_stop(hdl);

 roar_disconnect(&(hdl->con));

 roar_mm_free(hdl);
}

int    sio_eof    (struct sio_hdl * hdl) {
 return hdl->ioerror;
}

void   sio_onmove (struct sio_hdl * hdl, void (*cb)(void * arg, int delta), void * arg) {
 if ( hdl == NULL )
  return;

 hdl->on_move     = cb;
 hdl->on_move_arg = arg;
}

//ll
