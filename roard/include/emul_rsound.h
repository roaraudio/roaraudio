//emul_rsound.h:

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

#ifndef _EMUL_RSOUND_H_
#define _EMUL_RSOUND_H_

#include <roaraudio.h>

#ifndef ROAR_WITHOUT_DCOMP_EMUL_RSOUND

#define EMUL_RSOUND_MSG_HEADER_LEN (3+5)
#define EMUL_RSOUND_MSG_DATA_LEN   (64)

struct emul_rsound_msg {
 char header[EMUL_RSOUND_MSG_HEADER_LEN+1];
 size_t datalen;
 char data[EMUL_RSOUND_MSG_DATA_LEN+1];
 char * datasp;
 size_t dataslen;
};

int emul_rsound_vrecv_msg(struct emul_rsound_msg * msg, struct roar_vio_calls * vio);
int emul_rsound_vsend_msg(struct emul_rsound_msg * msg, struct roar_vio_calls * vio);

int emul_rsound_on_connect  (int fh, struct roard_listen * lsock);
int emul_rsound_check_client(int client, struct roar_vio_calls * vio);

#endif

#endif

//ll
