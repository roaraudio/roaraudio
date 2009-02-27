//vio_cmd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

int roar_vio_open_cmd(struct roar_vio_calls * calls, struct roar_vio_calls * dst,
                      int flags, char * reader, char * writer, int options) {
 struct roar_vio_cmd_state * state;

 if ( calls == NULL || dst == NULL )
  return -1;

 if ( flags == 0 )
  return -1;

 if ( reader == NULL && writer == NULL )
  return -1;

 if ( (state = malloc(sizeof(struct roar_vio_cmd_state))) == NULL )
  return -1;

 // clear all
 memset(calls, 0, sizeof(struct roar_vio_calls));
 memset(state, 0, sizeof(struct roar_vio_cmd_state));

 // init reader and writer:
 state->reader.pid = -1;
 state->reader.in  = -1;
 state->reader.out = -1;

 if ( reader != NULL )
  state->reader.cmd = strdup(reader);

 state->writer.pid = -1;
 state->writer.in  = -1;
 state->writer.out = -1;

 if ( writer != NULL )
  state->writer.cmd = strdup(writer);

 // init state
 state->next    = dst;
 state->flags   = flags;
 state->options = options;

 // init calls
 calls->inst = (void*) state;

 if ( !(options & ROAR_VIO_CMD_OPTS_ON_DEMAND) ) {
  if ( reader != NULL )
   if ( roar_vio_cmd_fork(&(state->reader)) == -1 )
    return roar_vio_cmd_close(calls);

  if ( writer != NULL )
   if ( roar_vio_cmd_fork(&(state->writer)) == -1 )
    return roar_vio_cmd_close(calls);
 }

 return 0;
}

int roar_vio_cmd_close(struct roar_vio_calls * vio) {
 struct roar_vio_cmd_state * state = (struct roar_vio_cmd_state *)vio->inst;

 if ( state->reader.opened )
  roar_vio_cmd_wait(&(state->reader));

 if ( state->writer.opened )
  roar_vio_cmd_wait(&(state->writer));

 if ( state->reader.cmd != NULL )
  free(state->reader.cmd);

 if ( state->writer.cmd != NULL )
  free(state->writer.cmd);

 roar_vio_close(state->next);

 free(state);

 return 0;
}

int roar_vio_cmd_fork(struct roar_vio_cmd_child * child) {
 int in[2], out[2];

 if ( child == NULL )
  return -1;

 if ( child->opened )
  return 0;

 if ( child->cmd == NULL )
  return -1;

 // open some pipes...
 if ( pipe(in) != 0 )
  return -1;

 if ( pipe(out) != 0 ) {
  close(in[0]);
  close(in[1]);
  return -1;
 }

 child->pid = fork();

 switch (child->pid) {
  case -1:
    close(in[0]);
    close(in[1]);
    close(out[0]);
    close(out[1]);
    return -1;
   break;
  case 0:
    close(in[0]);
    close(out[1]);
    close(ROAR_STDIN);
    close(ROAR_STDOUT);

    if ( dup2(out[0], ROAR_STDIN) == -1 )
     _exit(1);

    if ( dup2(in[1], ROAR_STDOUT) == -1 )
     _exit(1);

    execlp("/bin/sh", "/bin/sh", "-c", child->cmd, NULL);

    _exit(1);
   break;
 }

 close(in[1]);
 close(out[0]);

 child->opened = 1;
 child->in     = in[0];
 child->out    = out[1];

 return 0;
}

int roar_vio_cmd_wait(struct roar_vio_cmd_child * child) {
 int status;

 if ( child == NULL )
  return -1;

 if ( !child->opened )
  return 0;

 if ( child->out != -1 )
  close(child->out);

 if ( child->in  != -1 )
  close(child->in);

 waitpid(child->pid, &status, 0);

 return 0;
}

// VIOs:

