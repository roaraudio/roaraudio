//req.h:

#ifndef _REQ_H_
#define _REQ_H_

#include <roaraudio.h>

int req_on_noop        (int client, struct roar_message * mes, char * data);

int req_on_identify    (int client, struct roar_message * mes, char * data);
int req_on_auth        (int client, struct roar_message * mes, char * data);

int req_on_new_stream  (int client, struct roar_message * mes, char * data);
int req_on_exec_stream (int client, struct roar_message * mes, char * data);
int req_on_con_stream  (int client, struct roar_message * mes, char * data);

int req_on_set_meta    (int client, struct roar_message * mes, char * data);

int req_on_get_standby (int client, struct roar_message * mes, char * data);
int req_on_set_standby (int client, struct roar_message * mes, char * data);

int req_on_exit        (int client, struct roar_message * mes, char * data);

int req_on_server_oinfo(int client, struct roar_message * mes, char * data);

int req_on_list_clients(int client, struct roar_message * mes, char * data);
int req_on_list_streams(int client, struct roar_message * mes, char * data);

int req_on_get_client  (int client, struct roar_message * mes, char * data);
int req_on_get_stream  (int client, struct roar_message * mes, char * data);

int req_on_kick        (int client, struct roar_message * mes, char * data);

int req_on_set_vol     (int client, struct roar_message * mes, char * data);
int req_on_get_vol     (int client, struct roar_message * mes, char * data);

int req_on_add_data    (int client, struct roar_message * mes, char * data);


//int req_on_ (int client, struct roar_message * mes, char * data);

#endif

//ll
