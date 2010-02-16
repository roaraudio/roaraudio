//req.h:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _REQ_H_
#define _REQ_H_

#include <roaraudio.h>

int req_on_noop        (int client, struct roar_message * mes, char * data);

int req_on_identify    (int client, struct roar_message * mes, char * data);
int req_on_auth        (int client, struct roar_message * mes, char * data);

int req_on_whoami      (int client, struct roar_message * mes, char * data);

int req_on_new_stream  (int client, struct roar_message * mes, char * data);
int req_on_exec_stream (int client, struct roar_message * mes, char * data);
int req_on_con_stream  (int client, struct roar_message * mes, char * data);
int req_on_passfh      (int client, struct roar_message * mes, char * data);

#ifdef ROAR_SUPPORT_META
int req_on_set_meta    (int client, struct roar_message * mes, char * data);
int req_on_get_meta    (int client, struct roar_message * mes, char * data);
int req_on_list_meta   (int client, struct roar_message * mes, char * data);
#endif

int req_on_get_standby (int client, struct roar_message * mes, char * data);
int req_on_set_standby (int client, struct roar_message * mes, char * data);

int req_on_exit        (int client, struct roar_message * mes, char * data);

int req_on_server_oinfo(int client, struct roar_message * mes, char * data);

int req_on_list_clients(int client, struct roar_message * mes, char * data);
int req_on_list_streams(int client, struct roar_message * mes, char * data);

int req_on_get_client  (int client, struct roar_message * mes, char * data);
int req_on_get_stream  (int client, struct roar_message * mes, char * data);

int req_on_get_stream_para (int client, struct roar_message * mes, char * data);
int req_on_set_stream_para (int client, struct roar_message * mes, char * data);

int req_on_kick        (int client, struct roar_message * mes, char * data);
int req_on_attach      (int client, struct roar_message * mes, char * data);

int req_on_set_vol     (int client, struct roar_message * mes, char * data);
int req_on_get_vol     (int client, struct roar_message * mes, char * data);

int req_on_add_data    (int client, struct roar_message * mes, char * data);


//int req_on_ (int client, struct roar_message * mes, char * data);

#endif

//ll
