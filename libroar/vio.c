//vio.c:

#include "libroar.h"

int roar_vio_init_calls (struct roar_vio_calls * calls) {
 if ( !calls )
  return -1;

 memset((void*)calls, 0, sizeof(struct roar_vio_calls));

 calls->read  = read;
 calls->write = write;
 calls->lseek = lseek;

 return 0;
}

//ll
