//events.c:

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

// RoarAudio isn't event based, so we need to trick a lot here

int YGetNextEvent (YConnection *con, YEvent *event, Boolean block) {
 struct roar_connection rcon;
 struct roar_stream s;
 struct roar_message    m;

 if ( con == NULL || event == NULL )
  return -1;

 rcon.fh = con->fd;

 if ( con->prev_generated_yid != YIDNULL ) {
  if ( roar_get_stream(&rcon, &s, ROARYIFF_YID2ROAR(con->prev_generated_yid)) == -1 ) {
   // ok, we know something happened.
   // next we try to seend a noop to see of the server is still alive
   m.cmd = ROAR_CMD_NOOP;
   m.datalen = 0;

   if (  roar_req(&rcon, &m, NULL) == -1 ) {
    // the server died
    event->type     = YDisconnect;
    return 1;
   } else {
    // the stream died
    event->type     = YSoundObjectKill;
    event->kill.yid = con->prev_generated_yid;
    return 1;
   }
  }
  // else { nothing interesting happened }
 }

 return 0;
}

//ll
