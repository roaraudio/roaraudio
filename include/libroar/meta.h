//meta.h:

#ifndef _LIBROARMETA_H_
#define _LIBROARMETA_H_

#include "libroar.h"

int roar_stream_meta_set (struct roar_connection * con, struct roar_stream * s, int mode, struct roar_meta * meta);
int roar_stream_meta_get (struct roar_connection * con, struct roar_stream * s, struct roar_meta * meta);

int roar_stream_meta_list (struct roar_connection * con, struct roar_stream * s, int * types, size_t len);

int roar_meta_free (struct roar_meta * meta);

char * roar_meta_strtype(int type);
int    roar_meta_inttype(char * type);

#endif

//ll
