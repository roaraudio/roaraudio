//util.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

/** Return the binary file name of the current process. This is not
 * supported on all architectures, in which case NULL is returned. */
char *pa_get_binary_name(char *s, size_t l) {
#ifdef __linux__
 int ret;
 char path[PATH_MAX];

 if ( (ret = readlink("/proc/self/exe", path, PATH_MAX-1)) != -1 ) {
  path[ret] = 0;
  return strncpy(s, pa_path_get_filename(path), l);
 }
#endif

 return NULL;
}

/** Return a pointer to the filename inside a path (which is the last
 * component). */
// some versions declare this as const char * f(...)
// and newer(?) versions as char * f(...)...
ROAR_HAVE_TYPE_PA_PATH_GET_FILENAME pa_path_get_filename(const char *p) {
 char * r;

 if ( (r = strrchr(p, '/')) ) {
  return (const char *) r+1;
 } else {
  return p;
 }

}

/** Return the current username in the specified string buffer. */
char *pa_get_user_name(char *s, size_t l);

/** Return the current hostname in the specified buffer. */
char *pa_get_host_name(char *s, size_t l);

/** Return the fully qualified domain name in s */
char *pa_get_fqdn(char *s, size_t l);

/** Return the home directory of the current user */
char *pa_get_home_dir(char *s, size_t l) {
 char * home = getenv("HOME");

 if ( home == NULL )
  return NULL;

 strncpy(s, home, l);

 s[l-1] = 0;

 return s;
}

/** Wait t milliseconds */
int pa_msleep(unsigned long t) {
#ifdef ROAR_TARGET_WIN32
 Sleep(t);
#else
 usleep(1000*t);
#endif
 return 0;
}

//ll
