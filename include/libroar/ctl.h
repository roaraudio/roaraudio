//auth.h:

#ifndef _LIBROARCTL_H_
#define _LIBROARCTL_H_

#include "libroar.h"

#define ROAR_CTL_FILTER_ANY   0


int roar_get_standby   (struct roar_connection * con);
int roar_set_standby   (struct roar_connection * con, int state);

int roar_exit   (struct roar_connection * con);
int roar_server_oinfo   (struct roar_connection * con, struct roar_stream * s);


int roar_list         (struct roar_connection * con, int * items,   int max, int cmd);

/*
int roar_list_clients (struct roar_connection * con, int * clients, int max);
int roar_list_streams (struct roar_connection * con, int * streams, int max);
*/
#define roar_list_clients(c,i,m) roar_list((c),(i),(m),ROAR_CMD_LIST_CLIENTS)
#define roar_list_streams(c,i,m) roar_list((c),(i),(m),ROAR_CMD_LIST_STREAMS)

int roar_get_client   (struct roar_connection * con, struct roar_client * client, int id);
int roar_get_stream   (struct roar_connection * con, struct roar_stream * stream, int id);

int roar_kick         (struct roar_connection * con, int type, int id);

int roar_set_vol      (struct roar_connection * con, int id, struct roar_mixer_settings * mixer, int channels);

// filter...
int roar_ctl_f2m      (struct roar_message * m, unsigned char   filter, unsigned char   cmp, uint32_t   id);
int roar_ctl_m2f      (struct roar_message * m, unsigned char * filter, unsigned char * cmp, uint32_t * id);
#define roar_ctl_f2m_any(m) roar_ctl_f2m((m), ROAR_CTL_FILTER_ANY, ROAR_CTL_FILTER_ANY, ROAR_CTL_FILTER_ANY)

// int array
int roar_ctl_ia2m     (struct roar_message * m, int * data, int len);
int roar_ctl_m2ia     (struct roar_message * m, int * data, int len);

// client
int roar_ctl_c2m      (struct roar_message * m, struct roar_client * c);
int roar_ctl_m2c      (struct roar_message * m, struct roar_client * c);

#endif

//ll
