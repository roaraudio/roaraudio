//roarfish.c:

#include "roarfish.h"

void usage (void) {
 printf("Usage: roarfish COMMAND FILE [OPTIONS]\n");
 printf("\nCOMMANDS:\n");
 printf("play          - plays a file\n"
       );
}

int main (int argc, char * argv[]) {
// int i;
 char * command, * file;

 if ( argc < 3 ) {
  usage();
  return 1;
 }

 command = argv[1];
 file    = argv[2];

 if ( !strcmp(command, "play") ) {
  play(file, NULL);
 } else {
  usage();
  return 1;
 }

 return 0;
}

//ll
