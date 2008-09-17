//audiocd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - August 2008
 *
 *  This file is part of libroaryiff a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroaryiff is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <libroaryiff.h>

struct {
 int fh;
 int stream;
 char * device;
} _g_roaryiff_cdrom = {-1, -1, NULL};

int YEjectAudioCD(YConnection *con) {
 YStopAudioCD(con);
 return system("eject") == 0 ? 0 : -1;
}

int YPlayAudioCDTrack(YConnection *con, int track_number) {
 struct roar_connection rcon;
 int flags;

 if ( con == NULL )
  return -1;

 rcon.fh = con->fd;


 if ( _g_roaryiff_cdrom.fh == -1 ) {
  // ok, we need to open the cdrom device
  if ( _g_roaryiff_cdrom.device == NULL ) {
   _g_roaryiff_cdrom.device = roar_cdromdevice();
   if ( _g_roaryiff_cdrom.device == NULL )
    return -1;
  }
  if ( (_g_roaryiff_cdrom.fh = open(_g_roaryiff_cdrom.device, O_RDONLY, 0644)) == -1 )
   return -1;

  if ( (flags = fcntl(_g_roaryiff_cdrom.fh, F_GETFL, 0)) == -1 ) {
   close(_g_roaryiff_cdrom.fh);
   _g_roaryiff_cdrom.fh = -1;
   return -1;
  }

  flags |= FD_CLOEXEC;

  if ( fcntl(_g_roaryiff_cdrom.fh, F_SETFL, flags) == -1 ) {
   close(_g_roaryiff_cdrom.fh);
   _g_roaryiff_cdrom.fh = -1;
   return -1;
  }
 }

 return -1;
}

int YStopAudioCD(YConnection *con) {
 struct roar_connection rcon;

 if ( con == NULL )
  return -1;

 rcon.fh = con->fd;

 if ( _g_roaryiff_cdrom.fh != -1 ) {
  close(_g_roaryiff_cdrom.fh);
 }

 if ( _g_roaryiff_cdrom.stream != -1 ) {
  roar_kick(&rcon, ROAR_OT_STREAM, _g_roaryiff_cdrom.stream);
 }

 return 0;
}

//ll
