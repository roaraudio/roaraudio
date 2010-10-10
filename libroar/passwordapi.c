//passwordapi.c:

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

ssize_t roar_passwd_simple_ask_pw (char ** pw, char * prompt, const char * cachetoken) {
#ifdef ROAR_SUPPORT_PASSWORD_API
 struct roar_pinentry pe;

 if ( pw == NULL )
  return -1;

 if ( prompt == NULL ) {
  ROAR_WARN("roar_passwd_simple_ask_pw(pw=%p, prompt=NULL, cachetoken='%s'): No prompt given. This may be a bug in application.");
  prompt = "Please enter Password";
 }

 *pw = NULL;

 // TODO: test for cache here.

 if ( *pw == NULL ) {
  if ( roar_pinentry_simple_open(&pe) != -1 ) {
   if ( roar_pinentry_getpin(&pe, pw, NULL, prompt) == -1 ) {
    *pw = NULL;
   }
   roar_pinentry_close(&pe);
  }
 }

 if ( *pw == NULL ) {
  if ( roar_sshaskpass_getpass(pw, prompt) == -1 ) {
   *pw = NULL;
  }
 }

 if ( *pw == NULL )
  return -1;

 // TODO: save to cache here.

 return strlen(*pw);
#else
 return -1;
#endif
}

//ll
