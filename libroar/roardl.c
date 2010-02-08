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

#if defined(ROAR_HAVE_LIBDL) && !defined(RTLD_NEXT)
#define RTLD_NEXT ((void *) -1L)
#endif

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

 // early errors just return.

 if ( flags != ROAR_DL_FLAG_DEFAUTS )
  return NULL;

 if ( (ret = roar_mm_malloc(sizeof(struct roar_dl_lhandle))) == NULL )
  return NULL;

 memset(ret, 0, sizeof(struct roar_dl_lhandle));

#if defined(ROAR_HAVE_LIBDL)
 flags  = RTLD_NOW;

#ifdef RTLD_DEEPBIND
 flags |= RTLD_DEEPBIND;
#endif

 ret->handle = dlopen(filename, flags);

 if ( ret->handle == NULL ) {
  roar_mm_free(ret);
  return NULL;
 }
#else
 roar_mm_free(ret);
 return NULL;
#endif

 if ( ret == NULL )
  return NULL;

 if ( ra_init ) {
  roar_dl_ra_init(ret, NULL);
 }

 return ret;
}

int                      roar_dl_close(struct roar_dl_lhandle * lhandle) {
 int ret = -1;

 if ( lhandle == ROAR_DL_HANDLE_DEFAULT )
  return -1;
 if ( lhandle == ROAR_DL_HANDLE_NEXT )
  return -1;

 if ( lhandle->lib != NULL && lhandle->lib->unload != NULL )
  lhandle->lib->unload(lhandle->para, lhandle->lib);

#if defined(ROAR_HAVE_LIBDL)
 ret = dlclose(_roardl2ldl(lhandle));
#else
 ret = -1;
#endif

 roar_mm_free(lhandle);

 return ret;
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

 if ( (void*)lhandle < (void*)128 ) {
  if ( prefix == NULL )
   return -1;
 } else {
  para = lhandle->para;
 }


 if ( prefix != NULL ) {
  strcpy(name, "_");
  strcat(name, prefix);
  strcat(name, _SUFFIX);
 }

 ROAR_DBG("roar_dl_ra_init(lhandle=%p, prefix='%s'): name='%s'", lhandle, prefix, name);

 if ( (func = roar_dl_getsym(lhandle, name, -1)) == NULL )
  return -1;

 ROAR_DBG("roar_dl_ra_init(lhandle=%p, prefix='%s'): func=%p", lhandle, prefix, func);

 lib = func(para);

 if ( lib == NULL )
  return -1;

 if ( lib->version != ROAR_DL_LIBINST_VERSION )
  return -1;

 if ( sizeof(struct roar_dl_libraryinst) > lib->len )
  return -1;

 if ( !((void*)lhandle < (void*)128) ) {
  lhandle->lib = lib;
 }

 for (i = 0; i < ROAR_DL_FN_MAX; i++) {
  if ( lib->func[i] != NULL )
   lib->func[i](para, lib);
 }

 return 0;
}

char *                   roar_dl_errstr(struct roar_dl_lhandle * lhandle) {
#if defined(ROAR_HAVE_LIBDL)
 return dlerror();
#else
 return NULL;
#endif
}

//ll
