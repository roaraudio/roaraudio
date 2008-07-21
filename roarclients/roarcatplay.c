//roarcat.c:

#include <roaraudio.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarcatplay [OPTIONS]... [FILE]\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --help             - Show this help\n"
       );

}

int main (int argc, char * argv[]) {
 char * server   = NULL;
 char * k;
 int    i;
 char * file = NULL;
 struct roar_connection con;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else if ( file == NULL ) {
   file = argv[i];
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }


 if ( file == NULL )
  file = "/dev/stdin";

 if ( roar_simple_play_file(file, server, "roarcatplay") == -1 )
  return 1;

 return 0;
}

//ll
