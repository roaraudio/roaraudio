//codecfilter_mulaw.c:

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

int cf_mulaw_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct roar_stream * s = ROAR_STREAM(info);


 *inst = (CODECFILTER_USERDATA_T) info;

 s->info.bits  = 16;
 s->info.codec = ROAR_CODEC_DEFAULT;

 return 0;
}

int cf_mulaw_close(CODECFILTER_USERDATA_T   inst) {
 return 0;
}

int cf_mulaw_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct roar_stream_server * s = ROAR_STREAM_SERVER(inst);

 if ( (len = stream_vio_s_read(s, buf, len/2)) < 1 ) {
  return len;
 }

 roardsp_conv_mulaw2pcm16((int16_t *)buf, buf, len);

 return len*2;
}

int cf_mulaw_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct roar_stream_server * s = ROAR_STREAM_SERVER(inst);
 char * out;

 // TODO: add a more effect way to use memory than allways alloc/freeing it.
 //       maybe by keeping a buffer over instanzes or by using ca global buffer
 //       with an refrenze counter so we can free it on last use

 len /= 2;

 if ( (out = (char*)malloc(len)) == NULL )
  return -1;

 roardsp_conv_pcm162mulaw(out, (int16_t*)buf, len);

 len = stream_vio_s_write(s, out, len);

 free(out);

 if ( len > 0 ) {
  return len*2;
 } else {
  return -1;
 }
}

//ll
