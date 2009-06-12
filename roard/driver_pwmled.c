//driver_pwmled.c:

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

int driver_pwmled_open_vio  (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh) {
 struct roar_vio_defaults def;
 struct driver_pwmled * self = malloc(sizeof(struct driver_pwmled));

 if ( self == NULL )
  return -1;

 if ( fh == -1 ) {
  if ( device == NULL )
   device = "/dev/ttyS0";

  if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, O_WRONLY, 0644) == -1 ) {
   free(self);
   return -1;
  }

  if ( roar_vio_open_dstr(&(self->vio), device, &def, 1) == -1 ) {
   free(self);
   return -1;
  }

  inst->inst = self;
 } else {
/*
  if ( roar_vio_open_fh(inst, fh) == -1 )
   return -1;
*/
  free(self);
  return -1;
 }

 inst->read     = NULL;
 inst->write    = driver_pwmled_write;
 inst->lseek    = NULL;
 inst->nonblock = NULL;
 inst->sync     = NULL;
 inst->ctl      = driver_pwmled_ctl;
 inst->close    = driver_pwmled_close;

 info->codec = ROAR_CODEC_DMX512;

 return 0;
}

int     driver_pwmled_close (struct roar_vio_calls * vio) {
 int ret = roar_vio_close(&(((struct driver_pwmled*)(vio->inst))->vio));

 if ( vio->inst != NULL )
  free(vio->inst);

 return ret;
}

// TODO: this function should be optimized.
ssize_t driver_pwmled_write (struct roar_vio_calls * vio,  void *buf, size_t count) {
 struct driver_pwmled * self = vio->inst;

 if ( vio == NULL || buf == NULL )
  return -1;

 if ( count != 512 )
  return -1;

 if ( roar_light_pwm_new(&(self->state), 16) == -1 )
  return -1;

 if ( roar_light_pwm_set(&(self->state), ((unsigned char*)buf)[0] / 16) == -1 )
  return -1;

 return roar_light_pwm_send(&(self->state), &(self->vio), 1) == 0 ? count : -1;
}

int driver_pwmled_ctl(struct roar_vio_calls * vio, int cmd, void * data) {

 if ( vio == NULL )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_SET_SSTREAM:
    ROAR_STREAM(data)->dir = ROAR_DIR_LIGHT_OUT;
    ROAR_STREAM_SERVER(data)->codec_orgi = ROAR_CODEC_DMX512;
   break;
  default:
   return -1;
 }

 return 0;
}

//ll
