//codecfilter_celt.c:

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
#ifdef ROAR_HAVE_LIBCELT

int cf_celt_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter) {
 struct codecfilter_celt_inst * self = malloc(sizeof(struct codecfilter_celt_inst));
 struct roar_stream * s = ROAR_STREAM(info);

 if ( !self )
  return -1;

/*
 CELTMode * mode;
 CELTEncoder * encoder;
 CELTDecoder * decoder;
 int frame_size;
 int lookahead;
 int out_size;
 char * ibuf;
 char * obuf;
 char * rest;
 int s_buf;
 int f_rest; /-* how much is in rest? *-/
*/

 self->stream               = info;
 self->frame_size           = 256;
 self->lookahead            = self->frame_size;
 self->encoder              = NULL;
 self->decoder              = NULL;
 self->opened               = 0;
 self->s_buf                = s->info.channels * self->frame_size * 2;
 self->ibuf                 = malloc(self->s_buf);
 self->obuf                 = malloc(self->s_buf);
 self->i_rest               = malloc(self->s_buf);
 self->o_rest               = malloc(self->s_buf);
 self->fi_rest              = 0;
 self->fo_rest              = 0;

 if ( !(self->ibuf && self->obuf && self->i_rest && self->o_rest) ) {
  if ( self->ibuf )
   free(self->ibuf);

  if ( self->obuf )
   free(self->obuf);

  if ( self->i_rest )
   free(self->o_rest);

  if ( self->o_rest )
   free(self->o_rest);

  free(self);
  return -1;
 }
 
 self->mode                 = celt_mode_create(s->info.rate, s->info.channels, self->frame_size, NULL);

 if ( !self->mode ) {
  free(self);
  return -1;
 }

 if ( s->dir == ROAR_DIR_PLAY ) {
   self->decoder = celt_decoder_create(self->mode);
 } else if ( s->dir == ROAR_DIR_MONITOR || s->dir == ROAR_DIR_OUTPUT ) {
   self->encoder = celt_encoder_create(self->mode);
 } else {
  celt_mode_destroy(self->mode);
  free(self);
  return -1;
 }

 *inst = (CODECFILTER_USERDATA_T) self;

 s->info.codec = ROAR_CODEC_DEFAULT;
 s->info.bits  = 16; // CELT hardcoded

 return 0;
}

int cf_celt_close(CODECFILTER_USERDATA_T   inst) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;

 if ( !inst )
  return -1;

 if ( self->encoder )
 celt_encoder_destroy(self->encoder);

 if ( self->decoder )
 celt_decoder_destroy(self->decoder);

 if ( self->mode )
  celt_mode_destroy(self->mode);

 if ( self->ibuf )
  free(self->ibuf);

 if ( self->obuf )
  free(self->obuf);

 if ( self->i_rest )
  free(self->i_rest);

 if ( self->o_rest )
  free(self->o_rest);

 free(inst);
 return 0;
}

