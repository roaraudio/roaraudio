//timeval.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from libpulse*. They are mostly copyrighted by:
 *  Lennart Poettering <poettering@users.sourceforge.net> and
 *  Pierre Ossman <drzeus@drzeus.cx>
 *
 *  This file is part of libroarpulse a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>

struct timeval *pa_gettimeofday(struct timeval *tv) {
#ifdef ROAR_HAVE_GETTIMEOFDAY
 if ( gettimeofday(tv, NULL) == -1 ) {
  return NULL;
 } else {
  return tv;
 }
#else
 return NULL;
#endif
}

pa_usec_t pa_timeval_diff(const struct timeval *a, const struct timeval *b);
int pa_timeval_cmp(const struct timeval *a, const struct timeval *b);
pa_usec_t pa_timeval_age(const struct timeval *tv);
struct timeval* pa_timeval_add(struct timeval *tv, pa_usec_t v) {
 unsigned long long int secs;

 if ( tv == NULL )
  return NULL;

 secs = v/1000000LLU;

 tv->tv_sec += secs;

 v -= secs*1000000LLU;

 tv->tv_usec += v;

 while ( tv->tv_usec > 1000000LLU ) {
  tv->tv_usec -= 1000000LLU;
  tv->tv_sec  += 1;
 }

 return tv;
}


//ll