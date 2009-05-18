//signals.c:

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

// we should add signal handling on micro controllsers here.

#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_TARGET_WIN32)

void on_sig_int (int signum) {
 ROAR_DBG("got SIGINT");

 alive = 0;
 clean_quit();
}

void on_sig_chld (int signum) {
 ROAR_DBG("got SIGCHLD");

 wait(NULL);

 signal(SIGCHLD, on_sig_chld);
}

#endif


//ll
