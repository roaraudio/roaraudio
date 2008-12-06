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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _DRIVER_H_
#define _DRIVER_H_

#include <roaraudio.h>

#define DRIVER_USERDATA_T void *

#ifdef ROAR_HAVE_ESD
#define ROAR_DRIVER_DEFAULT "esd"
#else
#define ROAR_DRIVER_DEFAULT "null"
#endif

#ifdef ROAR_HAVE_ESD
#include <esd.h>
#include "driver_esd.h"
#endif

#include "driver_roar.h"
#include "driver_raw.h"

#ifdef ROAR_HAVE_LIBAO
#include <ao/ao.h>
#include "driver_ao.h"
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

struct roar_driver {
 char * name;
 char * desc;
 char * devices;
 int (*open )(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info);
 int (*close)(DRIVER_USERDATA_T   inst);
 int (*pause)(DRIVER_USERDATA_T   inst, int newstate);
 int (*write)(DRIVER_USERDATA_T   inst, char * buf, int len);
 int (*read )(DRIVER_USERDATA_T   inst, char * buf, int len);
 int (*flush)(DRIVER_USERDATA_T   inst);
 int (*vio_init)(struct roar_vio_calls * calls, char * device, struct roar_audio_info * info);
};

void print_driverlist (void);

int driver_open (DRIVER_USERDATA_T * inst,
                 int * driver_id, char * driver /* NOTE: this is not part of struct roar_driver's def! */,
                 char * device, struct roar_audio_info * info);
int driver_close(DRIVER_USERDATA_T   inst, int driver);
int driver_pause(DRIVER_USERDATA_T   inst, int driver, int newstate);
int driver_write(DRIVER_USERDATA_T   inst, int driver, char * buf, int len);
int driver_read (DRIVER_USERDATA_T   inst, int driver, char * buf, int len);
int driver_flush(DRIVER_USERDATA_T   inst, int driver);

#endif

//ll
