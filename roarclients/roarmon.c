//roarcat.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roarclients a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <roaraudio.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarmon [OPTIONS]... [FILE]\n");

 printf("\nOptions:\n\n");

 printf("  --server   SERVER    - Set server hostname\n"
        "  --rate  -R RATE      - Set sample rate\n"
        "  --bits  -B BITS      - Set bits per sample\n"
        "  --chans -C CHANNELS  - Set number of channels\n"
        "  --codec    CODEC     - Set the codec\n"
        "  --help               - Show this help\n"
       );

}

int main (int argc, char * argv[]) {
 int    rate     = ROAR_RATE_DEFAULT;
 int    bits     = ROAR_BITS_DEFAULT;
 int    channels = ROAR_CHANNELS_DEFAULT;
 int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 char * k;
 int    fh;
 int    i;
 int    out = -1;
 char buf[BUFSIZE];

 for (i = 1; i < argc; i++) {
  k = argv[i];

 //         esdmon [-s server ][-b] [-m] [-r freq] < file

  if ( !strcmp(k, "--server") || !strcmp(k, "-s") ) {
   server = argv[++i];
  } else if ( !strcmp(k, "--rate") || !strcmp(k, "-r") || !strcmp(k, "-R") ) {
   rate = atoi(argv[++i]);
  } else if ( !strcmp(k, "--bits") || !strcmp(k, "-B") ) {
   bits = atoi(argv[++i]);
  } else if ( !strcmp(k, "-b") ) {
   bits = 8;
  } else if ( !strcmp(k, "--channels") || !strcmp(k, "--chans") || !strcmp(k, "-C") ) {
   channels = atoi(argv[++i]);
  } else if ( !strcmp(k, "-m") ) {
   channels = 2;
  } else if ( !strcmp(k, "--codec") ) {
   codec = roar_str2codec(argv[++i]);
  } else if ( !strcmp(k, "--help") || !strcmp(k, "-h") ) {
   usage();
   return 0;
  } else if ( out == -1 ) {
   if ( (out = open(k, O_CREAT|O_TRUNC|O_WRONLY, 0644)) == -1 ) {
    fprintf(stderr, "Error: can not open file: %s: %s\n", k, strerror(errno));
    return 1;
   }
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( out == -1 )
  out = ROAR_STDOUT;

 if ( (fh = roar_simple_monitor(rate, channels, bits, codec, server, "roarmon")) == -1 ) {
  fprintf(stderr, "Error: can not start monitoring\n");
  return 1;
 }

 while((i = read(fh, buf, BUFSIZE)))
  if (write(out, buf, i) != i)
   break;

 roar_simple_close(fh);

 return 0;
}

//ll
