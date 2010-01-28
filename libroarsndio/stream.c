//stream.c:

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

// this is void because we do not care about if this failed...
static void send_vol_event (struct sio_hdl * hdl) {
 int channels;
 struct roar_mixer_settings mixer;
 int i;
 unsigned vol;

 // in case of no event handler we do not send the event at all ;)
 if ( hdl->on_vol == NULL )
  return;

 if ( roar_get_vol(&(hdl->con), roar_stream_get_id(&(hdl->stream)), &mixer, &channels) == -1 )
  return;

 ROAR_DBG("send_vol_event(*): channels=%i", channels);

 switch (channels) {
  case 1:
    ROAR_DBG("send_vol_event(*): mixer.scale=%i, mixer.mixer={%i,...}", mixer.scale, mixer.mixer[0]);
    vol  = mixer.mixer[0] * SIO_MAXVOL;
    vol /= mixer.scale;
    ROAR_DBG("send_vol_event(*): vol=%u", vol);
   break;
  case 2:
    vol = (mixer.mixer[0] + mixer.mixer[1]) * SIO_MAXVOL / mixer.scale / 2;
   break;
  default:
    vol = 0;
    for (i = 0; i < channels; i++)
     vol += mixer.mixer[i];

    vol /= channels;
    vol *= SIO_MAXVOL;
    vol /= mixer.scale;
   break;
 }

 ROAR_DBG("send_vol_event(*): vol=%u", vol);
 hdl->on_vol(hdl->on_vol_arg, vol);
}

#define _i(x) (hdl->info.x)
int    sio_start  (struct sio_hdl * hdl) {

 // TODO: FIXME: use full VIO support here, not fh->vio!

 if ( hdl == NULL )
  return 0;

 if ( hdl->stream_opened )
  return 0;

 if ( roar_vio_simple_new_stream_obj(&(hdl->svio), &(hdl->con), &(hdl->stream),
                                     _i(rate), _i(channels), _i(bits), _i(codec), hdl->dir) == -1 )
  return 0;

 ROAR_DBG("sio_start(hdl=%p): rate=%i, channels=%i, bits=%i, codec=%i", hdl, _i(rate), _i(channels), _i(bits), _i(codec));

 if ( hdl->nonblock ) {
  if ( roar_vio_nonblock(&(hdl->svio), ROAR_SOCKET_NONBLOCK) == -1 ) {
   roar_vio_close(&(hdl->svio));
   return 0;
  }
 }

 send_vol_event(hdl);

 hdl->stream_opened = 1;
 hdl->ioerror       = 0;

 return 1;
}
#undef _i

int    sio_stop   (struct sio_hdl * hdl) {

 if ( hdl == NULL )
  return 0;

 if ( !hdl->stream_opened )
  return 0;

 roar_vio_close(&(hdl->svio));

 hdl->stream_opened = -1;

 return 1;
}

size_t sio_read   (struct sio_hdl * hdl, void * addr, size_t nbytes) {
 ssize_t ret;

 if ( hdl == NULL )
  return 0;

 if ( !hdl->stream_opened )
  return 0;

 if ( (ret = roar_vio_read(&(hdl->svio), addr, nbytes)) < 0 ) {
  hdl->ioerror = 1;
  return 0;
 }

 if ( hdl->nonblock )
  hdl->ioerror = 0;

 return ret;
}
size_t sio_write  (struct sio_hdl * hdl, const void * addr, size_t nbytes) {
 ssize_t ret;

 if ( hdl == NULL )
  return 0;

 if ( !hdl->stream_opened )
  return 0;

 if ( (ret = roar_vio_write(&(hdl->svio), (void*) addr, nbytes)) < 0 ) {
  hdl->ioerror = 1;
  return 0;
 }

 if ( hdl->nonblock )
  hdl->ioerror = 0;

 if ( hdl->on_move != NULL ) {
  hdl->on_move(hdl->on_move_arg, 8*ret/(hdl->info.channels * hdl->info.bits));
 }

 return ret;
}

//ll
