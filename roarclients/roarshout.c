//roarshout.c:

#include <roaraudio.h>

#ifdef ROAR_HAVE_LIBSHOUT
#include <shout/shout.h>

#define BUFSIZE 2048

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
 int    codec    = ROAR_CODEC_OGG_VORBIS;
 char * server   = NULL;
 char * k;
 char * s_server = "localhost";
 char * s_mount  = "/roar.ogg";
 char * s_pw     = "hackme";
 int    s_port   = 8000;
 int    fh;
 int    i;
 char buf[BUFSIZE];
 shout_t * shout;

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

 shout_init();

 if (!(shout = shout_new())) {
  ROAR_ERR("Can not clreate shout object");
  return 1;
 }

 if (shout_set_host(shout, s_server) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting hostname: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting protocol: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_port(shout, s_port) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting port: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_password(shout, s_pw) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting password: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_mount(shout, s_mount) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting mount: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_user(shout, "source") != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting user: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_format(shout, SHOUT_FORMAT_OGG) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting format: %s", shout_get_error(shout));
  return 1;
 }

 if ( (fh = roar_simple_monitor(rate, channels, bits, codec, server, "roarshout")) == -1 ) {
  fprintf(stderr, "Error: can not start monetoring\n");
  return 1;
 }

 if (shout_open(shout) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Can not open connection via libshout!");
  return -1;
 }

 while((i = read(fh, buf, BUFSIZE)))
  if (shout_send(shout, buf, i) != SHOUTERR_SUCCESS)
   break;

 roar_simple_close(fh);

 shout_sync(shout);

 shout_close(shout);

 shout_shutdown();

 return 0;
}

#else
int main (void) {
 fprintf(stderr, "No libshout support compiled in!\n");
 return 1;
}
#endif
//ll