ssize_t roar_vio_cmd_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_cmd_state * state = (struct roar_vio_cmd_state *)vio->inst;
 fd_set rfhs[1], wfhs[1];
 struct timeval tv;
 size_t done = 0;
 int max_fh;
 int ret;
 char   tbuf[ROAR_VIO_CMD_BUFSIZE];
 char * tp   = NULL;
 size_t tlen = 0;

 if ( !state->reader.opened ) {
  if ( buf == NULL && count == 0 ) /* sync: no need to do anything if no reader is forked :) */
   return 0;

  if ( !(state->options & ROAR_VIO_CMD_OPTS_ON_DEMAND) ) /* we are not on demand and no reader exists? -> err */
   return -1;

  if ( roar_vio_cmd_fork(&(state->reader)) == -1 )
   return -1;
 }

 while (done < count) {
  if ( state->options & ROAR_VIO_CMD_OPTS_NONBLOCK ) {
   tv.tv_sec  =    0;
   tv.tv_usec =    1;
  } else {
   tv.tv_sec  = 3600;
   tv.tv_usec =    0;
  }

  FD_ZERO(rfhs);
  FD_ZERO(wfhs);

  FD_SET(state->reader.in,  rfhs);
  FD_SET(state->reader.out, wfhs);

  max_fh = state->reader.in > state->reader.out ? state->reader.in : state->reader.out;

  if ( (ret = select(max_fh + 1, rfhs, wfhs, rfhs, &tv)) == -1 )
   return -1;

  if ( ret > 0 ) {
   if ( FD_ISSET(state->reader.in, rfhs) ) {
    if ( (ret = read(state->reader.in, buf+done, count-done)) == -1 )
     break;

    if ( ret == 0 )
     break;

    done += ret;
   }

   if ( FD_ISSET(state->reader.out, wfhs) ) {
    if ( !tlen ) {
     tp = tbuf;
     if ( (tlen = roar_vio_read(state->next, tp, ROAR_VIO_CMD_BUFSIZE)) == -1 )
      tlen = 0;
    }

    if ( tlen ) {
     if ( (ret = write(state->reader.out, tp, tlen)) > 0 ) {
      tlen -= ret;
      tp   += ret;
     }
    }
   }
  }

  if ( state->options & ROAR_VIO_CMD_OPTS_NONBLOCK )
   break;
 }

 if ( tlen ) { /* we have some data to write to the child... */
  // TODO: try to write it out...
  return -1;
 }

 return done;
}

ssize_t roar_vio_cmd_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 struct roar_vio_cmd_state * state = (struct roar_vio_cmd_state *)vio->inst;

 if ( !state->writer.opened ) {
  if ( buf == NULL && count == 0 ) /* sync: no need to do anything if no writer is forked :) */
   return 0;

  if ( !(state->options & ROAR_VIO_CMD_OPTS_ON_DEMAND) ) /* we are not on demand and no writer exists? -> err */
   return -1;

  if ( roar_vio_cmd_fork(&(state->writer)) == -1 )
   return -1;
 }


 return -1;
}

int     roar_vio_cmd_nonblock(struct roar_vio_calls * vio, int state) {
 struct roar_vio_cmd_state * self = (struct roar_vio_cmd_state *)vio->inst;

 self->options |= ROAR_VIO_CMD_OPTS_NONBLOCK;

 if ( state == ROAR_SOCKET_BLOCK )
  self->options -= ROAR_VIO_CMD_OPTS_NONBLOCK;

 roar_vio_nonblock(self->next, state); // this should help, but may not nessessery.

 return 0;
}

int     roar_vio_cmd_sync    (struct roar_vio_calls * vio) {
 struct roar_vio_cmd_state * state = (struct roar_vio_cmd_state *)vio->inst;
 int oldblock;
 int ret = 0;

 oldblock = state->options & ROAR_VIO_CMD_OPTS_NONBLOCK ? ROAR_SOCKET_NONBLOCK : ROAR_SOCKET_BLOCK;

 if ( roar_vio_cmd_nonblock(vio, ROAR_SOCKET_BLOCK) == -1 )
  return -1;

 if ( roar_vio_cmd_write(vio, NULL, 0) == -1 )
  ret = -1;

 if ( roar_vio_cmd_read(vio, NULL, 0) == -1 )
  ret = -1;

 if ( roar_vio_cmd_nonblock(vio, oldblock) == -1 )
  return -1;

 return ret;
}

//ll
