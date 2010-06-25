//env.c:

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

int roar_env_set(struct roar_keyval * keyval);

const char * roar_env_get_home(int level) {
 const char * home = getenv("HOME");

 return home;
}

int roar_env_get_home_r(int level, char * str, size_t len) {
 size_t homelen;
 const char * home;

 if ( len == 0 )
  return 0;

 if ( str == NULL )
  return -1;

 *str = 0; // write a single termination byte to be safe...

 home = roar_env_get_home(level);

 if ( home == NULL )
  return -1;

 homelen = strlen(home);

 if ( (len - 1) < homelen )
  return -1;

 strncpy(str, home, len);
 str[homelen] = 0;

 return 0;
}

int roar_env_render_path_r(char * out, size_t len, const char * inpath) {
 size_t inlen;
 size_t needlen = 0;
 size_t homelen = 0;
 const char * home;
 enum { UNKNOWN, COPY, HOME } mode = UNKNOWN;

 if ( len == 0 && inpath == NULL )
  return 0;

 if ( inpath == NULL )
  return -1;

 inlen = strlen(inpath);

 if ( len == 0 && inlen == 0 )
  return 0;

 if ( len == 0 )
  return -1;

 if ( out == NULL )
  return -1;

 home = roar_env_get_home(0);

 if ( home != NULL )
  homelen = strlen(home);

 if ( inpath[0] == '/' ) {
  needlen = inlen;
  mode    = COPY;
 } else if ( inpath[0] == '~' && inpath[1] == '/' ) {
  if ( homelen == 0 ) /* we don't know our $HOME */
   return -1;

  needlen = inlen + homelen - 1;
  mode    = HOME;
 }

 if ( len < (needlen + 1) )
  return -1;

 switch (mode) {
  case COPY:
    strncpy(out, inpath, len);
    out[len-1] = 0;
   break;
  case HOME:
    strcpy(out, home);
    strcat(out, inpath+1); // strip only ~, so we have the / if home is not /-terminated.
   break;
  default:
    return -1;
   break;
 }

 return 0;
}

//ll
