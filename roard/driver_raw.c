//driver_raw.c:

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

int driver_raw_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
 int * di = malloc(sizeof(int));

 if ( di == NULL )
  return -1;

 *inst = (DRIVER_USERDATA_T)di;

 if ( device == NULL ) {
  free(di);
  *inst = NULL;
  return -1;
 }

 *di = open(device, O_CREAT|O_TRUNC|O_WRONLY, 0644);

 if ( *di == -1 ) {
  free(di);
  *inst = NULL;
  return -1;
 }

 return 0;
}

int driver_raw_close(DRIVER_USERDATA_T   inst) {
 int fh = *(int*)inst;

 free((void*)inst);

 return close(fh);
}

int driver_raw_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

int driver_raw_write(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return write(*(int*)inst, buf, len);
}

int driver_raw_read(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return read(*(int*)inst, buf, len);
}

int driver_raw_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

//ll
