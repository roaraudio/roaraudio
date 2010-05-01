//driver.h:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <roaraudio.h>

#define DRIVER_USERDATA_T void *

#ifndef ROAR_DRIVER_DEFAULT

#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
#define ROAR_DRIVER_DEFAULT "oss"
#else

#ifdef ROAR_HAVE_LIBWINMM
#define ROAR_DRIVER_DEFAULT "wmm"
#else

#ifdef ROAR_HAVE_LIBAO
#define ROAR_DRIVER_DEFAULT "ao"
#else

#ifdef ROAR_HAVE_ESD
#define ROAR_DRIVER_DEFAULT "esd"
#else

#define ROAR_DRIVER_DEFAULT "null"
#endif
#endif
#endif
#endif

#endif

#ifdef ROAR_HAVE_ESD
#include <esd.h>
#include "driver_esd.h"
#endif

#include "driver_roar.h"
#include "driver_raw.h"
#include "driver_dstr.h"
#include "driver_dmx.h"
#include "driver_pwmled.h"

#if defined(ROAR_HAVE_GETTIMEOFDAY) && defined(ROAR_HAVE_USLEEP)
#define ROAR_HAVE_DRIVER_SYSCLOCK
#include "driver_sysclock.h"
#endif

#ifdef ROAR_HAVE_LIBAO
#include <ao/ao.h>
#include "driver_ao.h"
#endif

#ifdef ROAR_HAVE_LIBSHOUT
#include <shout/shout.h>
#include "driver_shout.h"
#endif

#ifdef ROAR_HAVE_LIBSNDIO
#include <sndio.h>
#include "driver_sndio.h"
#endif

#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
#if defined(__OpenBSD__) || defined(__NetBSD__)
#include <soundcard.h>
#else
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>

#include "driver_oss.h"
#endif

#ifdef ROAR_HAVE_LIBRSOUND
#ifdef _DRV_NEED_RSOUND_H
#include <rsound.h>
#endif
#include "driver_rsound.h"
#endif

#ifdef ROAR_HAVE_LIBPORTAUDIO
#include <portaudio.h>
#ifdef ROAR_HAVE_LIBPABLIO
#include <pablio/pablio.h>
#endif
#include "driver_portaudio.h"
#endif

#ifdef ROAR_HAVE_LIBASOUND
#include <alsa/asoundlib.h>
#include "driver_alsa.h"
#endif

#ifdef ROAR_HAVE_LIBWINMM
#include <windows.h>
#include <mmsystem.h>
#include "driver_wmm.h"
#endif

#define DRV_FLAG_NONE           0x00
#define DRV_FLAG_FHSEC          0x01

struct roar_driver {
 char * name;
 char * desc;
 char * devices;
 unsigned int flags;
 unsigned int subsystems;
 int (*open )(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
 int (*close)(DRIVER_USERDATA_T   inst);
 int (*vio_init)(struct roar_vio_calls * calls, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);
};

void print_driverlist (void);

int driver_open (DRIVER_USERDATA_T * inst,
                 int * driver_id, char * driver /* NOTE: this is not part of struct roar_driver's def! */,
                 char * device, struct roar_audio_info * info);
int driver_openvio(struct roar_vio_calls * calls,
                 int * driver_id, char * driver /* NOTE: this is not part of struct roar_driver's def! */,
                 char * device, struct roar_audio_info * info, int fh,
                 struct roar_stream_server * sstream);

int driver_close(DRIVER_USERDATA_T   inst, int driver);
int driver_closevio(struct roar_vio_calls * calls, int driver);
int driver_pause(DRIVER_USERDATA_T   inst, int driver, int newstate);
int driver_write(DRIVER_USERDATA_T   inst, int driver, char * buf, int len);
int driver_read (DRIVER_USERDATA_T   inst, int driver, char * buf, int len);
int driver_flush(DRIVER_USERDATA_T   inst, int driver);
int driver_set_volume(int stream, struct roar_mixer_settings * mixer);

// opening a cdriver
#ifndef ROAR_WITHOUT_DCOMP_CDRIVER
int driver_cdriver_open(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);
#endif

#endif

//ll
