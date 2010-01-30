//emul_simple.c:

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

int emul_simple_on_connect  (int client, struct roard_listen * lsock) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 struct roar_client        *  c;
 int stream;

 if ( clients_get(client, &c) == -1 ) {
  return -1;
 }

 if ((stream = streams_new()) == -1 ) {
  clients_delete(client);
  return -1;
 }

 if ( streams_get(stream, &ss) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 s = ROAR_STREAM(ss);

 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  clients_delete(client);
  return -1;
 }

 memcpy(&(s->info), &(lsock->inst.stpl.info), sizeof(struct roar_audio_info));

 ss->codec_orgi = s->info.codec;

 if ( streams_set_dir(stream, lsock->inst.stpl.dir, 1) == -1 ) {
  clients_delete(client);
  return -1;
 }

 if ( client_stream_exec(client, stream) == -1 ) {
  clients_delete(client);
  return -1;
 }

 return 0;
}

//ll
