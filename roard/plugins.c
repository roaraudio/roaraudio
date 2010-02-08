//plugins.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of roard a part of RoarAudio,
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
 */

#include "roard.h"

#define MAX_PLUGINS    8

static struct roar_dl_lhandle * g_plugins[MAX_PLUGINS];

static struct roar_dl_lhandle ** _find_free(void) {
 int i;

 for (i = 0; i < MAX_PLUGINS; i++) {
  if ( g_plugins[i] == NULL )
   return &(g_plugins[i]);
 }

 return NULL;
}

int plugins_preinit  (void) {
 memset(g_plugins, 0, sizeof(g_plugins));

 return 0;
}

int plugins_init  (void) {
 int i;

 for (i = 0; i < MAX_PLUGINS; i++) {
  if ( g_plugins[i] != NULL ) {
   roar_dl_ra_init(g_plugins[i], NULL);
  }
 }

 return 0;
}

int plugins_free  (void) {
 int i;

 for (i = 0; i < MAX_PLUGINS; i++) {
  if ( g_plugins[i] != NULL ) {
   roar_dl_close(g_plugins[i]);
  }
 }

 return plugins_preinit();
}

int plugins_load  (const char * filename) {
 struct roar_dl_lhandle ** next = _find_free();

 if ( next == NULL )
  return -1;

 *next = roar_dl_open(filename, -1, 0 /* we delay this until plugins_init() */);
 if ( *next == NULL )
  return -1;

 return 0;
}

//ll
