//xmalloc.c:

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
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include <libroarpulse/libroarpulse.h>

/** Allocate the specified number of bytes, just like malloc() does. However, in case of OOM, terminate */
void* pa_xmalloc(size_t l) {
 return roar_mm_malloc(l);
}

/** Same as pa_xmalloc(), but initialize allocated memory to 0 */
void *pa_xmalloc0(size_t l) {
 void * data = roar_mm_malloc(l);

 if ( data == NULL )
  return NULL;

 memset(data, 0, l);

 return data;
}

/**  The combination of pa_xmalloc() and realloc() */
void *pa_xrealloc(void *ptr, size_t size) {
 return roar_mm_realloc(ptr, size);
}

/** Free allocated memory */
void pa_xfree(void *p) {
 return roar_mm_free(p);
}

/** Duplicate the specified string, allocating memory with pa_xmalloc() */
char *pa_xstrdup(const char *s) {
 return roar_mm_strdup(s);
}

/** Duplicate the specified string, but truncate after l characters */
char *pa_xstrndup(const char *s, size_t l) {
 size_t i;
 char * data;

 for (i = 0; i < l && s[i] != 0; i++);

 if ( (data = roar_mm_malloc(i+1)) == NULL )
  return NULL;

 memcpy(data, s, i);
 data[i] = 0;

 return data;
}

/** Duplicate the specified memory block */
void* pa_xmemdup(const void *p, size_t l) {
 void * data = roar_mm_malloc(l);

 if ( data == NULL )
  return NULL;

 memcpy(data, p, l);

 return data;
}

//ll
