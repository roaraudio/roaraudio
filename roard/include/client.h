//client.h:

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

struct roar_client * g_clients[ROAR_CLIENTS_MAX];


// basic functions

int clients_init      (void);
int clients_free      (void);
int clients_new       (void);
int clients_delete    (int id);
int clients_set_fh    (int id, int    fh);
int clients_set_pid   (int id, int    pid);
int clients_set_uid   (int id, int    uid);
int clients_set_gid   (int id, int    gid);
int clients_set_name  (int id, char * name);
int clients_get       (int id, struct roar_client ** client);

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

#endif

//ll
