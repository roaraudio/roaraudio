//auth.h:

#ifndef _LIBROARVIO_H_
#define _LIBROARVIO_H_

#include "libroar.h"

struct roar_vio_calls {
 void * inst;
 ssize_t (*read) (int fd, void *buf, size_t count, inst);
 ssize_t (*write)(int fd, void *buf, size_t count, inst);
 off_t   (*lseek)(int fildes, off_t offset, int whence, inst);
};

int roar_vio_init_calls (struct roar_vio_calls * calls);

#endif

//ll
