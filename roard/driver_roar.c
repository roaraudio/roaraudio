//driver_raor.c:

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

// this is to avoid warning messages on platforms
// where sizeof(void*) == 8 and szeof(int) == 4
#ifdef __LP64__
#define INSTINT long int
#else
#define INSTINT int
#endif

int driver_roar_open(DRIVER_USERDATA_T * inst, char * device, struct roar_audio_info * info) {
/*
 esd_format_t format = ESD_STREAM | ESD_PLAY;
 char name[80] = "roard";

 format |= info->bits     == 16 ? ESD_BITS16 : ESD_BITS8;
 format |= info->channels ==  2 ? ESD_STEREO : ESD_MONO;

 *(int*)inst = esd_play_stream_fallback(format, info->rate, device, name);

 if ( *(int*)inst == -1 )
  return -1;

 return 0;
*/

 *(INSTINT*)inst =  roar_simple_play(info->rate, info->channels, info->bits, info->codec, device, "roard");

 if ( *(INSTINT*)inst == -1 )
  return -1;

 return 0;
}

int driver_roar_close(DRIVER_USERDATA_T   inst) {
 return roar_simple_close((INSTINT)inst);
}

int driver_roar_pause(DRIVER_USERDATA_T   inst, int newstate) {
 return -1;
}

int driver_roar_write(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return write((INSTINT)inst, buf, len);
}

int driver_roar_read(DRIVER_USERDATA_T   inst, char * buf, int len) {
 return read((INSTINT)inst, buf, len);
}

int driver_roar_flush(DRIVER_USERDATA_T   inst) {
 return 0;
}

//ll
