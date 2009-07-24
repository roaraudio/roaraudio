//driver_oss.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

#define _err() roar_vio_close(calls); return -1

int roar_cdriver_oss(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir) {
 int fh;
 int tmp, ctmp;

 ROAR_DBG("roar_cdriver_oss(*) = ?");

 if ( dev == NULL )
  dev = ROAR_DEFAULT_OSS_DEV;

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
  default:
    return -1;
 }

 if ( roar_vio_open_file(calls, dev, tmp, 0644) == -1 )
  return -1;

 if ( roar_vio_ctl(calls, ROAR_VIO_CTL_GET_FH, &fh) == -1 ) {
  _err();
 }

// channels:
#ifdef SNDCTL_DSP_CHANNELS
 tmp = info->channels;

 if ( ioctl(fh, SNDCTL_DSP_CHANNELS, &tmp) == -1 ) {
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

 if ( ioctl(fh, SNDCTL_DSP_STEREO, &tmp) == -1 ) {
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
 }

 ctmp = tmp;
#ifdef SNDCTL_DSP_SETFMT
 if ( ioctl(fh, SNDCTL_DSP_SETFMT, &tmp) == -1 ) {
#else
 if ( ioctl(fh, SNDCTL_DSP_SAMPLESIZE, &tmp) == -1 ) {
#endif
  _err();
 }

 if ( tmp != ctmp ) {
  _err();
 }

// rate:
 tmp = info->rate;

 if ( ioctl(fh, SNDCTL_DSP_SPEED, &tmp) == -1 ) {
  _err();
 }

 if ( tmp != info->rate ) {
  _err();
 }

 return 0;
}

#endif

//ll
