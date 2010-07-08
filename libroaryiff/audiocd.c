//audiocd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from the yiff sound system.
 *  According to the debian/copyright file upstream author is
 *  Tara Milana <learfox@twu.net>. Also copyright is listed as:
 *  Copyright (C)  1997-2003 WolfPack Entertainment
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 * NOTE: code fragments (like prototypes) taken from the yiff package
 *       are 'GPLv2 or later' and are upgraded to GPLv3 by being used
 *       within this document.
 *
 */

#include <libroaryiff.h>

struct {
 int is_open;
 int is_playing;
 struct roar_cdrom cdrom[1];
} _g_roaryiff_cdrom = {0, 0, {}};

int YEjectAudioCD(YConnection *con) {
 YStopAudioCD(con);
 return system("eject") == 0 ? 0 : -1;
}

int YPlayAudioCDTrack(YConnection *con, int track_number) {
 struct roar_connection rcon;

 if ( con == NULL )
  return -1;

 roar_connect_fh(&rcon, con->fd);

 if ( ! _g_roaryiff_cdrom.is_open ) {
  if ( roar_cdrom_open(&rcon, _g_roaryiff_cdrom.cdrom, NULL) == -1 )
   return -1;
 }

 return roar_cdrom_play(_g_roaryiff_cdrom.cdrom, track_number);
}

int YStopAudioCD(YConnection *con) {
 struct roar_connection rcon;

 if ( con == NULL )
  return -1;

 roar_connect_fh(&rcon, con->fd);

 if ( _g_roaryiff_cdrom.is_playing ) {
  roar_cdrom_stop(_g_roaryiff_cdrom.cdrom);
  _g_roaryiff_cdrom.is_playing = 0;
 }

 if ( _g_roaryiff_cdrom.is_open ) {
  roar_cdrom_close(_g_roaryiff_cdrom.cdrom);
  _g_roaryiff_cdrom.is_open = 0;
 }

 return 0;
}

//ll
