//libroarbasic.h:

#ifndef _LIBROARBASIC_H_
#define _LIBROARBASIC_H_

#include "libroar.h"

#define LIBROAR_BUFFER_SMALL   80
#define LIBROAR_BUFFER_MSGDATA LIBROAR_BUFFER_SMALL
#define _ROAR_MESSAGE_VERSION 0

#if ROAR_MAX_CHANNELS > (LIBROAR_BUFFER_SMALL - 10)
#error ROAR_MAX_CHANNELS too large change ROAR_MAX_CHANNELS or LIBROAR_BUFFER_SMALL
#endif

struct roar_message {
 int cmd;
 unsigned int stream;
 uint32_t pos;
 int datalen;
 char data[LIBROAR_BUFFER_MSGDATA];
};

struct roar_connection {
 int fh;
};


int roar_connect_raw (char * server);

int roar_connect    (struct roar_connection * con, char * server);
int roar_disconnect (struct roar_connection * con);

int roar_identify   (struct roar_connection * con, char * name);

int roar_send_message (struct roar_connection * con, struct roar_message * mes, char *  data);
int roar_recv_message (struct roar_connection * con, struct roar_message * mes, char ** data);
int roar_req          (struct roar_connection * con, struct roar_message * mes, char ** data);

int roar_debug_message_print (struct roar_message * mes);

int roar_debug_audio_info_print (struct roar_audio_info * info);

#endif

//ll
