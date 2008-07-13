//meta.h:

#ifndef _META_H_
#define _META_H_

#include <roaraudio.h>

int stream_meta_set   (int id, int type, char * name, char * val);
int stream_meta_add   (int id, int type, char * name, char * val);
int stream_meta_get   (int id, int type, char * name, char * val, size_t len);
int stream_meta_list  (int id, int * types, size_t len);
int stream_meta_clear (int id);

#endif

//ll
