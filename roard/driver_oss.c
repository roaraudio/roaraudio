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

struct driver_oss {
 char * device;
 int fh;
 int blocks;
 int blocksize;
 struct roar_audio_info info;
 int need_reopen;
 int need_config;
};

#define _get(vio,obj) (((struct driver_oss*)((vio)->inst))->obj)

ssize_t driver_oss_write    (struct roar_vio_calls * vio, void *buf, size_t count);
int     driver_oss_nonblock (struct roar_vio_calls * vio, int state);
int     driver_oss_close_vio(struct roar_vio_calls * vio);

int driver_oss_init_vio(struct roar_vio_calls * vio, struct driver_oss * inst) {
 if ( vio == NULL )
  return -1;

 memset(vio, 0, sizeof(struct roar_vio_calls));

 vio->write    = driver_oss_write;
 vio->nonblock = driver_oss_nonblock;
 vio->sync     = driver_oss_sync;
 vio->ctl      = driver_oss_ctl;
 vio->close    = driver_oss_close_vio;

 vio->inst     = (void*) inst;

 return 0;
}

int driver_oss_open_device(struct driver_oss * self) {
 int    fh     = self->fh;
 char * device = self->device;

 if ( fh != -1 )
  return 0;

#ifdef ROAR_DEFAULT_OSS_DEV
 if ( device == NULL )
  device = ROAR_DEFAULT_OSS_DEV;
#endif

 if ( device == NULL ) {
  ROAR_ERR("driver_oss_open_device(*): no default device found, you need to specify one manuelly");
  return -1;
 }

 if ( (fh = open(device, O_WRONLY, 0644)) == -1 ) {
  ROAR_ERR("driver_oss_open_device(*): Can not open OSS device: %s: %s", device, strerror(errno));
  return -1;
 }

 self->fh          = fh;
 self->need_config = 1;

 return 0;
}

int driver_oss_config_device(struct driver_oss * self) {
 int                      fh   =   self->fh;
 struct roar_audio_info * info = &(self->info);
 int tmp, ctmp;
 char * es;

 if ( fh == -1 )
  return -1;

#ifdef SNDCTL_DSP_CHANNELS
 tmp = info->channels;

 if ( ioctl(fh, SNDCTL_DSP_CHANNELS, &tmp) == -1 ) {
  ROAR_ERR("driver_oss_open(*): can not set number of channels");
  return -1;
 }

 if ( tmp != info->channels ) {
   ROAR_ERR("driver_oss_open(*): can not set requested numer of channels, OSS suggested %i channels, to use this restart with -oO channels=%i or set codec manuelly via -oO channels=num", tmp, tmp);
  return -1;
 }
#else
 switch (info->channels) {
  case  1: tmp = 0; break;
  case  2: tmp = 1; break;
  default: return -1;
 }

 if ( ioctl(fh, SNDCTL_DSP_STEREO, &tmp) == -1 ) {
  ROAR_ERR("driver_oss_open(*): can not set number of channels");
  return -1;
 }
#endif

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
    switch (info->bits) {
     case  8: tmp = AFMT_S8;     break;
     case 16: tmp = AFMT_S16_LE; break;
//     case 24: tmp = AFMT_S24_PACKED; break;
#ifdef AFMT_S32_LE
     case 32: tmp = AFMT_S32_LE; break;
#endif
     default: return -1;
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
     default: return -1;
    }
   break;
  case ROAR_CODEC_PCM_U_LE:
    switch (info->bits) {
     case  8: tmp = AFMT_U8;     break;
     case 16: tmp = AFMT_U16_LE; break;
     default: return -1;
    }
   break;
  case ROAR_CODEC_PCM_U_BE:
    switch (info->bits) {
     case  8: tmp = AFMT_U8;     break;
     case 16: tmp = AFMT_U16_BE; break;
     default: return -1;
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
    return -1;
   break;
 }

 ctmp = tmp;
#ifdef SNDCTL_DSP_SETFMT
 if ( ioctl(fh, SNDCTL_DSP_SETFMT, &tmp) == -1 ) {
#else
 if ( ioctl(fh, SNDCTL_DSP_SAMPLESIZE, &tmp) == -1 ) {
#endif
  ROAR_ERR("driver_oss_open(*): can not set sample format");
  return -1;
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
   ROAR_ERR("driver_oss_open(*): can not set requested codec, OSS retruned another codec than requested, to use this restart with -oO %s or set codec manuelly via -oO codec=somecodec", es);
  } else {
   ROAR_ERR("driver_oss_open(*): can not set requested codec, set codec manuelly via -oO codec=somecodec");
  }
  return -1;
 }

 tmp = info->rate;

 if ( ioctl(fh, SNDCTL_DSP_SPEED, &tmp) == -1 ) {
  ROAR_ERR("driver_oss_open(*): can not set sample rate");
  return -1;
 }

 if ( tmp < info->rate * 0.98 || tmp > info->rate * 1.02 ) {
  ROAR_ERR("driver_oss_open(*): sample rate out of acceptable accuracy");
  return -1;
 }

 // latency things:
#ifdef SNDCTL_DSP_SETFRAGMENT

 // defaults
 if ( self->blocksize < 1 )
  self->blocksize = 2048;
 if ( self->blocks < 1 )
  self->blocks    =    4;

 switch (self->blocksize) {
  case 1<< 4: tmp =  4; break;
  case 1<< 5: tmp =  5; break;
  case 1<< 6: tmp =  6; break;
  case 1<< 7: tmp =  7; break;
  case 1<< 8: tmp =  8; break;
  case 1<< 9: tmp =  9; break;
  case 1<<10: tmp = 10; break;
  case 1<<11: tmp = 11; break;
  case 1<<12: tmp = 12; break;
  case 1<<13: tmp = 13; break;
  case 1<<14: tmp = 14; break;
  case 1<<15: tmp = 15; break;
  case 1<<16: tmp = 16; break;
  default: tmp = 11;
    ROAR_WARN("driver_oss_config_device(*): blocksize of %i byte is not a valid value. trying 2KB", self->blocksize);
   break;
 }

 ROAR_DBG("driver_oss_config_device(*): blocksize=%i(N=%i), blocks=%i", self->blocksize, tmp, self->blocks);

 tmp |= self->blocks << 16;
 if ( ioctl(fh, SNDCTL_DSP_SETFRAGMENT, &tmp) == -1 ) {
  ROAR_WARN("driver_oss_ctl(*): Can not set fragment size, sorry :(");
 }
#endif

 self->need_config = 0;

 return 0;
}

