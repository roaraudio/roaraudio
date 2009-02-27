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

 if ( reader != NULL )
  if ( roar_vio_cmd_fork(&(state->reader)) == -1 )
   return roar_vio_cmd_close(calls);

 if ( writer != NULL )
  if ( roar_vio_cmd_fork(&(state->writer)) == -1 )
   return roar_vio_cmd_close(calls);

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
 return -1;
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

//ll
