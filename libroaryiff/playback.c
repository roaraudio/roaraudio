//playback.c:

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

YID YStartPlaySoundObjectSimple (YConnection *con, const char *path) {
 return YStartPlaySoundObject(con, path, NULL);
}

YID YStartPlaySoundObject (YConnection *con, const char *path, YEventSoundPlay *value) {
 if ( !con )
  return YIDNULL;

 if ( !path )
  return YIDNULL;

 // hm,... find out how to do this.
 // need to start ssize_t roar_file_play (struct roar_connection * con, char * file, int exec)
 // in background


 return YIDNULL;
}

void YDestroyPlaySoundObject(YConnection *con, YID yid) {
 struct roar_connection rcon;

 if ( !con )
  return ;

 if ( yid == YIDNULL )
  return;

 rcon.fh = con->fd;

 roar_kick(&rcon, ROAR_OT_STREAM, ROARYIFF_YID2ROAR(yid));
}

//ll
