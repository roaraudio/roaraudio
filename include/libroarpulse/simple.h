//simple.h:

#ifndef _LIBROARPULSE_SIMPLE_H_
#define _LIBROARPULSE_SIMPLE_H_

#include <libroarpulse/libroarpulse.h>

struct roarpulse_simple {
 struct roar_connection con;
 struct roar_stream     stream;
 int data_fh;
};

#endif

//ll
