//roarsocktypes.c:

#include <roaraudio.h>

int main (void) {
 int i, fh;
 struct {
  char * name;
  int (*func)(void);
 } tests[] = {
  {"TCPv4"           , roar_socket_new_tcp },
  {"UDPv4"           , roar_socket_new_udp },
  {"TCPv6"           , roar_socket_new_tcp6},
  {"UDPv6"           , roar_socket_new_udp6},
  {"UNIX"            , roar_socket_new_unix},
  {"DECnet seqpacket", roar_socket_new_decnet_seqpacket},
  {"DECnet stream"   , roar_socket_new_decnet_stream},
  {"IPX"             , roar_socket_new_ipx},
  {"IPX/SPX"         , roar_socket_new_ipxspx},
  {NULL, NULL}
 };

 for (i = 0; tests[i].func; i++) {
  printf("Type %-16s ", tests[i].name);
  errno = 0;
  fh = tests[i].func();
  if ( fh == -1 ) {
   printf("not working: %s\n", strerror(errno));
  } else {
   close(fh);
   printf("working\n");
  }
 }

 return 0;
}

//ll
