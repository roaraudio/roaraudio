//driver_sndio.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#include "roard.h"
#ifdef ROAR_HAVE_LIBSNDIO

int driver_sndio_init_vio(struct roar_vio_calls * vio, struct driver_sndio * inst) {
 if ( vio == NULL )
  return -1;

 memset(vio, 0, sizeof(struct roar_vio_calls));

 vio->close    = driver_sndio_close_vio;
 vio->write    = driver_sndio_write;
 vio->sync     = driver_sndio_sync;
 vio->ctl      = driver_sndio_ctl;

 vio->inst     = (void*) inst;

 return 0;
}

#define er() if ( self->handle ) sio_close(self->handle); if ( self->device ) free(self->device); free(self); return -1
int driver_sndio_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 struct driver_sndio * self = NULL;

 if ( (self = malloc(sizeof(struct driver_sndio))) == NULL ) {
  ROAR_ERR("driver_sndio_open(*): Can not malloc() instance data: %s", strerror(errno));
  return -1;
 }

 memset(self, 0, sizeof(struct driver_sndio));
 memcpy(&(self->info), info, sizeof(struct roar_audio_info));

 self->ssid = -1;

 if ( device != NULL )
  self->device = strdup(device);

 if ( driver_sndio_init_vio(inst, self) == -1 ) {
  ROAR_ERR("driver_sndio_open(*): Can not init vio interface");
  er();
 }

 if ( driver_sndio_open_device(self) == -1 ) {
  ROAR_ERR("driver_sndio_open(*): Can not open audio device");
  if ( self->handle )
   sio_close(self->handle);

  if ( self->device )
   free(self->device);

  free(self);

  return -1;
//  er();
 }

 ROAR_DBG("driver_sndio_open(*): OSS devices opened :)");

 return 0;
}
#undef er

int     driver_sndio_close_vio    (struct roar_vio_calls * vio) {
 struct driver_sndio * self = vio->inst;

 if ( self->handle != NULL )
  sio_close(self->handle);

 if ( self->device != NULL )
  free(self->device);

 free(self);

 return 0;
}

int     driver_sndio_open_device  (struct driver_sndio * self) {

 if ( (self->handle = sio_open(self->device, SIO_PLAY, 0)) == NULL ) {
  ROAR_ERR("driver_sndio_open_device(*): Can not open sndio audio device");
  return -1;
 }

 self->need_config = 1;

 return 0;
}

int     driver_sndio_config_device(struct driver_sndio * self) {
 struct sio_par par;

 sio_initpar(&par);

 par.bits  = self->info.bits;
 par.rate  = self->info.rate;
 par.pchan = self->info.channels;

 switch (self->info.codec) {
  case ROAR_CODEC_PCM_S_LE:
    par.le  = 1;
    par.sig = 1;
   break;
  case ROAR_CODEC_PCM_S_BE:
    par.le  = 0;
    par.sig = 1;
   break;
  case ROAR_CODEC_PCM_U_LE:
    par.le  = 1;
    par.sig = 0;
   break;
  case ROAR_CODEC_PCM_U_BE:
    par.le  = 0;
    par.sig = 0;
   break;
  default:
    return -1;
   break;
 }

 if ( sio_setpar(self->handle, &par) == 0 ) {
  ROAR_ERR("driver_sndio_config_device(*): Can not set stream parameters");
  return -1;
 }

 if ( sio_start(self->handle) == 0 ) {
  ROAR_ERR("driver_sndio_config_device(*): Can not start stream");
  return -1;
 }

 self->need_config = 0;

 return 0;
}

int     driver_sndio_reopen_device(struct driver_sndio * self) {
 return -1;
}

ssize_t driver_sndio_write        (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct driver_sndio * self = vio->inst;

 if ( self->need_config ) {
  if ( driver_sndio_config_device(vio->inst) == -1 ) {
   return -1;
  }
 }

 return sio_write(self->handle, buf, count);
}

int     driver_sndio_sync         (struct roar_vio_calls * vio) {
 return -1;
}

#define data(x) x: if ( data == NULL ) return -1;
#define no_data(x) x: if ( data != NULL ) return -1;

int     driver_sndio_ctl          (struct roar_vio_calls * vio, int cmd, void * data) {
 struct driver_sndio * self = vio->inst;
 unsigned d;

 switch (cmd) {
  case data(ROAR_VIO_CTL_SET_SSTREAMID)
    self->ssid = *(int *)data;
   break;
  case data(ROAR_VIO_CTL_SET_SSTREAM)
    self->stream = data;
   break;
  case data(ROAR_VIO_CTL_GET_AUINFO)
    memcpy(data, &(self->info), sizeof(struct roar_audio_info));
   break;
  case data(ROAR_VIO_CTL_SET_AUINFO)
    memcpy(&(self->info), data, sizeof(struct roar_audio_info));
    return driver_sndio_reopen_device(self);
   break;
  case ROAR_VIO_CTL_SET_VOLUME:
    switch (self->info.channels) {
     case 1:
       d = ROAR_MIXER(data)->mixer[0] * SIO_MAXVOL / ROAR_MIXER(data)->scale;
      break;
     case 2:
       if ( ROAR_MIXER(data)->mixer[0] != ROAR_MIXER(data)->mixer[1] )
        return -1;
       d = ROAR_MIXER(data)->mixer[0] * SIO_MAXVOL / ROAR_MIXER(data)->scale;
      break;
     default:
      return -1;
    }
    return sio_setvol(self->handle, d) == 0 ? -1 : 0;
   break;
  default:
    return -1;
   break;
 }

 return 0;
}

#endif

//ll
