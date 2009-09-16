//rdtcs.c:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

#ifndef ROAR_WITHOUT_DCOMP_RDTCS

int rdtcs_init  (void) {
 return 0;
}

int rdtcs_free  (void) {
 return 0;
}

int rdtcs_init_config  (void) {
 memset(&g_rdtcs, 0, sizeof(g_rdtcs));

 strncpy(g_rdtcs.rds.ps, RDTCS_RDS_PS_DEFAULT, RDTCS_RDS_PS_LEN);
 g_rdtcs.rds.ps[RDTCS_RDS_PS_LEN] = 0;

 g_rdtcs.rds.pty   = RDTCS_RDS_PTY_DEFAULT;
 g_rdtcs.rds.pi    = RDTCS_RDS_PI_DEFAULT;
 g_rdtcs.rds.flags = RDTCS_RDS_FLAG_NONE;

 return 0;
}

int rdtcs_rds_set_ps  (char * ps) {
 int i;

 if ( ps == NULL )
  return -1;

 if ( strlen(ps) > 8 )
  return -1;

 // coppy string converting to upper case:
 for (i = 0; ps[i]; i++) {
  g_rdtcs.rds.ps[i] = toupper(ps[i]);
 }

 g_rdtcs.rds.ps[i] = 0; // terminating \0

 return 0;
}

int rdtcs_rds_set_pty (char * pty) {
 if ( pty == NULL )
  return -1;

 return -1;
}

int rdtcs_rds_set_flag  (unsigned int flag, int reset) {
 
 g_rdtcs.rds.flags |= flag;

 if ( reset )
  g_rdtcs.rds.flags -= flag;

 return 0;
}


int rdtcs_check_stream  (int id) {
 return -1;
}

int rdtcs_send_stream   (int id) {
 struct roar_stream        *   s;
 struct roar_stream_server *  ss;

 if ( g_streams[id] == NULL )
  return -1;

 ROAR_DBG("rdtcs_send_stream(id=%i) = ?", id);

 s = ROAR_STREAM(ss = g_streams[id]);

 switch (s->info.codec) {
  case ROAR_CODEC_RDS:
    return rdtcs_send_stream_rds(id, ss);
   break;
  default:
    streams_delete(id);
    return -1;
 }

 return 0;
}

int rdtcs_send_stream_rds  (int id, struct roar_stream_server *  ss) {
 struct roar_stream        *   s;

 s = ROAR_STREAM(ss);


 return rdtcs_send_stream_rds_group(id, ss);
}

int rdtcs_send_stream_rds_group  (int id, struct roar_stream_server *  ss) {
 char out[RDTCS_RDS_GROUP_LEN];
 char * c;
 int      block[4] = {RDTCS_RDS_BLOCK_A, RDTCS_RDS_BLOCK_B, RDTCS_RDS_BLOCK_C0, RDTCS_RDS_BLOCK_D};
 uint16_t data[4];
 uint16_t crc;
 register uint32_t s;
 int i, fill;

 // TODO: think about byte order!

 for (i = 0; i < 4; i++) data[i] = 0;

 data[0] = g_rdtcs.rds.pi;

 memset(out, 0, sizeof(out));

 c    = out;
 s    = 0;
 fill = 0;
 for (i = 0; i < 4; i++) {
  // data is 16 bit long
  s |= data[i] << (fill & 0xFFFF);
  fill += 16;

  crc = rdtcs_rds_crc_calc(data[i], block[i]);

  // CRC is 10 bit long
  s |= crc << (fill & 0x03FF);
  fill += 10;

  // shift all complet bytes we allready have out
  while (fill >= 8) {
   *c     = s & 0xFF;
    c++;
    s   >>= 8;
    fill -= 8;
  }
 }

 return stream_vio_s_write(ss, out, RDTCS_RDS_GROUP_LEN) == RDTCS_RDS_GROUP_LEN ? 0 : -1;
}

uint16_t rdtcs_rds_crc_calc      (uint16_t data, int block) {
 return 0xAAAA;
}

#endif

//ll
