//roardl.c:

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

#include "libroar.h"

#if defined(ROAR_HAVE_LIBDL)
static void * _roardl2ldl (struct roar_dl_lhandle * lhandle) {
 if ( lhandle == ROAR_DL_HANDLE_DEFAULT )
  return RTLD_DEFAULT;

 if ( lhandle == ROAR_DL_HANDLE_NEXT )
  return RTLD_NEXT;

 return lhandle->handle;
}
#endif

struct roar_dl_lhandle * roar_dl_open(const char * filename, int flags, int ra_init) {
 struct roar_dl_lhandle * ret = NULL;

 if ( ret == NULL )
  return NULL;

 if ( ra_init ) {
  roar_dl_ra_init(ret, NULL);
 }

 return ret;
}

int                      roar_dl_close(struct roar_dl_lhandle * lhandle) {
#if defined(ROAR_HAVE_LIBDL)
 return dlclose(_roardl2ldl(lhandle));
#else
 return -1;
#endif
}

void                   * roar_dl_getsym(struct roar_dl_lhandle * lhandle, const char * sym, int type) {
#if defined(ROAR_HAVE_LIBDL)
 return dlsym(_roardl2ldl(lhandle), sym);
#else
 return NULL;
#endif
}

int                      roar_dl_ra_init(struct roar_dl_lhandle * lhandle, const char * prefix) {
#define _SUFFIX "_roaraudio_library_init"
 char name[80] = _SUFFIX;
 struct roar_dl_libraryinst * (*func)(struct roar_dl_librarypara * para);
 struct roar_dl_libraryinst * lib;
 struct roar_dl_librarypara * para = NULL;
 int i;

 if ( (void*)lhandle < (void*)128 && prefix == NULL )
  return -1;

 if ( prefix != NULL ) {
  strcpy(name, "_");
  strcat(name, prefix);
  strcat(name, "_");
  strcat(name, _SUFFIX);
 }

 if ( (func = roar_dl_getsym(lhandle, name, -1)) == NULL )
  return -1;

 lib = func(para);

 if ( lib == NULL )
  return -1;

 if ( lib->version != ROAR_DL_LIBINST_VERSION )
  return -1;

 if ( sizeof(struct roar_dl_libraryinst) > lib->len )
  return -1;

 for (i = 0; i < ROAR_DL_FN_MAX; i++) {
  if ( lib->func[i] != NULL )
   lib->func[i](para, lib);
 }

 // do something with lib here.

 return -1;
}

//ll
