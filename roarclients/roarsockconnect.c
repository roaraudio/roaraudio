//roarsockconnect.c:

#include <roaraudio.h>
#include <string.h>
#include <stdio.h>

void usage (void) {
 fprintf(stderr, "Usage: roarsockconnect HOST PORT\n");

 exit(1);
}

int main (int argc, char * argv[]) {
 char buf[1024];
 int fh;
 int port;
 int len;

 if ( argc != 3 )
  usage();

 port     = atoi(argv[2]);

 if ( (fh = roar_socket_connect(argv[1], port)) == -1 ) {
  ROAR_ERR("Error: can not connect!");
  return 1;
 }

 while ((len = read(ROAR_STDIN, buf, 1024)))
  write(fh, buf, len);

 while ((len = read(fh, buf, 1024)))
  write(ROAR_STDOUT, buf, len);

 return 0;
}

//ll
