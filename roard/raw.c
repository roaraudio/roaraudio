//raw.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifndef ROAR_WITHOUT_DCOMP_RAW

int raw_check_stream  (int id) {
 struct roar_stream        *    s;
 struct roar_stream_server *   ss;
 struct roar_buffer        *  buf;
 void                      * data;
 ssize_t                     len;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("raw_check_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 if ( s->pos_rel_id == -1 )
  return -1;

 if ( roar_buffer_new(&buf, RAW_READ_LEN) == -1 )
  return -1;

 if ( roar_buffer_get_data(buf, &data) == -1 ) {
  roar_buffer_free(buf);
  return -1;
 }

 if ( (len = stream_vio_s_read(ss, data, RAW_READ_LEN)) < 1 ) {
  // this is len=0 -> eof OR len=-1 -> error
  streams_delete(id);
  roar_buffer_free(buf);
  return -1;
 }

 if ( stream_vio_write(s->pos_rel_id, data, len) != len ) {
  // we are that hard here as this is needed to ensure data integrety
  streams_delete(s->pos_rel_id);
 }

 roar_buffer_free(buf);

 return 0;
}

#endif

//ll
