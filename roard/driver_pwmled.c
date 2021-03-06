//driver_pwmled.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#if !defined(ROAR_WITHOUT_DCOMP_PWMLED) && !defined(ROAR_WITHOUT_VIO_DSTR)

int driver_pwmled_open_vio  (struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
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

 if ( info->rate == g_sa->rate ) {
  self->rate = 9600;
 } else {
  self->rate = info->rate;
 }

 self->channel = 0;

 if ( roar_light_pwm_new(&(self->state), 16) == -1 ) {
  free(self);
  return -1;
 }

 if ( sstream != NULL )
  driver_pwmled_ctl(inst, ROAR_VIO_CTL_SET_SSTREAM, sstream);

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
 int value;

 if ( vio == NULL || buf == NULL )
  return -1;

 if ( count != 512 )
  return -1;

 value = ((unsigned char*)buf)[self->channel] / 15;

 if ( roar_light_pwm_set(&(self->state), value) == -1 )
  return -1;

// ROAR_WARN("driver_pwmled_write(*): value=%i", value);

 if ( value ) {
                                                                     // bit per word, bit per byte
  return roar_light_pwm_send(&(self->state), &(self->vio), self->rate/11/8/100) == 0 ? count : -1;
 }

 return count;
}

int driver_pwmled_ctl(struct roar_vio_calls * vio, int cmd, void * data) {
 struct driver_pwmled * self = vio->inst;

 if ( vio == NULL )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_SET_SSTREAM:
    ROAR_STREAM(data)->dir = ROAR_DIR_LIGHT_OUT;
    ROAR_STREAM_SERVER(data)->codec_orgi = ROAR_CODEC_DMX512;
   break;
  case ROAR_VIO_CTL_SET_DMXSCHAN:
    if ( *(uint16_t*)data > 511 )
     return -1;

    self->channel = *(uint16_t*)data;
   break;
  default:
   return -1;
 }

 return 0;
}

#endif

//ll
