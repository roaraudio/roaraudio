//file.h:

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

#ifndef _LIBROARFILE_H_
#define _LIBROARFILE_H_

#include "libroar.h"

#ifdef ROAR_HAVE_LINUX_SENDFILE
#include <sys/sendfile.h>
#endif

int     roar_file_codecdetect(char * buf, int len);
ssize_t roar_file_send_raw   (int out, int in);
ssize_t roar_file_play       (struct roar_connection * con, char * file, int exec);
ssize_t roar_file_play_full  (struct roar_connection * con, char * file, int exec, int passfh, struct roar_stream * s);

ssize_t roar_file_map        (char * filename, int flags, mode_t mode, size_t len, void ** mem);
int     roar_file_unmap      (size_t len, void * mem);

char  * roar_cdromdevice     (void);

#endif

//ll
