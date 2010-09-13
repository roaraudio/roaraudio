//client.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#ifndef _CLIENT_H_
#define _CLIENT_H_

/*
 Defined in <roaraudio/client.h>


#define ROAR_BUFFER_NAME 80

#define ROAR_CLIENTS_MAX 64
#define ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT 4

struct roar_client {
 int fh; /-* controll connection *-/
// int last_stream; /-* id of the last stream created *-/
 char name[ROAR_BUFFER_NAME];
 int pid;
 char host[ROAR_BUFFER_NAME];
 int execed;
 int streams[ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT];
} * g_clients[ROAR_CLIENTS_MAX];
*/

struct roar_client_nsubscribe {
 struct roar_subscriber * sub;
};

struct roar_client_server {
 struct roar_client _client;
 size_t blockc;
 struct roar_subscriber ** waits;
} * g_clients[ROAR_CLIENTS_MAX];


// basic functions
int clients_init       (void);
int clients_free       (void);
int clients_new        (void);
int clients_new_from_fh(int fh, int proto, int byteorder, int update_nnode);
int clients_delete     (int id);
int clients_close      (int id, int nocheck_exec);
int clients_set_fh     (int id, int    fh);
int clients_set_pid    (int id, int    pid);
int clients_set_uid    (int id, int    uid);
int clients_set_gid    (int id, int    gid);
int clients_set_name   (int id, char * name);
int clients_set_proto  (int id, int    proto);
int clients_get        (int id, struct roar_client ** client);
int clients_get_fh     (int id);

int clients_block      (int id, int unblock);

// network functions
int clients_check_all  (void);
int clients_check      (int id);
int clients_send_mon   (struct roar_audio_info * sa, uint32_t pos);
int clients_send_filter(struct roar_audio_info * sa, uint32_t pos);

// stream functions
int client_stream_exec   (int client, int stream);
int client_stream_set_fh (int client, int stream, int fh);
int client_stream_add    (int client, int stream);
int client_stream_delete (int client, int stream);
int client_stream_move   (int client, int stream);

// notify thingys
int  clients_wait    (int client, struct roar_event * events, size_t num);
void clients_ncb_wait(struct roar_notify_core * core, struct roar_event * event, void * userdata);

#endif

//ll
