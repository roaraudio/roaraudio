//pipetest.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <roaraudio.h>

#define BUF_MAX 1024

int main (void) {
 char buf[BUF_MAX];
 size_t len;
 struct roar_vio_calls s0, s1;
 struct roar_vio_calls * me;

 if ( roar_vio_open_pipe(&s0, &s1, ROAR_VIO_PIPE_TYPE_AUTO, O_RDWR) == -1 ) {
  fprintf(stderr, "Error: can not create pipe\n");
 }

 switch (fork()) {
  case 0:
    me = &s0;
    roar_vio_close(&s1);
   break;
  default:
    me = &s1;
    roar_vio_close(&s0);
 }

 snprintf(buf, BUF_MAX, "Hello from pid %i\n", getpid());
 roar_vio_write(me, buf, strlen(buf));

 buf[0] = 0;

 roar_vio_read(me, buf, BUF_MAX);
 printf("This is pid %i: %s", getpid(), buf);

 roar_vio_close(me);

 return 0;
}

//ll
