//buffer.h:

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

#ifndef _LIBROAR_BUFFER_H_
#define _LIBROAR_BUFFER_H_

#include <roaraudio.h>

struct roar_buffer {
 size_t               len;
 size_t               user_len;
 void               * data;
 void               * user_data;
 void               * meta;
 struct roar_buffer * next;
};

struct roar_buffer_stats {
 int parts;
 int bytes;
 int memory_usage;
};

#define roar_buffer_next(a) roar_buffer_delete(*(a), (a))

int roar_buffer_new      (struct roar_buffer ** buf, size_t len);
int roar_buffer_free     (struct roar_buffer *  buf);
int roar_buffer_delete   (struct roar_buffer *  buf, struct roar_buffer ** next);
int roar_buffer_add      (struct roar_buffer *  buf, struct roar_buffer *  next);

int roar_buffer_get_next (struct roar_buffer *  buf, struct roar_buffer ** next);

int roar_buffer_get_data (struct roar_buffer *  buf, void   ** data);

int roar_buffer_set_offset (struct roar_buffer *  buf, size_t off);

int roar_buffer_shift_out (struct roar_buffer ** buf, void * data, size_t * len);

int roar_buffer_set_meta (struct roar_buffer *  buf, void   *  meta);
int roar_buffer_get_meta (struct roar_buffer *  buf, void   ** meta);

int roar_buffer_set_len  (struct roar_buffer *  buf, size_t    len);
int roar_buffer_get_len  (struct roar_buffer *  buf, size_t *  len);

int roar_buffer_duplicate (struct roar_buffer *  buf, struct roar_buffer ** copy);

int roar_buffer_ring_stats (struct roar_buffer *  buf, struct roar_buffer_stats * stats);

#endif

//ll
