//libroarsndio.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 */

#define ROAR_USE_OWN_SNDIO_HDL
#include "libroarsndio.h"

struct sio_hdl * sio_open(char * name, unsigned mode, int nbio_flag) {
 struct sio_hdl * hdl = NULL;
 int is_midi = 0;

 if ( (hdl = malloc(sizeof(struct sio_hdl))) == NULL )
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
  case SIO_REC:
  case SIO_PLAY|SIO_REC:
  case MIO_OUT|MIO_IN:
  default:
    free(hdl);
    return NULL;
 }

 if ( roar_simple_connect(&(hdl->con), name, "libroarsndio") == -1 ) {
  free(hdl);
  return NULL;
 }

 sio_initpar(&(hdl->para));

 hdl->stream_opened = 0;

 if ( name != NULL )
  hdl->device = strdup(name);

 if ( is_midi ) {
  hdl->info.codec    = ROAR_CODEC_MIDI;
  hdl->info.bits     = 8;
  hdl->info.channels = 16;
  hdl->info.rate     = 96;
  if ( !sio_start(hdl) ) {
   sio_close(hdl);
   return NULL;
  }
 }

 return hdl;
}

void   sio_close  (struct sio_hdl * hdl) {
 if ( hdl == NULL )
  return;

 sio_stop(hdl);

 roar_disconnect(&(hdl->con));

 free(hdl);
}

int    sio_eof    (struct sio_hdl * hdl) {
 return 0;
}

void   sio_onmove (struct sio_hdl * hdl, void (*cb)(void * arg, int delta), void * arg) {
 if ( hdl == NULL )
  return;

 hdl->on_move     = cb;
 hdl->on_move_arg = arg;
}

//ll
