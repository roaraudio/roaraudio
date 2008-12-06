//driver_oss.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)

#define er() close(fh); return -1

int driver_oss_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info) {
 int tmp;
 int fh;

 if ( device == NULL )
  device = ROAR_DEFAULT_OSS_DEV;

 roar_vio_init_calls(inst);

 if ( (fh = open(device, O_WRONLY, 0644)) == -1 ) {
  ROAR_ERR("driver_oss_open(*): Can not open OSS device: %s: %s", device, strerror(errno));
  return -1;
 }

 roar_vio_set_fh(inst, fh);

 switch (info->channels) {
  case  1: tmp = 0; break;
  case  2: tmp = 1; break;
  default: er();
 }

 if ( ioctl(fh, SNDCTL_DSP_STEREO, &tmp) == -1 ) {
  er();
 }

 switch (info->bits) {
  case  8: tmp = AFMT_S8; break;
  case 16:
    switch (info->codec) {
     case ROAR_CODEC_PCM_S_LE: tmp = AFMT_S16_LE; break;
     case ROAR_CODEC_PCM_S_BE: tmp = AFMT_S16_BE; break;
     default                 : er();
    }
   break;
  default: er();
 }

 if ( ioctl(fh, SNDCTL_DSP_SAMPLESIZE, &tmp) == -1 ) {
  er();
 }

 tmp = info->rate;

 if ( ioctl(fh, SNDCTL_DSP_SPEED, &tmp) == -1 ) {
  er();
 }

 if ( tmp < info->rate * 0.98 || tmp > info->rate * 1.02 ) {
  er();
 }

 return 0;
}

int driver_oss_close(DRIVER_USERDATA_T   inst) {
 return close(roar_vio_get_fh((struct roar_vio_calls *)inst));
}


#endif
//ll
