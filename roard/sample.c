//sample.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

int samples_init      (void) {
 int i;

 for (i = 0; i < ROAR_SAMPLES_MAX; i++)
  g_samples[i] = NULL;

 return 0;
}


int samples_free      (void) {
 int i;

 for (i = 0; i < ROAR_SAMPLES_MAX; i++)
  if ( g_samples[i] != NULL )
   samples_delete(i);

 return 0;
}

int samples_new       (void) {
 struct roar_sample * c = malloc(sizeof(struct roar_sample));
 int i;

 if ( c == NULL )
  return -1;

 c->name[0] = 0;
 c->data    = NULL;

 for (i = 0; i < ROAR_SAMPLES_MAX; i++) {
  if ( g_samples[i] == NULL ) {
   g_samples[i] = c;
   return i;
  }
 }

 free(c);

 return -1;
}

int samples_delete    (int id) {
 struct roar_sample * c = g_samples[id];

 if ( c == NULL )
  return -1;

 if (c->data != NULL )
  roar_buffer_free(c->data);

 free(c);

 g_samples[id] = NULL;

 return 0;
}

int samples_set_name  (int id, char * name) {
 struct roar_sample * c = g_samples[id];

 if ( c == NULL )
  return -1;

 strncpy(c->name, name, ROAR_BUFFER_NAME-1);

 c->name[ROAR_BUFFER_NAME-1] = 0;

 return 0;
}

int samples_add_data  (int id, void * data, size_t len) {
 struct roar_buffer * new;
 struct roar_sample * c = g_samples[id];

 if ( c == NULL )
  return -1;

 if ( roar_buffer_new(&new, len) == -1 )
  return -1;

 if ( c->data == NULL ) {
  c->data = new;
 } else {
  roar_buffer_add(c->data, new);
 }

 return 0;
}

//ll
