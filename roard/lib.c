//lib.c:

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

int lib_run_bg(char * cmd, int infh, int outfh, int errfh, int * closefh, int lenclose) {
 pid_t child = fork();
 int fh[3] = {-1, -1, -1};
 int i;

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

 close(g_listen_socket); // listen socket.

 clients_free(); // delete all clients!, this allso delets all streams

 midi_free(); // close midi devices

 // close fh's we got ask to close:
 for (i = 0; i < lenclose; i++)
  close(closefh[i]);

 // TODO: what aout the output driver?

 // next we need to remap our stdio:
 // stdio: fh 0..2

 for (i = 0; i < 3; i++) {
  close(i);
  dup2(fh[i], i); // todo test if this is ok.
  close(fh[i]);
 }

 // OK, now we should have set up all our fh properbly, exec our command:

 execl("/bin/sh", "sh", "-c", cmd, NULL);

 // still alive? BAD!
 ROAR_ERR("lib_run_bg(*): We are still alive! BAD!");
 _exit(3);
 return -1;
}

//ll
