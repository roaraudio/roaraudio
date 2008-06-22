//roartypes.c:

#include <roaraudio.h>
#include "../roard/include/roard.h"

int main (void) {
 struct { char * name; int len; } types[] = {
  { "roar_buffer",        sizeof(struct roar_buffer)        },
  { "roar_stream",        sizeof(struct roar_stream)        },
  { "roar_stream_server", sizeof(struct roar_stream_server) },
  { "roar_sample",        sizeof(struct roar_sample)        },
  { "roar_client",        sizeof(struct roar_client)        },
  { "roar_connection",    sizeof(struct roar_connection)    },
  { "roar_audio_info",    sizeof(struct roar_audio_info)    },
  { NULL, 0 }
 }, * c = types - 1;

 while ((++c)->name != NULL)
  printf("%-20s = %3i Bytes = %4i Bits\n", c->name, c->len, c->len * 8);

 return 0;
}

//ll
