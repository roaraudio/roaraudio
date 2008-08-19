//roarcat.c:

#include <roaraudio.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarmon [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
        "  --codec  CODEC     - Set the codec\n"
        "  --help             - Show this help\n"
       );

}

int main (int argc, char * argv[]) {
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 char * k;
 int    fh;
 int    i;
 char buf[BUFSIZE];

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--rate") == 0 ) {
   rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 ) {
   channels = atoi(argv[++i]);
  } else if ( strcmp(k, "--codec") == 0 ) {
   codec = roar_str2codec(argv[++i]);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( (fh = roar_simple_monitor(rate, channels, bits, codec, server, "roarmon")) == -1 ) {
  fprintf(stderr, "Error: can not start monetoring\n");
  return 1;
 }

 while((i = read(fh, buf, BUFSIZE)))
  if (write(1, buf, i) != i)
   break;

 roar_simple_close(fh);

 return 0;
}

//ll
