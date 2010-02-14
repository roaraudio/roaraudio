//utf8.c:

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
#ifdef ROAR_HAVE_H_ICONV
#include <iconv.h>
#endif

/** Test if the specified strings qualifies as valid UTF8. Return the string if so, otherwise NULL */
const char *pa_utf8_valid(const char *str);

/** Filter all invalid UTF8 characters from the specified string, returning a new fully UTF8 valid string. Don't forget to free the returned string with pa_xfree() */
char *pa_utf8_filter(const char *str);

static char * _roar_pa_iconv(const char * str, const char * from, const char * to) {
#ifdef ROAR_HAVE_H_ICONV
 iconv_t cd;
 char   * out;
 char   * ip, * op;
 size_t   il,   ol;
 size_t inlen;
 size_t outlen;
 size_t ret;

 if ( str == NULL )
  return NULL;

 if ( from == NULL )
  from = "";

 if ( to == NULL )
  to = "";

 inlen = strlen(str);

 outlen = inlen * 1.2;

 if ( (out = pa_xmalloc(outlen)) == NULL )
  return NULL;

 if ( (cd = iconv_open(from, to)) == (iconv_t)(-1) )
  return NULL;

 while (1) {
  ip = (char*) str;
  op = out;
  il = inlen;
  ol = outlen;

  ret = iconv(cd, &ip, &il, &op, &ol);

  if ( ret != (size_t)-1 )
   break;

  if ( errno != E2BIG ) {
   pa_xfree(out);
   out = NULL;
   break;
  }

  outlen += il * 1.2;
  out = pa_xrealloc(out, outlen);
 }

 iconv_close(cd);

 return out;
#else
 return NULL;
#endif
}

/** Convert a UTF-8 string to the current locale. Free the string using pa_xfree(). */
char* pa_utf8_to_locale (const char *str) {
 return _roar_pa_iconv(str, "UTF-8", NULL);
}

/** Convert a string in the current locale to UTF-8. Free the string using pa_xfree(). */
char* pa_locale_to_utf8 (const char *str) {
 return _roar_pa_iconv(str, NULL, "UTF-8");
}

//ll
