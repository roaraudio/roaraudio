//codecfilter_cmd.c:

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

#if defined(ROAR_HAVE_UNIX) && defined(ROAR_HAVE_BSDSOCKETS) && defined(ROAR_HAVE_IO_POSIX)
#define _CAN_OPERATE
#endif

#if !defined(ROAR_WITHOUT_CF_CMD) && defined(_CAN_OPERATE)

#define CMDMAXLEN 1024

int cf_cmd_open(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter) {
 int socks[2];
 int execed = -1;
 char cmd[CMDMAXLEN+1] = {0};
 char * tmp = NULL;
 char   tb[CMDMAXLEN+1];
 char * o = filter->options;
 int i;

 if ( !o )
  return -1;

 for (i = 0; i < CMDMAXLEN && *o != 0; i++, o++) {
  if ( *o == '%' ) {
//   printf("ol: *o='%c' (escape)\n", *o);
   tmp = NULL;

   o++;
   if ( *o == 0 ) {
    break;
   } else if ( *o == 'R' ) {
    tmp = tb;
    snprintf(tb, CMDMAXLEN, "%i", g_sa->rate);
   } else if ( *o == 'B' ) {
    tmp = tb;
    snprintf(tb, CMDMAXLEN, "%i", g_sa->bits);
   } else if ( *o == 'C' ) {
    tmp = tb;
    snprintf(tb, CMDMAXLEN, "%i", g_sa->channels);
   }

//   printf("*o='%c', tmp=%p\n", *o, tmp);

   if ( tmp ) {
    for (; i < CMDMAXLEN && *tmp != 0; i++, tmp++)
     cmd[i] = *tmp;
    i--;
   }
  } else {
//   printf("ol: *o='%c' (copy to cmd[i=%i])\n", *o, i);
   cmd[i] = *o;
  }
 }

 cmd[i+1] = 0;

 //printf("cmd='%s'\n", cmd);

 if ( socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1 ) {
  return -1;
 }

 if ( lib_run_bg(cmd, ROAR_STREAM(info)->fh, socks[1], ROAR_STDERR, socks, 2) == -1 )
  return -1;

 if ( info->client != -1 ) {
  execed = g_clients[info->client]->execed;

  if ( execed != -1 ) {
   if ( g_streams[execed] == info ) {
    g_clients[info->client]->fh = socks[0];
   } else {
    close(ROAR_STREAM(info)->fh);
   }
  } else {
   close(ROAR_STREAM(info)->fh);
  }
 } else {
  close(ROAR_STREAM(info)->fh);
 }

 ROAR_STREAM(info)->fh = socks[0];
 ROAR_STREAM(info)->info.codec = ROAR_CODEC_DEFAULT;
 close(socks[1]);

 info->codecfilter = -1;

 return 0;
}

#endif

//ll
