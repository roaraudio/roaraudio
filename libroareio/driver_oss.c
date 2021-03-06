//driver_oss.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <roaraudio.h>
#include "driver.h"

#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)

#if defined(__OpenBSD__) || defined(__NetBSD__)
#include <soundcard.h>
#else
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>

#ifdef SNDCTL_DSP_SETFRAGMENT
static void roar_cdriver_oss_try_buf_setups(struct roar_vio_calls * calls) {
 struct roar_vio_sysio_ioctl ctl;
 int blocksizes[] = {11, 12, 13};
 int blocks[]     = {4, 5, 6, 3, 7, 2, 8};
 int bs, b;
 int tmp;

 ctl.cmd  = SNDCTL_DSP_SETFRAGMENT;
 ctl.argp = &tmp;

 for (bs = 0; bs < sizeof(blocksizes)/sizeof(int); bs++) {
  for (b = 0; b  < sizeof(blocks)    /sizeof(int); b++ ) {
   tmp = blocksizes[bs] | (blocks[b] << 16);
   if ( roar_vio_ctl(calls, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == 0 )
    return;
  }
 }
}
#endif

#define _err() roar_vio_close(calls); return -1

int roar_cdriver_oss(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir) {
 struct roar_vio_sysio_ioctl ctl;
 int tmp, ctmp;

 // preinit ctl struct, we always pass ints in tmp.
 ctl.argp = &tmp;

 ROAR_DBG("roar_cdriver_oss(*) = ?");

#ifdef ROAR_DEFAULT_OSS_DEV
 if ( dev == NULL )
  dev = ROAR_DEFAULT_OSS_DEV;
#endif

 if ( dev == NULL )
  return -1;

 ROAR_DBG("roar_cdriver_oss(*) = ?");

 switch (dir) {
  case ROAR_DIR_PLAY:
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_OUTPUT:
    tmp = O_WRONLY;
   break;
  case ROAR_DIR_BIDIR:
    tmp = O_RDWR;
   break;
  case ROAR_DIR_RECORD:
    tmp = O_RDONLY;
   break;
  default:
    return -1;
 }

 ROAR_DBG("roar_cdriver_oss(*) = ?");

 if ( roar_vio_open_file(calls, dev, tmp, 0644) == -1 )
  return -1;

// channels:
#ifdef SNDCTL_DSP_CHANNELS
 tmp = info->channels;

 ctl.cmd  = SNDCTL_DSP_CHANNELS;

 if ( roar_vio_ctl(calls, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 ) {
  _err();
 }

 if ( tmp != info->channels ) {
  _err();
 }
#else
 switch (info->channels) {
  case  1: tmp = 0; break;
  case  2: tmp = 1; break;
  default: _err();
 }

 ctl.cmd = SNDCTL_DSP_STEREO;

 if ( roar_vio_ctl(calls, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 ) {
  _err();
 }
#endif

// codec/bits:
 if ( info->codec != ROAR_CODEC_ALAW && info->codec != ROAR_CODEC_MULAW && info->bits != 16 ) {
  // other modes are currently not supported
  _err();
 }

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
    tmp = AFMT_S16_LE;
   break;
  case ROAR_CODEC_PCM_S_BE:
    tmp = AFMT_S16_BE;
   break;
  case ROAR_CODEC_PCM_U_LE:
    tmp = AFMT_U16_LE;
   break;
  case ROAR_CODEC_PCM_U_BE:
    tmp = AFMT_U16_BE;
   break;
  case ROAR_CODEC_ALAW:
    tmp = AFMT_A_LAW;
   break;
  case ROAR_CODEC_MULAW:
    tmp = AFMT_MU_LAW;
   break;
  default:
    _err();
 }

 ctmp = tmp;
#ifdef SNDCTL_DSP_SETFMT
 ctl.cmd = SNDCTL_DSP_SETFMT;
#else
 ctl.cmd = SNDCTL_DSP_SAMPLESIZE;
#endif

 if ( roar_vio_ctl(calls, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 ) {
  _err();
 }

 if ( tmp != ctmp ) {
  _err();
 }

// rate:
 tmp = info->rate;

 ctl.cmd = SNDCTL_DSP_SPEED;
 if ( roar_vio_ctl(calls, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 ) {
  _err();
 }

 if ( tmp != info->rate ) {
  _err();
 }

#ifdef SNDCTL_DSP_SETFRAGMENT
 roar_cdriver_oss_try_buf_setups(calls);
#endif

 return 0;
}

#endif

//ll
