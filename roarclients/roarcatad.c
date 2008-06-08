//roarcatad.c:

#include <roaraudio.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarcat [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
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
 int    i;
 char buf[BUFSIZE];
 struct roar_connection con;
 struct roar_stream     s;


 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = k;
  } else if ( strcmp(k, "--rate") == 0 ) {
   rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 ) {
   channels = atoi(argv[++i]);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( roar_simple_connect(&con, server, "roarcatad") == -1 ) {
  ROAR_DBG("roar_simple_play(*): roar_simple_connect() faild!");
  return -1;
 }

 if ( roar_stream_new(&s, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 if ( roar_stream_connect(&con, &s, ROAR_DIR_PLAY) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 while((i = read(0, buf, BUFSIZE)))
  if (roar_stream_add_data(&con, &s, buf, i) == -1)
   break;

 roar_disconnect(&con);

 return 0;
}

//ll
