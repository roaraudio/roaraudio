//file.h:

#ifndef _LIBROARFILE_H_
#define _LIBROARFILE_H_

#include "libroar.h"

#ifdef ROAR_HAVE_LINUX_SENDFILE
#include <sys/sendfile.h>
#endif

int     roar_file_codecdetect(char * buf, int len);
ssize_t roar_file_send_raw   (int out, int in);
ssize_t roar_file_play       (struct roar_connection * con, char * file, int exec);

ssize_t roar_file_map        (char * filename, int flags, mode_t mode, size_t len, void ** mem);
int     roar_file_unmap      (size_t len, void * mem);
#endif

//ll