int cf_celt_read(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;
 int r = 0;
 uint16_t fs;
 char * cbuf;
 char magic[ROAR_CELT_MAGIC_LEN];

// printf("buf=%p, len=%i\n", buf, len);

 if ( !self->opened ) {
  errno = ENOSYS;
  if ( stream_vio_s_read(self->stream, magic, ROAR_CELT_MAGIC_LEN) != ROAR_CELT_MAGIC_LEN )
   return -1;
  if ( memcmp(magic, ROAR_CELT_MAGIC, ROAR_CELT_MAGIC_LEN) != 0 )
   return -1;

  errno = 0;
  self->opened = 1;
 }

 if ( self->fi_rest ) {
  memcpy(buf, self->i_rest, self->fi_rest);
  r += self->fi_rest;
  self->fi_rest = 0;
 }

 while ( r <= (len - self->s_buf) ) {
  if ( stream_vio_s_read(self->stream, &fs, 2) != 2 )
   break;

  fs = ROAR_NET2HOST16(fs);

  if ( fs > self->s_buf )
   return -1;

  if ( stream_vio_s_read(self->stream, self->ibuf, fs) != fs )
   return -1;

  cbuf = buf + r;

//  printf("buf=%p, r=%i // cbuf=%p\n", buf, r, cbuf);
  if ( celt_decode(self->decoder, (unsigned char *) self->ibuf, fs, (celt_int16_t *) cbuf) < 0 )
   return -1;

  r += self->s_buf;
 }

 if ( r < len ) {
//  printf("r < len!\n");
  if ( stream_vio_s_read(self->stream, &fs, 2) == 2 ) {
   fs = ROAR_NET2HOST16(fs);
//   printf("next: fs=%i\n", fs);
   if ( fs > self->s_buf )
    return -1;
   if ( stream_vio_s_read(self->stream, self->ibuf, fs) == fs ) {
//    printf("got data!\n");
    if ( celt_decode(self->decoder, (unsigned char *) self->ibuf, fs, (celt_int16_t *) self->obuf) >= 0 ) {
//     printf("{ // decode rest\n");
//     printf(" r=%i // need %i Bytes\n", r, len - r);
//     printf(" memcpy(buf+%i, self->obuf, %i) = ?\n", r, len - r);
     memcpy(buf+r, self->obuf, len - r);
     self->fi_rest = self->s_buf + r - len;
     memcpy(self->i_rest, self->obuf + len - r, self->fi_rest);
//     printf(" len=%i, r=%i, fi_rest=%i, s_buf=%i\n", len, r, self->fi_rest, self->s_buf);
     r = len;
//     printf("}\n");
    }
   }
  }
 }

 ROAR_DBG("cf_celt_read(inst=%p, buf=%p, len=%i) = %i", inst, buf, len, r);
 return r;
}

#define BS (ROAR_STREAM(self->stream)->info.channels * 64)
int cf_celt_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 struct codecfilter_celt_inst * self = (struct codecfilter_celt_inst *) inst;
 int have = 0;
 int org_len = len;
 int diff;
 int fs2 = self->frame_size * 2 * ROAR_STREAM(self->stream)->info.channels;
 uint16_t pkglen_net, pkglen;
 unsigned char cbits[BS+2];

 if ( !self->opened ) {
  if ( stream_vio_s_write(self->stream, ROAR_CELT_MAGIC, ROAR_CELT_MAGIC_LEN) != ROAR_CELT_MAGIC_LEN )
   return -1;
  self->opened = 1;
 }

 if ( (self->fo_rest + len) > fs2 ) {
  if ( self->fo_rest ) {
   memcpy(self->obuf, self->o_rest, self->fo_rest);
   have = self->fo_rest;
   self->fo_rest = 0;
  }

  memcpy(self->obuf+have, buf, (diff=fs2-have));
  buf += diff;
  len -= diff;

  pkglen     = celt_encode(self->encoder, (celt_int16_t *) self->obuf, NULL, cbits+2, BS);
  pkglen_net = ROAR_HOST2NET16(pkglen);
  *(uint16_t*)cbits = pkglen_net;

  if ( stream_vio_s_write(self->stream, cbits, pkglen+2) == -1 )
   return -1;

  while (len >= fs2) {
   pkglen     = celt_encode(self->encoder, (celt_int16_t *) buf, NULL, cbits+2, BS);
   pkglen_net = ROAR_HOST2NET16(pkglen);
   *(uint16_t*)cbits = pkglen_net;

   if ( stream_vio_s_write(self->stream, cbits, pkglen+2) == -1 )
    return -1;
   len -= fs2;
   buf += fs2;
  }
 }

 if ( len ) {
  memcpy(self->o_rest + self->fo_rest, buf, len);
  self->fo_rest += len;
  len            = 0;
 } 

 return org_len;
}

#endif
//ll
