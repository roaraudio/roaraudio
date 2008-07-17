//roar-config.c:

#include <roaraudio.h>

int main (int argc, char * argv[]) {
 int i;

 if ( argc == 1 ) {
  printf("Usage: roar-config [--version] [--libs] [--cflags]\n");
  return 0;
 }

 for (i = 1; i < argc; i++) {
  if ( !strcmp(argv[i], "--version") ) {
  } else if ( !strcmp(argv[i], "--libs") ) {
   printf("%s\n", ROAR_LIBS);
  } else if ( !strcmp(argv[i], "--cflags") ) {
   printf("%s\n", ROAR_CFLAGS);
  } else {
   fprintf(stderr, "Unknown option: %s\n", argv[i]);
   return 1;
  }
 }

 return 0;
}

//ll
