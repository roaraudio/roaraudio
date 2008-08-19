//vio.c:

#include "libroar.h"

int roar_vio_init_calls (struct roar_vio_calls * calls) {
 if ( !calls )
  return -1;

 memset((void*)calls, 0, sizeof(struct roar_vio_calls));

 calls->read  = (ssize_t (*)(int fd, void *buf, size_t count,      void * inst))read;
 calls->write = (ssize_t (*)(int fd, void *buf, size_t count,      void * inst))write;
 calls->lseek = (off_t   (*)(int fildes, off_t offset, int whence, void * inst))lseek;

 return 0;
}

//ll
