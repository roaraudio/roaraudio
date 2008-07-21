//file.h:

#ifndef _LIBROARFILE_H_
#define _LIBROARFILE_H_

#include "libroar.h"

#ifdef ROAR_HAVE_LINUX_SENDFILE
#include <sys/sendfile.h>
#endif

ssize_t roar_file_send_raw (int out, int in);

#endif

//ll
