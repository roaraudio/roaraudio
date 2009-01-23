//libroar.h:

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

#ifndef _LIBROARSIMPLE_H_
#define _LIBROARSIMPLE_H_

#include "libroar.h"

int roar_simple_connect (struct roar_connection * con, char * server, char * name);

int roar_simple_stream  (int rate, int channels, int bits, int codec, char * server, int dir, char * name);
int roar_simple_stream_obj  (struct roar_stream * s, int rate, int channels, int bits, int codec, char * server, int dir, char * name);

int roar_simple_new_stream (struct roar_connection * con, int rate, int channels, int bits, int codec, int dir);
int roar_simple_new_stream_obj (struct roar_connection * con, struct roar_stream * s, int rate, int channels, int bits, int
codec, int dir);

int roar_simple_play_file(char * file, char * server, char * name);

int roar_simple_play    (int rate, int channels, int bits, int codec, char * server, char * name);
int roar_simple_monitor (int rate, int channels, int bits, int codec, char * server, char * name);
int roar_simple_record  (int rate, int channels, int bits, int codec, char * server, char * name);
int roar_simple_filter  (int rate, int channels, int bits, int codec, char * server, char * name);

int roar_simple_close(int fh);

int roar_simple_get_standby (int fh);

#endif

//ll
