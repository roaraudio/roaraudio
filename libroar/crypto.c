//crypto.c:

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

int roar_crypto_init (void) {
 static int inited = 0;

 if ( inited )
  return 0;

#ifdef ROAR_HAVE_LIBGCRYPT
 if ( !gcry_control(GCRYCTL_INITIALIZATION_FINISHED_P) ) {
  ROAR_INFO("roar_crypto_init(void): libgcrypt not yet inited, initing it now.", ROAR_DBG_INFO_INFO);
  if ( !gcry_check_version(GCRYPT_VERSION) ) {
   ROAR_ERR("roar_crypto_init(void): libgcrypt version missmatch");
   return -1;
  }

  // what about SECMEM?

  gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
 }
#endif

 inited = 1;
 return 0;
}

//ll
