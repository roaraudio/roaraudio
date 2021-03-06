//lib.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

int lib_run_bg(char * cmd, int infh, int outfh, int errfh, int * closefh, int lenclose) {
#ifdef ROAR_HAVE_FORK
 pid_t child;
 int fh[3] = {-1, -1, -1};
 int i;

 ROAR_WARN("lib_run_bg(cmd='%s', ...): This function should never be called. Contact devels.");

 child = fork();

 if ( child == -1 ) {
  ROAR_ERR("lib_run_bg(*): Can not fork: %s", strerror(errno));
  return -1;
 }

 if ( child ) {
  // we are the parent!
  return child;
 }

 // we are the child.
 // first we need to close a lot of open files!

 // before we do this we need to keep backups of our handles:
 fh[0] = dup(infh);
 fh[1] = dup(outfh);
 fh[2] = dup(errfh);

 // TODO: test for errors here.

#ifdef ROAR_SUPPORT_LISTEN
 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++)
  if ( g_listen[i].used )
   roar_vio_close(&(g_listen[i].sock)); // listen socket.
#endif

// this breaks the new driver interface
// clients_free(); // delete all clients!, this allso delets all streams

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 midi_free(); // close midi devices
#endif

 // close fh's we got ask to close:
#ifdef ROAR_HAVE_IO_POSIX
 for (i = 0; i < lenclose; i++)
  close(closefh[i]);
#else
 if ( lenclose ) {
  ROAR_WARN("lib_run_bg(*): lenclose > 0 and no way known to close fds");
 }
#endif

 // next we need to remap our stdio:
 // stdio: fh 0..2

 for (i = 0; i < 3; i++) {
#ifdef ROAR_HAVE_IO_POSIX
  close(i);
#endif
  dup2(fh[i], i); // todo test if this is ok.
  close(fh[i]);
 }

 // OK, now we should have set up all our fh properbly, exec our command:

 execl("/bin/sh", "sh", "-c", cmd, (char*)NULL);

 // still alive? BAD!
 ROAR_ERR("lib_run_bg(*): We are still alive! BAD!");
 ROAR_U_EXIT(3);
 return -1;
#else
 return -1;
#endif
}

//ll
