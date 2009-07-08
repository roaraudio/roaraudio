//buffer.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

int roar_buffer_new      (struct roar_buffer ** buf, size_t len) {
 void * data;

 if ((data = malloc(len)) == NULL) {
  return -1;
 }

 if ( roar_buffer_new_no_ma(buf, len, data) == -1 ) {
  free(data);
  return -1;
 }

 if ( roar_buffer_set_flag(*buf, ROAR_BUFFER_FLAG_NOFREE, ROAR_BUFFER_RESET) == -1 ) {
  roar_buffer_free(*buf);
  free(data);
  return -1;
 }

 return 0;
}

int roar_buffer_new_no_ma(struct roar_buffer ** buf, size_t len, void * data) { // no internal malloc
 struct roar_buffer * new;

 ROAR_DBG("buffer_new(buf=%p, len=%i) = ?", buf, len);

 if ( buf == NULL || data == NULL )
  return -1;

 if ((new = malloc(sizeof(struct roar_buffer))) == NULL) {
  *buf = NULL;
  return -1;
 }

 new->data      = data;

 new->flags     = ROAR_BUFFER_FLAG_NONE|ROAR_BUFFER_FLAG_NOFREE;

 new->user_data = new->data;

 new->next      = NULL;

 new->len       = len;
 new->user_len  = len;
 *buf           = new;

 ROAR_DBG("buffer_new(buf=%p, len=%i): New buffer at %p", buf, len, new);

 return 0;
}

int roar_buffer_free     (struct roar_buffer * buf) {
 struct roar_buffer * next;

 if ( buf == NULL )
  return -1;

 while ((next = buf->next)) {
  free(buf->data);
  free(buf);
  buf = next;
 }

 if ( roar_buffer_get_flag(buf, ROAR_BUFFER_FLAG_NOFREE) != 1 )
  free(buf->data);

 free(buf);

 return 0;
}

int roar_buffer_delete   (struct roar_buffer * buf, struct roar_buffer ** next) {
 if ( buf == NULL ) {
  if ( next != NULL )
   *next = NULL;
  return -1;
 }

 ROAR_DBG("buffer_delete(buf=%p, next=%p) = ?", buf, next);

 if ( next != NULL )
  *next = buf->next;

 free(buf->data);
 free(buf);

 ROAR_DBG("buffer_delete(buf=%p, next=%p) = 0", buf, next);
 return 0;
}

int roar_buffer_add      (struct roar_buffer * buf, struct roar_buffer *  next) {
 unsigned int deep = 0;

 if ( buf == NULL )
  return -1;

 ROAR_DBG("buffer_add(buf=%p, next=%p) = ?", buf, next);

 if ( buf == next ) {
  ROAR_ERR("buffer_add(*): both pointer are of the same destination, This is a error in the application");
  return -1;
 }

 while ( buf->next != NULL ) {
  ROAR_DBG("buffer_add(*): buf=%p, next=%p (len=%i)", buf, buf->next, buf->user_len);
//  ROAR_DBG("buffer_add(): buf=%p, buf->next=%p", buf, buf->next);
  buf = buf->next;
  deep++;

  if ( buf == next ) {
   ROAR_ERR("buffer_add(*): Can not add buffer: loop detected at deep %u. This is a error in the application", deep);
  }
 }

 buf->next = next;

 ROAR_DBG("buffer_add(*): adding buffer at deep %u", deep);

 return 0;
}

int roar_buffer_get_next (struct roar_buffer *  buf, struct roar_buffer ** next) {
 if ( buf == NULL )
  return -1;

 *next = buf->next;

 return 0;
}

int roar_buffer_get_data (struct roar_buffer *  buf, void   ** data) {
 if ( buf == NULL )
  return -1;

 *data = buf->user_data;

 return 0;
}

int roar_buffer_set_offset (struct roar_buffer *  buf, size_t off) {
 if ( buf == NULL )
  return -1;

 buf->user_len  -= off;
 buf->user_data += off;

 return 0;
}

