//stream.h:

#ifndef _ROARAUDIO_CLIENT_H_
#define _ROARAUDIO_CLIENT_H_

#define ROAR_BUFFER_NAME 80

#define ROAR_CLIENTS_MAX 64
#define ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT 4

struct roar_client {
 int fh; /* controll connection */
// int last_stream; /* id of the last stream created */
 char name[ROAR_BUFFER_NAME];
 int pid;
 char host[ROAR_BUFFER_NAME];
 int execed;
 int streams[ROAR_CLIENTS_MAX_STREAMS_PER_CLIENT];
 struct roar_acl_target * acl;
};

#endif

//ll
