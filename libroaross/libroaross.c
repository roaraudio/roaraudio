//libroaross.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#include "roaraudio.h"

#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
#if defined(__OpenBSD__) || defined(__NetBSD__)
#include <soundcard.h>
#else
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>

#ifdef ROAR_HAVE_H_SYS_TYPES
#include <sys/types.h>
#endif

#ifdef ROAR_HAVE_H_FCNTL
#include <fcntl.h>
#endif

#ifdef ROAR_HAVE_H_UNISTD
#include <unistd.h>
#endif

#include <sys/stat.h>

#define _MAX_POINTER  8

struct session {
 int refc;
 struct roar_connection con;
};

static struct session _session = {.refc = 0};

struct handle {
 int refc; // refrence counter
 struct session * session;
};

static struct {
 int     (*open)(const char *pathname, int flags, mode_t mode);
 int     (*close)(int fd);
 ssize_t (*write)(int fd, const void *buf, size_t count);
 ssize_t (*read)(int fd, void *buf, size_t count);
} _os;

static struct {
 int fh;
 struct handle * handle;
} _ptr[_MAX_POINTER];

static void _init_os (void) {
 memset(&_os, 0, sizeof(_os));
}

static void _init_ptr (void) {
 int i;

 for (i = 0; i < _MAX_POINTER; i++) {
  _ptr[i].fh = -1;
 }
}

static void _init (void) {
 static int inited = 0;

 if ( !inited ) {
  _init_os();
  _init_ptr();
  inited++;
 }
}

static int _open_dummy (void) {
 int p[2];

 if ( pipe(p) == -1 )
  return -1;

 close(p[1]);

 return p[0];
}

static struct session * _open_session (char * server, char * name) {
 if ( _session.refc == 0 ) {

  if ( name == NULL )
   name = "libroaross client";

  if ( roar_simple_connect(&(_session.con), server, name) == -1 )
   return NULL;

  _session.refc++;
 } else {
  _session.refc++;
  return &_session;
 }
}

static void _close_session(struct session * session) {
 if ( session == NULL )
  return;

 session->refc--;

 if ( session->refc == 0 ) {
  roar_disconnect(&(session->con));
 }
}


// -------------------------------------
// emulated functions follow:
// -------------------------------------

int     open(const char *pathname, int flags, ...) {
 mode_t mode = 0;
 va_list args;

 _init();

 if (flags & O_CREAT) {
  va_start(args, flags);
  mode = va_arg(args, mode_t);
  va_end(args);
 }

 return _os.open(pathname, flags, mode);
}

int     close(int fd) {
 _init();

 return _os.close(fd);
}

ssize_t write(int fd, const void *buf, size_t count) {
 _init();

 return _os.write(fd, buf, count);
}

ssize_t read(int fd, void *buf, size_t count) {
 _init();

 return _os.read(fd, buf, count);
}

#endif

//ll
