//vio_cmd.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
                      char * reader, char * writer, int options) {
 struct roar_vio_cmd_state * state;

 if ( calls == NULL || dst == NULL )
  return -1;

 if ( reader == NULL && writer == NULL )
  return -1;

 if ( (state = malloc(sizeof(struct roar_vio_cmd_state))) == NULL )
  return -1;

 ROAR_DBG("roar_vio_open_cmd(*): pre reqs are OK");

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
 state->options = options;
 state->state   = ROAR_VIO_CMD_STATE_OPEN;

 // init calls
 calls->close    = roar_vio_cmd_close;
 calls->read     = roar_vio_cmd_read;
 calls->write    = roar_vio_cmd_write;
 calls->nonblock = roar_vio_cmd_nonblock;
 calls->sync     = roar_vio_cmd_sync;
 calls->inst     = (void*) state;

 ROAR_DBG("roar_vio_open_cmd(*): var setup OK");

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

 state->state = ROAR_VIO_CMD_STATE_CLOSING;

 if ( state->writer.opened ) {
  if ( state->writer.out != -1 ) {
   close(state->writer.out);
   state->writer.out = -1;
  }
 }

 roar_vio_cmd_sync(vio);

 if ( state->reader.opened )
  roar_vio_cmd_wait(&(state->reader));

 if ( state->writer.opened )
  roar_vio_cmd_wait(&(state->writer));

 if ( state->reader.cmd != NULL )
  free(state->reader.cmd);

 if ( state->writer.cmd != NULL )
  free(state->writer.cmd);

 roar_vio_close(state->next);

// state->state = ROAR_VIO_CMD_STATE_CLOSED;
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
 int    nonblock = state->options & ROAR_VIO_CMD_OPTS_NONBLOCK;
 int    in, out;

 ROAR_DBG("roar_vio_cmd_read(*) = ?");

 if ( !state->reader.opened ) {
  if ( buf == NULL && count == 0 ) /* sync: no need to do anything if no reader is forked :) */
   return 0;

  if ( !(state->options & ROAR_VIO_CMD_OPTS_ON_DEMAND) ) /* we are not on demand and no reader exists? -> err */
   return -1;

  if ( roar_vio_cmd_fork(&(state->reader)) == -1 )
   return -1;
 }

 in  = state->reader.in;
 out = state->reader.out;

 while (done < count) {
  if ( nonblock ) {
   tv.tv_sec  =    0;
   tv.tv_usec =    1;
  } else {
   tv.tv_sec  = 3600;
   tv.tv_usec =    0;
  }

  FD_ZERO(rfhs);
  FD_ZERO(wfhs);

  FD_SET(in,  rfhs);

  if ( out != -1 ) {
   FD_SET(out, wfhs);
  }

#ifdef DEBUG
  if ( FD_ISSET(in, rfhs) ) {
   ROAR_DBG("roar_vio_cmd_read(*): reader set in fh group");
  }
#endif

  max_fh = in > out ? in : out;
  ROAR_DBG("roar_vio_cmd_read(*): max_fh=%i", max_fh);

  if ( (ret = select(max_fh + 1, rfhs, wfhs, NULL, &tv)) == -1 )
   return -1;

  ROAR_DBG("roar_vio_cmd_read(*): select(*) = %i", ret);
  ROAR_DBG("roar_vio_cmd_read(*): reader=%i, writer=%i", in, out);

  if ( ret > 0 ) {
   if ( FD_ISSET(in, rfhs) ) {
    ROAR_DBG("roar_vio_cmd_read(*): event on reader");

    if ( (ret = read(in, buf+done, count-done)) == -1 )
     break;

    if ( ret == 0 )
     break;

    done += ret;
   }

   if ( out != -1 && FD_ISSET(out, wfhs) ) {
    ROAR_DBG("roar_vio_cmd_read(*): event on writer");

    if ( !tlen ) {
     tp   = tbuf;
     tlen = 0;
     if ( (tlen = roar_vio_read(state->next, tp, ROAR_VIO_CMD_BUFSIZE)) == -1 )
      continue;
    }

    if ( tlen ) {
     if ( (ret = write(out, tp, tlen)) > 0 ) {
      tlen -= ret;
      tp   += ret;
     }
    } else {
     close(out);
     state->reader.out = out = -1;
    }
   }
  }

  if ( nonblock )
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
 fd_set rfhs[1], wfhs[1];
 struct timeval tv;
 size_t done = 0;
 int max_fh;
 int ret;
 char   tbuf[ROAR_VIO_CMD_BUFSIZE];
 int    nonblock = state->options & ROAR_VIO_CMD_OPTS_NONBLOCK;
 int    in, out;

 if ( !state->writer.opened ) {
  if ( buf == NULL && count == 0 ) /* sync: no need to do anything if no writer is forked :) */
   return 0;

  if ( !(state->options & ROAR_VIO_CMD_OPTS_ON_DEMAND) ) /* we are not on demand and no writer exists? -> err */
   return -1;

  if ( roar_vio_cmd_fork(&(state->writer)) == -1 )
   return -1;
 }

 in  = state->writer.in;
 out = state->writer.out;

 if ( buf == NULL ) { // we are requested to sync
  if ( in != -1 ) {
   ret = 1;
   done = 0;
   while (ret > 0) {
    if ( state->state == ROAR_VIO_CMD_STATE_CLOSING ) {
     tv.tv_sec  = 3600;
     tv.tv_usec = 0;
    } else {
     tv.tv_sec  = 0;
     tv.tv_usec = done ? 1 : 50000; // 50ms
    }

    done++;

    FD_ZERO(rfhs);
    FD_SET(in, rfhs);

    if ( select(in+1, rfhs, NULL, NULL, &tv) < 1 )
     break;

    ret = read(in, tbuf, ROAR_VIO_CMD_BUFSIZE);

    if ( roar_vio_write(state->next, tbuf, ret) != ret )
     return -1;
   }
  }

  return 0;
 }

 while (done < count) {
  if ( nonblock ) {
   tv.tv_sec  =    0;
   tv.tv_usec =    1;
  } else {
   tv.tv_sec  = 3600;
   tv.tv_usec =    0;
  }

  FD_ZERO(rfhs);
  FD_ZERO(wfhs);

  FD_SET(out, wfhs);

  if ( in != -1 ) {
   FD_SET(in,  rfhs);
  }

  max_fh = in > out ? in : out;

  if ( (ret = select(max_fh + 1, rfhs, wfhs, NULL, &tv)) == -1 )
   return -1;

  if ( ret > 0 ) {
   if ( FD_ISSET(out, wfhs) ) {

    if ( (ret = write(out, buf+done, count-done)) == -1 )
     break;

    if ( ret == 0 )
     break;

    done += ret;
   }
   if ( in != -1 && FD_ISSET(in, wfhs) ) {
    if ( (ret = read(in, tbuf, ROAR_VIO_CMD_BUFSIZE)) == -1 ) { /* error case: can not read on reader -> EOF */
     close(in);
     state->writer.in = in = -1;
     break;
    }

    if ( roar_vio_write(state->next, tbuf, ret) != ret )
     return -1;
   }
  }

  if ( nonblock )
   break;
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

// MISC:
int roar_vio_open_gzip(struct roar_vio_calls * calls, struct roar_vio_calls * dst, int level) {
#ifdef ROAR_HAVE_BIN_GZIP
 char   wbuf[80];
 char * writer = ROAR_HAVE_BIN_GZIP " -c";

 if ( level != -1 ) {
  snprintf(wbuf, 80, "%s -c%i", ROAR_HAVE_BIN_GZIP, level);
  writer = wbuf;
 }

 return roar_vio_open_cmd(calls, dst, ROAR_HAVE_BIN_GZIP " -dc", writer, ROAR_VIO_CMD_OPTS_ON_DEMAND);
#else
 return -1;
#endif
}

int roar_vio_open_gpg(struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * pw, int wronly, char * opts, int options) {
#ifdef ROAR_HAVE_BIN_GPG
 char command[1024];
 char para[1024] = {0};
 int pwpipe[2];
 int ret;

/*
#define ROAR_VIO_PGP_OPTS_NONE      0x00
#define ROAR_VIO_PGP_OPTS_ASCII     0x01
#define ROAR_VIO_PGP_OPTS_SIGN      0x02
#define ROAR_VIO_PGP_OPTS_TEXTMODE  0x04
*/

 if ( options & ROAR_VIO_PGP_OPTS_ASCII )
  strncat(para, "--armor ", 16);

 if ( options & ROAR_VIO_PGP_OPTS_SIGN )
  strncat(para, "--sign ", 16);

 if ( options & ROAR_VIO_PGP_OPTS_TEXTMODE )
  strncat(para, "--textmode ", 16);

 if ( pw != NULL ) {
  if ( pipe(pwpipe) == -1 )
   return -1;

  snprintf(command, 1024, "%s --batch --no-verbose --quiet --passphrase-repeat 0 --passphrase-fd %i %s %s", ROAR_HAVE_BIN_GPG, pwpipe[0], para, opts);

  write(pwpipe[1], pw, strlen(pw));

  close(pwpipe[1]);
 } else {
  snprintf(command, 1024, "%s --no-verbose --quiet %s %s", ROAR_HAVE_BIN_GPG, para, opts);
 }

 if ( wronly ) {
  ret = roar_vio_open_cmd(calls, dst, NULL, command, 0);
 } else {
  ret = roar_vio_open_cmd(calls, dst, command, NULL, 0);
 }

 if ( pw != NULL )
  close(pwpipe[0]);

 return ret;
#else
 return -1;
#endif
}

int roar_vio_open_pgp_decrypt(struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * pw) {
 return roar_vio_open_gpg(calls, dst, pw, 0, "-d", ROAR_VIO_PGP_OPTS_NONE);
}

int roar_vio_open_pgp_store(struct roar_vio_calls * calls, struct roar_vio_calls * dst, int options) {
 return roar_vio_open_gpg(calls, dst, NULL, 1, "--store", options);
}

int roar_vio_open_pgp_encrypt_sym(struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * pw, int options) {
 return roar_vio_open_gpg(calls, dst, pw, 1, "--symmetric", options);
}
int roar_vio_open_pgp_encrypt_pub(struct roar_vio_calls * calls, struct roar_vio_calls * dst, char * pw, int options, char * recipient) {
 char buf[1024];

 snprintf(buf, 1024, "-e -r %s", recipient);
 return roar_vio_open_gpg(calls, dst, pw, 1, buf, options);
}

//ll
