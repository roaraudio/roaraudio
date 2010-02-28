//driver_dstr.c:

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

#ifndef ROAR_WITHOUT_VIO_DSTR
int driver_dstr_open_vio  (struct roar_vio_calls * inst,
                           char * device,
                           struct roar_audio_info * info,
                           int fh,
                           struct roar_stream_server * sstream) {
 struct roar_vio_defaults def;

 if ( fh == -1 ) {
  if ( device == NULL )
   return -1;

  if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, O_WRONLY|O_CREAT|O_TRUNC, 0644) == -1 )
   return -1;

  if ( roar_vio_open_dstr(inst, device, &def, 1) == -1 )
   return -1;
 } else {
  if ( roar_vio_open_fh(inst, fh) == -1 )
   return -1;
 }

 if ( sstream != NULL )
  roar_vio_ctl(inst, ROAR_VIO_CTL_SET_SSTREAM, sstream);

 return 0;
}
#endif

//ll
