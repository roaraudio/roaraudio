//driver.c:

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
#include "autodetected.h"

#ifdef ROAR_HAVE_AD_ESDFW
#include "driver_esdfw.h"
#endif

int roar_cdriver_null(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir) {
 ROAR_DBG("roar_cdriver_null(calls=%p, name='%s', dev='%s', info=%p{...}, dir=%i(?)) = ?", calls, name, dev, info, dir);

 memset(calls, 0, sizeof(struct roar_vio_calls));
 calls->read  = roar_vio_null_rw;
 calls->write = roar_vio_null_rw;

 ROAR_DBG("roar_cdriver_null(calls=%p, name='%s', dev='%s', info=%p{...}, dir=%i(?)) = 0", calls, name, dev, info, dir);
 return 0;
}

struct roar_cdriver _g_roar_cdriver[] = {
 {"null", roar_cdriver_null},
#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
 {"oss",  roar_cdriver_oss},
#endif
#ifdef ROAR_HAVE_AD_ESDFW
#include "driver_esdfw.c"
#endif
 {NULL, NULL}
};

int roar_cdriver_open(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir) {
 int i;

 for (i = 0; _g_roar_cdriver[i].name != NULL; i++) {
  ROAR_DBG("roar_cdriver_open(*): _g_roar_cdriver[i].name='%s' <cmp> name='%s'", _g_roar_cdriver[i].name, name);
  if ( !strcmp(_g_roar_cdriver[i].name, name) )
   return _g_roar_cdriver[i].open(calls, name, dev, info, dir);
 }

 return -1;
}

//ll
