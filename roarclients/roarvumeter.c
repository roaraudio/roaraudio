//roarcat.c:

#include <roaraudio.h>
#include <math.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarvumeter [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server  SERVER   - Set server hostname\n"
        "  --rate    RATE     - Set sample rate\n"
        "  --bits    BITS     - Set bits per sample\n"
        "  --chans   CHANNELS - Set number of channels\n"
        "  --samples SAMPLES  - Set number of samples\n"
        "  --help             - Show this help\n"
       );

}

int vumeter16bit2ch (int fh, int samples, int16_t * buf) {
 int i;
 int samples_half = samples/2;
 double suml, sumr;

 printf("\e[s");
 fflush(stdout);

 while (read(fh, buf, samples * 2)) {
  suml = sumr = 0;

  for (i = 0; i < samples; i += 2) {
   suml += (double) buf[i  ] * (double) buf[i  ];
   sumr += (double) buf[i+1] * (double) buf[i+1];
  }

  suml = sqrt(suml/samples_half)/327.68;
  sumr = sqrt(sumr/samples_half)/327.68;

  printf("\e[uL: %3i%% R: %3i%%          ", (int)suml, (int)sumr);
  fflush(stdout);
 }
 return 0;
}

int vumeter (int fh, int samples, int bits, int channels) {
 void * buf = malloc(samples*bits*2);

 if ( !buf )
  return -1;

 if ( bits == 16 ) {
  if ( channels == 2 ) {
   vumeter16bit2ch(fh, samples, (int16_t *) buf);
   free(buf);
   return 0;
  } else {
   return -1;
  }
 } else {
  return -1;
 }
}

int main (int argc, char * argv[]) {
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 int    samples  = 441;
 char * server   = NULL;
 char * k;
 int    fh;
 int    i;

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
  } else if ( strcmp(k, "--samples") == 0 ) {
   samples = atoi(argv[++i]);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( (fh = roar_simple_monitor(rate, channels, bits, codec, server, "roarvumeter")) == -1 ) {
  fprintf(stderr, "Error: can not start playback\n");
  return 1;
 }

 vumeter(fh, samples*channels, bits, channels);

 printf("\n"); // if the reach this then roard has quited and we should print a newline

 roar_simple_close(fh);

 return 0;
}

//ll