#define er() close(self->fh); if ( self->device ) free(self->device); free(self); return -1
int driver_oss_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh) {
 struct driver_oss * self = NULL;

 if ( (self = malloc(sizeof(struct driver_oss))) == NULL ) {
  ROAR_ERR("driver_oss_open(*): Can not malloc() instance data: %s", strerror(errno));
  return -1;
 }

 memset(self, 0, sizeof(struct driver_oss));
 memcpy(&(self->info), info, sizeof(struct roar_audio_info));

 self->fh = fh;

 if ( device != NULL )
  self->device = strdup(device);

 if ( driver_oss_init_vio(inst, self) == -1 ) {
  ROAR_ERR("driver_oss_open(*): Can not init vio interface");
  er();
 }

 if ( driver_oss_open_device(self) == -1 ) {
  ROAR_ERR("driver_oss_open(*): Can not open audio device");
  er();
 }

 ROAR_DBG("driver_oss_open(*): OSS devices opened :)");

 return 0;
}
#undef er

int driver_oss_close(DRIVER_USERDATA_T   inst) {
 return roar_vio_close((struct roar_vio_calls *)inst);
}

int     driver_oss_close_vio(struct roar_vio_calls * vio) {
 close(_get(vio,fh));

 if ( _get(vio,device) != NULL )
  free(_get(vio,device));

 free(vio->inst);
 return 0;
}

int     driver_oss_nonblock(struct roar_vio_calls * vio, int state) {
 if ( roar_socket_nonblock(_get(vio,fh), state) == -1 )
  return -1;

 if ( state == ROAR_SOCKET_NONBLOCK )
  return 0;

 roar_vio_sync(vio);

 return 0;
}

int driver_oss_sync(struct roar_vio_calls * vio) {
#ifdef SNDCTL_DSP_SYNC
 return ioctl(_get(vio,fh), SNDCTL_DSP_SYNC, NULL);
#else
 return 0;
#endif
}

int driver_oss_ctl(struct roar_vio_calls * vio, int cmd, void * data) {
 struct driver_oss * self = vio->inst;
 int d;

 ROAR_DBG("driver_oss_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 if ( vio == NULL )
  return -1;

 switch (cmd) {
  case ROAR_VIO_CTL_GET_DELAY:
#ifdef SNDCTL_DSP_GETODELAY
    if ( ioctl(_get(vio,fh), SNDCTL_DSP_GETODELAY, &d) == -1 )
     return -1;

    ROAR_DBG("driver_oss_ctl(*): delay=%i byte", d);

    *(uint_least32_t *)data = d;
#else
    return -1;
#endif
   break;
  case ROAR_VIO_CTL_SET_DBLOCKS:
#ifdef SNDCTL_DSP_SETFRAGMENT
    if ( !self->need_config ) {
     ROAR_WARN("driver_oss_ctl(*): possible late ROAR_VIO_CTL_SET_DBLOCKS, setting anyway.");
    }

    self->blocks    = *(uint_least32_t *)data;
#else
    return -1;
#endif
   break;
  case ROAR_VIO_CTL_SET_DBLKSIZE:
#ifdef SNDCTL_DSP_SETFRAGMENT
    if ( !self->need_config ) {
     ROAR_WARN("driver_oss_ctl(*): possible late ROAR_VIO_CTL_SET_DBLKSIZE, setting anyway.");
    }

    self->blocksize = *(uint_least32_t *)data;
#else
    return -1;
#endif
   break;
  case ROAR_VIO_CTL_GET_DBLKSIZE:
    if ( !self->blocksize )
     return -1;

    *(uint_least32_t *)data = self->blocksize;
    return 0;
   break;
  default:
   return -1;
 }

 return 0;
}

ssize_t driver_oss_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 if ( _get(vio,need_config) ) {
  if ( driver_oss_config_device(vio->inst) == -1 ) {
   return -1;
  }
 }

 return write(_get(vio,fh), buf, count);
}

#endif
//ll