int roar_buffer_shift_out (struct roar_buffer ** buf, void * data, size_t * len) {
 size_t todo, cl;
 struct roar_buffer * cur;
 void * cd;

 if ( len == NULL || buf == NULL || data == NULL ) {
  ROAR_DBG("roar_buffer_shift_out(buf=%p, data=%p, len={%lu}) = -1 // Invalid input", buf, data, (unsigned long)len);
  return -1;
 }

 if ( *buf == NULL ) {
  ROAR_DBG("roar_buffer_shift_out(buf=%p, data=%p, len={%lu}) = -1 // Invalid pointer to buffer ring", buf, data, (unsigned long)len);
  return -1;
 }

 todo = *len;
 cur  = *buf;

 *len = 0;

 while (todo && cur != NULL) {
  ROAR_DBG("roar_buffer_shift_out(*): todo=%u, cur=%p", (unsigned int) todo, cur);

  if ( roar_buffer_get_len(cur, &cl) == -1 )
   return -1;

  if ( cl > todo ) {
   if ( roar_buffer_get_data(cur, &cd) == -1 )
    return -1;

   cl = todo;

   memcpy(data, cd, cl);
   todo -= cl;
   data += cl;
   *len += cl;

   if ( roar_buffer_set_offset(cur, cl) == -1 )
    return -1;
  } else {
   if ( roar_buffer_get_data(cur, &cd) == -1 )
    return -1;

   memcpy(data, cd, cl);
   todo -= cl;
   data += cl;
   *len += cl;

   if ( roar_buffer_next(&cur) == -1 )
    return -1;
  }

/*
  if ( cur == NULL )
   break;
*/
 }

 *buf = cur;

 return 0;
}

int roar_buffer_set_meta (struct roar_buffer * buf, void *  meta) {
 if ( buf == NULL )
  return -1;

 buf->meta = meta;

 return 0;
}

int roar_buffer_get_meta (struct roar_buffer * buf, void ** meta) {
 if ( buf == NULL )
  return -1;

 *meta = buf->meta;

 return 0;
}

int roar_buffer_set_len  (struct roar_buffer *  buf, size_t    len) {
 if ( buf == NULL )
  return -1;

 buf->user_len = len;

 return 0;
}

int roar_buffer_get_len  (struct roar_buffer *  buf, size_t *  len) {
 if ( buf == NULL )
  return -1;

 *len = buf->user_len;

 return 0;
}

int roar_buffer_set_flag (struct roar_buffer *  buf, int flag, int reset) {
 if ( buf == NULL )
  return -1;

 buf->flags |= flag;

 if ( reset )
  buf->flags -= flag;

 return 0;
}

int roar_buffer_get_flag (struct roar_buffer *  buf, int flag) {
 if ( buf == NULL )
  return -1;

 return buf->flags & flag;
}

int roar_buffer_duplicate (struct roar_buffer *  buf, struct roar_buffer ** copy) {
 struct roar_buffer *  cur = buf;
 struct roar_buffer *  new;
 void * od, * nd;

 *copy = NULL;

 while (cur) {
  if ( roar_buffer_new(&new, cur->user_len) == -1 ) {
   roar_buffer_free(*copy);
   return -1;
  }

  if ( *copy == NULL )
   *copy = new;

  roar_buffer_get_data(cur, &od);
  roar_buffer_get_data(new, &nd);
  memcpy(nd, od, cur->user_len);

  roar_buffer_add(*copy, new);

  cur = cur->next;
 }
 return 0;
}

int roar_buffer_ring_stats (struct roar_buffer *  buf, struct roar_buffer_stats * stats) {
 if ( buf == NULL )
  return -1;

 stats->parts        = 0;
 stats->bytes        = 0;
 stats->memory_usage = 0;

 while (buf) {
  stats->parts++;
  stats->bytes        += buf->user_len;
  stats->memory_usage += buf->len + sizeof(struct roar_buffer);
  buf = buf->next;
 }

 return 0;
}

//ll
