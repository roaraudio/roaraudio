//roarfilt.c:

#include <roaraudio.h>
#include <math.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarcat [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
        "  --help             - Show this help\n"
        "\n"
        "  --half             - half the volume\n"
        "  --double           - double the volume\n"
        "  --amp VAL          - Set amplification\n"
        "  --mul VAL          - Set mul\n"
        "  --div VAL          - Set div\n"
       );

}

void vol2 (void * data, int mul, int div, int len) {
 int16_t * samples = (int16_t *) data;
 int i;

 len /= 2;

 for (i = 0; i < len; i++)
  samples[i] = ((int) samples[i] * mul) / div;
}

void vol1 (void * data, int mul, int div, int len) {
 int8_t * samples = (int8_t *) data;
 int i;

 for (i = 0; i < len; i++)
  samples[i] = ((int) samples[i] * mul) / div;
}

void logs2 (void * data, float scale, int len) {
 int16_t * samples = (int16_t *) data;
 int i;
 float div = logf(scale);
 float scalemul = scale - 1;
 int neg;

 len /= 2;

 //printf("logs2(data=%p, scale=%f, len=%i): scalemul=%f, div=%f\n", data, scale, len, scalemul, div);

 for (i = 0; i < len; i++) {
  if ( (neg = (samples[i] < 0)) ) 
   samples[i] = abs(samples[i]);


  samples[i] = (neg ? 32768.0 : 32767.0)*logf(1 + (scalemul*(float)samples[i]/(neg ? 32768.0 : 32767.0))) / div;

  if ( neg )
   samples[i] *= -1;
 }
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
 int    mul = 1, div = 1;
 float  logscale = 0;
 char buf[BUFSIZE];

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--rate") == 0 ) {
   rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 || strcmp(k, "--chans") == 0 ) {
   channels = atoi(argv[++i]);
  } else if ( strcmp(k, "--half") == 0 ) {
   div *= 2;
  } else if ( strcmp(k, "--double") == 0 ) {
   mul *= 2;
  } else if ( strcmp(k, "--amp") == 0 ) {
   mul *= atoi(argv[++i]);
  } else if ( strcmp(k, "--mul") == 0 ) {
   mul  = atoi(argv[++i]);
  } else if ( strcmp(k, "--div") == 0 ) {
   div  = atoi(argv[++i]);
  } else if ( strcmp(k, "--log") == 0 ) {
   logscale = atof(argv[++i]);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( (fh = roar_simple_filter(rate, channels, bits, codec, server, "roarifilt")) == -1 ) {
  fprintf(stderr, "Error: can not start playback\n");
  return 1;
 }

 if ( mul == div && logscale == 0 ) {
  fprintf(stderr, "Error: filter is useless!\n");
  return 0;
 }

 if ( bits == 16 ) {
  while((i = read(fh, buf, BUFSIZE))) {
   if ( mul != div )
    vol2((void*)buf, mul, div, i);
   if ( logscale )
    logs2((void*)buf, logscale, i);
   if (write(fh, buf, i) != i)
    break;
  }
 } else if ( bits == 8 ) {
  while((i = read(fh, buf, BUFSIZE))) {
   vol1((void*)buf, mul, div, i);
   if (write(fh, buf, i) != i)
    break;
  }
 } else {
  fprintf(stderr, "Error: %i bits per sample is not supported!\n", bits);
  return 1;
 }

 roar_simple_close(fh);

 return 0;
}

//ll
