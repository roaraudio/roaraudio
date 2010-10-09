//random.c:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
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

#ifdef ROAR_HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

static void roar_random_init (void) {
 static int inited = 0;

 if (inited)
  return;

 // add stuff here needed to bring up random source.

 inited = 1;
}

int roar_random_gen(void * buffer, size_t len, int quality) {
 if ( len == 0 )
  return 0;

 if ( buffer == NULL )
  return -1;

 roar_random_init();

 switch (quality) {
  case ROAR_RANDOM_NONE:
    // no entropy:
    memset(buffer, 0, len);
   break;
#ifdef ROAR_HAVE_LIBGCRYPT
  case ROAR_RANDOM_VERY_WEAK:
  case ROAR_RANDOM_WEAK:
    gcry_create_nonce(buffer, len);
   break;
  case ROAR_RANDOM_NORMAL:
  case ROAR_RANDOM_STRONG:
    gcry_randomize(buffer, len, GCRY_STRONG_RANDOM);
   break;
  case ROAR_RANDOM_VERY_STRONG:
    gcry_randomize(buffer, len, GCRY_VERY_STRONG_RANDOM);
   break;
#endif
  default:
    return -1;
   break;
 }

 return 0;
}

void * roar_random_genbuf(size_t len, int quality, int locked) {
 void * ret = roar_mm_malloc(len);

 if (ret == NULL)
  return NULL;

 if ( locked ) {
  if ( roar_mm_mlock(ret, len) == -1 ) {
   roar_mm_free(ret);
   return NULL;
  }
 }

 if ( roar_random_gen(ret, len, quality) == -1 ) {
  roar_mm_free(ret);
  return NULL;
 }

 return ret;
}

//ll
