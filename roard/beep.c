//beep.c:

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

// TODO: FIXME: move them out of here into libroar:
static ssize_t beep_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 ROAR_DBG("beep_read(vio=%p, buf=%p, count=%llu) = ?", vio, buf, (long long unsigned int)count);

 if ( vio->inst == NULL )
  return 0;

 if ( roar_buffer_shift_out(&(vio->inst), buf, &count) == -1 )
  return -1;

 return count;
}

static int     beep_close   (struct roar_vio_calls * vio) {
 ROAR_DBG("beep_close(vio=%p) = ?", vio);

 if ( vio->inst != NULL )
  roar_buffer_free(vio->inst);

 ROAR_DBG("beep_close(vio=%p) = 0", vio);
 return 0;
}

static void beep_init_vio (struct roar_vio_calls * vio, void * inst) {
 ROAR_DBG("beep_init_vio(vio=%p, inst=%p) = ?", vio, inst);

 memset(vio, 0, sizeof(struct roar_vio_calls));

 vio->inst  = inst;
 vio->read  = beep_read;
 vio->close = beep_close;

 ROAR_DBG("beep_init_vio(vio=%p, inst=%p) = (void)", vio, inst);
}

struct roar_buffer * beep_fill_buffer (struct roar_beep * beep, struct roar_audio_info * info) {
 struct roar_buffer          * buf;
 size_t frames  = beep->time * info->rate     / 1000;
 size_t samples = frames     * info->channels;
 size_t mod     = info->rate / beep->freq;
 char                        * data;
 char   val;
 size_t pos;
 size_t chan;

 ROAR_DBG("beep_fill_buffer(beep=%p, info=%p) = ?", beep, info);

 if ( roar_buffer_new(&buf, samples) == -1 )
  return NULL;

 if ( roar_buffer_get_data(buf, &data) == -1 ) {
  roar_buffer_free(buf);
  ROAR_DBG("beep_fill_buffer(beep=%p, info=%p) = NULL", beep, info);
  return NULL;
 }

 for (pos = 0; pos < frames; pos++) {
  val = (pos % mod) < mod/2 ? -128 : 127;
  for (chan = 0; chan < info->channels; chan++)
   data[pos*info->channels + chan] = val;
 }

 ROAR_DBG("beep_fill_buffer(beep=%p, info=%p) = %p", beep, info, buf);
 return buf;
}

int beep_start (int client, struct roar_beep * beep) {
 struct roar_stream_server *  ss;
 struct roar_stream        *   s;
 struct roar_buffer        * buf;
 int stream;
 int i;

 ROAR_DBG("beep_start(client=%i, beep=%p) = ?", client, beep);

 if ( beep->vol  == 0 )
  beep->vol  = ROAR_BEEP_DEFAULT_VOL;

 if ( beep->time == 0 )
  beep->time = 512; // 512ms

 if ( beep->freq == 0 )
  beep->freq = 440;

 if ( beep->type == 0 )
  beep->type = ROAR_BEEP_TYPE_DEFAULT;

 // x, y, z location '0' is allready centered.

 // TODO: remove the following lions as soon as we support non zero values
 if ( beep->z != 0 )
  return -1;

 if ( beep->y != 0 )
  return -1;

 if ( beep->x != 0 )
  return -1;

 ROAR_DBG("beep_start(client=%i, beep=%p) = ?", client, beep);

 if ((stream = streams_new()) == -1 )
  return -1;

 ROAR_DBG("beep_start(client=%i, beep=%p): stream=%i", client, beep, stream);

 if ( client_stream_add(client, stream) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 if ( streams_set_name(stream, "Beep Source") == -1 ) {
  streams_delete(stream);
  return -1;
 }

 if ( streams_get(stream, &ss) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(s->info));

 s->info.channels = 1;
 s->info.bits     = 8;

 for (i = 0; i < s->info.channels; i++) {
  ss->mixer.mixer[i] = beep->vol;
  ss->mixer.scale    = ROAR_BEEP_MAX_VOL;
 }

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 if ( (buf = beep_fill_buffer(beep, &(s->info))) == NULL ) {
  streams_delete(stream);
  return -1;
 }

 ROAR_DBG("beep_start(client=%i, beep=%p): buf=%p", client, beep, buf);

 beep_init_vio(&(ss->vio), buf);

 if ( streams_set_fh(stream, -2) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 ROAR_DBG("beep_start(client=%i, beep=%p) = %i", client, beep, stream);
 return stream;
}

//ll
