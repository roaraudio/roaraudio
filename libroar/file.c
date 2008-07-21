//file.c:

#include "libroar.h"

#define BUFSIZE 8192
#define BUFMAX  65536

ssize_t roar_file_send_raw (int out, int in) {
 ssize_t r = 0;
 ssize_t ret;
 int len;
 char buf[BUFSIZE];

#ifdef ROAR_HAVE_LINUX_SENDFILE
 while ((ret = sendfile(out, in, NULL, BUFMAX)) > 0)
  r += ret;
#endif

 // TODO: try mmap here!

 while ((len = read(in, buf, BUFSIZE)) > 0)
  r += write(out, buf, len);

 return r;
}

//ll
