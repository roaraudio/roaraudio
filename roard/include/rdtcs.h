//rdtcs.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

#ifndef _RDTCS_H_
#define _RDTCS_H_

#include <roaraudio.h>

#ifndef ROAR_WITHOUT_DCOMP_RDTCS

#define RDTCS_RDS_PS_LEN      8
#define RDTCS_RDS_PS_DEFAULT  "ROARING"
#define RDTCS_RDS_PTY_DEFAULT 0
#define RDTCS_RDS_PI_DEFAULT  0x0000

#define RDTCS_RDS_BLOCK_A     1
#define RDTCS_RDS_BLOCK_B     2
#define RDTCS_RDS_BLOCK_C0    3
#define RDTCS_RDS_BLOCK_C1    4
#define RDTCS_RDS_BLOCK_D     5

#define RDTCS_RDS_GROUP_LEN   ((4*(16+10))/8)

#define RDTCS_RDS_FLAG_NONE   0x0000
#define RDTCS_RDS_FLAG_TP     0x0001
#define RDTCS_RDS_FLAG_CT     0x0002

struct {
 int inited;
 struct {
  char          ps[RDTCS_RDS_PS_LEN+1];
  unsigned char pty;
  uint16_t      pi;
  unsigned int  flags;
 } rds;
} g_rdtcs;

int rdtcs_init  (void);
int rdtcs_free  (void);

int rdtcs_init_config  (void);

int rdtcs_rds_set_ps  (char * ps);
int rdtcs_rds_set_pty (char * pty);
int rdtcs_rds_set_flag(unsigned int flag, int reset);

int rdtcs_check_stream  (int id);
int rdtcs_send_stream   (int id);

int rdtcs_send_stream_rds        (int id, struct roar_stream_server *  ss);
int rdtcs_send_stream_rds_group  (int id, struct roar_stream_server *  ss);

uint16_t rdtcs_rds_crc_calc      (uint16_t data, int block);

#endif

#endif

//ll
