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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#define MAX_PLUGINS    8

static struct _roard_plugin {
 struct roar_dl_lhandle     * lhandle;
 struct roard_plugins_sched * sched;
} g_plugins[MAX_PLUGINS];
static struct _roard_plugin * _pp = NULL;

static struct _roard_plugin * _find_free(void) {
 int i;

 for (i = 0; i < MAX_PLUGINS; i++) {
  if ( g_plugins[i].lhandle == NULL ) {
   memset(&(g_plugins[i]), 0, sizeof(struct _roard_plugin));
   return &(g_plugins[i]);
  }
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
  if ( g_plugins[i].lhandle != NULL ) {
   _pp = &(g_plugins[i]);

   roar_dl_ra_init(g_plugins[i].lhandle, NULL);

   if ( g_plugins[i].sched->init != NULL )
    g_plugins[i].sched->init();

   _pp = NULL;
  }
 }

 return 0;
}

int plugins_free  (void) {
 int i;

 for (i = 0; i < MAX_PLUGINS; i++) {
  if ( g_plugins[i].lhandle != NULL ) {
   if ( g_plugins[i].sched->free != NULL )
    g_plugins[i].sched->free();

   roar_dl_close(g_plugins[i].lhandle);
  }
 }

 return plugins_preinit();
}

int plugins_update   (void) {
 int ret = 0;
 int i;

 for (i = 0; i < MAX_PLUGINS; i++)
  if ( g_plugins[i].lhandle != NULL )
   if ( g_plugins[i].sched->update != NULL )
    if ( g_plugins[i].sched->update() == -1 )
     ret = -1;

 return ret;
}

int plugins_load  (const char * filename) {
 struct _roard_plugin * next = _find_free();

 if ( next == NULL )
  return -1;

 next->lhandle = roar_dl_open(filename, -1, 0 /* we delay this until plugins_init() */);
 if ( next->lhandle == NULL ) {
  ROAR_ERR("plugins_load(filename='%s'): can not load plugin: %s", filename, roar_dl_errstr(NULL));
  return -1;
 }

 return 0;
}

int plugins_reg_sched(struct roard_plugins_sched * sched) {
 if ( _pp == NULL )
  return -1;

 _pp->sched = sched;

 return 0;
}

//ll
