//auth.h:

#ifndef _LIBROARVIO_H_
#define _LIBROARVIO_H_

#include "libroar.h"

struct roar_vio_calls {
 ssize_t (*read) (int fd, void *buf, size_t count);
 ssize_t (*write)(int fd, void *buf, size_t count);
 off_t   (*lseek)(int fildes, off_t offset, int whence);
};

int roar_vio_init_calls (struct roar_vio_calls * calls);

#endif

//ll
