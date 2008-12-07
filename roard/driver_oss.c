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

int driver_oss_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh) {
 int tmp;
 int ctmp;
 char * es;

#ifdef ROAR_DEFAULT_OSS_DEV
 if ( device == NULL )
  device = ROAR_DEFAULT_OSS_DEV;
#endif

 if ( device == NULL ) {
  ROAR_ERR("driver_oss_open(*): no default device found, you need to specify one manuelly");
  return -1;
 }

 roar_vio_init_calls(inst);

 if (  fh == -1 ) {
  if ( (fh = open(device, O_WRONLY, 0644)) == -1 ) {
   ROAR_ERR("driver_oss_open(*): Can not open OSS device: %s: %s", device, strerror(errno));
   return -1;
  }
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

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
    switch (info->bits) {
     case  8: tmp = AFMT_S8;     break;
     case 16: tmp = AFMT_S16_LE; break;
//     case 24: tmp = AFMT_S24_PACKED; break;
#ifdef AFMT_S32_LE
     case 32: tmp = AFMT_S32_LE; break;
#endif
     default: er();
    }
   break;
  case ROAR_CODEC_PCM_S_BE:
    switch (info->bits) {
     case  8: tmp = AFMT_S8;     break;
     case 16: tmp = AFMT_S16_BE; break;
//     case 24: tmp = AFMT_S24_PACKED; break;
#ifdef AFMT_S32_BE
     case 32: tmp = AFMT_S32_BE; break;
#endif
     default: er();
    }
   break;
  case ROAR_CODEC_PCM_U_LE:
    switch (info->bits) {
     case  8: tmp = AFMT_U8;     break;
     case 16: tmp = AFMT_U16_LE; break;
     default: er();
    }
   break;
  case ROAR_CODEC_PCM_U_BE:
    switch (info->bits) {
     case  8: tmp = AFMT_U8;     break;
     case 16: tmp = AFMT_U16_BE; break;
     default: er();
    }
  case ROAR_CODEC_ALAW:
    tmp = AFMT_A_LAW;
   break;
  case ROAR_CODEC_MULAW:
    tmp = AFMT_MU_LAW;
   break;
#ifdef AFMT_VORBIS
  case ROAR_CODEC_OGG_VORBIS:
    tmp = AFMT_VORBIS;
   break;
#endif
  default:
    er();
   break;
 }

 ctmp = tmp;
 if ( ioctl(fh, SNDCTL_DSP_SAMPLESIZE, &tmp) == -1 ) {
  ROAR_ERR("driver_oss_open(*): can not set sample format");
  er();
 }

 if ( tmp != ctmp ) {
  es = NULL;
  switch (tmp) {
   case AFMT_S8    : es = "bits=8,codec=pcm";       break;
   case AFMT_U8    : es = "bits=8,codec=pcm_u_le";  break;
   case AFMT_S16_LE: es = "bits=16,codec=pcm_s_le"; break;
   case AFMT_S16_BE: es = "bits=16,codec=pcm_s_be"; break;
   case AFMT_U16_LE: es = "bits=16,codec=pcm_u_le"; break;
   case AFMT_U16_BE: es = "bits=16,codec=pcm_u_be"; break;
#ifdef AFMT_S32_LE
   case AFMT_S32_LE: es = "bits=32,codec=pcm_s_le"; break;
#endif
#ifdef AFMT_S32_BE
   case AFMT_S32_BE: es = "bits=32,codec=pcm_s_be"; break;
#endif
   case AFMT_A_LAW : es = "codec=alaw";             break;
   case AFMT_MU_LAW: es = "codec=mulaw";            break;
#ifdef AFMT_VORBIS
   case AFMT_VORBIS: es = "codec=ogg_vorbis";       break;
#endif
  }

  if ( es != NULL ) {
   ROAR_ERR("driver_oss_open(*): can not set requested codec, OSS retruned another codec then requested, to use this restart with -oO %s or set codec manuelly via -oO codec=somecodec", es);
  } else {
   ROAR_ERR("driver_oss_open(*): can not set requested codec, set codec manuelly via -oO codec=somecodec");
  }
  er();
 }

 tmp = info->rate;

 if ( ioctl(fh, SNDCTL_DSP_SPEED, &tmp) == -1 ) {
  ROAR_ERR("driver_oss_open(*): can not set sample rate");
  er();
 }

 if ( tmp < info->rate * 0.98 || tmp > info->rate * 1.02 ) {
  ROAR_ERR("driver_oss_open(*): sample rate out of acceptable accuracy");
  er();
 }

 ROAR_DBG("driver_oss_open(*): OSS devices opened :)");

 return 0;
}

int driver_oss_close(DRIVER_USERDATA_T   inst) {
 return close(roar_vio_get_fh((struct roar_vio_calls *)inst));
}


#endif
//ll
