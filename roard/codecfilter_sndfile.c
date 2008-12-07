//codecfilter_sndfile.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

#ifdef ROAR_HAVE_LIBSNDFILE

int cf_sndfile_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_sndfile_inst * obj;

 if ( (obj = (struct codecfilter_sndfile_inst *) malloc(sizeof(struct codecfilter_sndfile_inst))) == NULL )
  return -1;

 memset(obj, 0, sizeof(struct codecfilter_sndfile_inst));

 obj->stream = info;

 *inst = (CODECFILTER_USERDATA_T) obj;

/*
 s->info.bits  = 16;
 s->info.codec = ROAR_CODEC_DEFAULT;
*/

 return 0;
}

int cf_sndfile_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_sndfile_inst * obj = (struct codecfilter_sndfile_inst *) inst;

 if ( obj->state != NULL )
  sf_close(obj->state);

 free(obj);

 return 0;
}

int cf_sndfile_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_sndfile_inst * obj = (struct codecfilter_sndfile_inst *) inst;
 struct roar_stream * s = ROAR_STREAM(obj->stream);
 int ret;

 if ( obj->opened ) {
  len /= obj->bytes;

  if ( obj->bytes == 2 ) {
   if ( (ret = sf_read_short(obj->state, (short*)buf, len)) == -1 )
    return -1;
  } else if ( obj->bytes == 4 ) {
   if ( (ret = sf_read_int(obj->state, (int*)buf, len)) == -1 )
    return -1;
  } else {
   errno = ENOSYS;
   return -1;
  }

  return ret * obj->bytes;
 } else {
  if ( (obj->state = sf_open_fd(s->fh, SFM_READ, &(obj->info), 0)) == NULL ) {
   ROAR_ERR("cf_sndfile_read(*): can not sf_open_fd(*)!");
   return -1;
  }

  s->info.codec    = ROAR_CODEC_DEFAULT;
  s->info.rate     = obj->info.samplerate;
  s->info.channels = obj->info.channels;

  obj->bytes       = 2;
  if ( (obj->info.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_24 ||
       (obj->info.format & SF_FORMAT_SUBMASK) == SF_FORMAT_PCM_32   ) {
   obj->bytes      = 4;
  }

  s->info.bits     = obj->bytes * 8;

  obj->opened      = 1;
  errno = EAGAIN;
 }

 return -1;
}

int cf_sndfile_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 return -1;
}

#endif

//ll
