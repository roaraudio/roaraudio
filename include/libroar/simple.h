//libroar.h:

#ifndef _LIBROARSIMPLE_H_
#define _LIBROARSIMPLE_H_

#include "libroar.h"

int roar_simple_connect (struct roar_connection * con, char * server, char * name);

int roar_simple_stream  (int rate, int channels, int bits, int codec, char * server, int dir, char * name);

int roar_simple_play    (int rate, int channels, int bits, int codec, char * server, char * name);
int roar_simple_monitor (int rate, int channels, int bits, int codec, char * server, char * name);
int roar_simple_record  (int rate, int channels, int bits, int codec, char * server, char * name);
int roar_simple_filter  (int rate, int channels, int bits, int codec, char * server, char * name);

int roar_simple_close(int fh);

int roar_simple_get_standby (int fh);

#endif

//ll
