//roarbidir.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include <roaraudio.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarbidir [OPTIONS]... [IN_FILE]\n");

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
 int    in  = -1;
 int    out = -1;
 char buf[BUFSIZE];
 fd_set sl;
 struct timeval tv;
 int max_fh;

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
  } else if ( strcmp(k, "--codec") == 0 ) {
   codec = roar_str2codec(argv[++i]);
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else if ( in == -1 ) {
   if ( (in = open(k, O_RDONLY, 0644)) == -1 ) {
    fprintf(stderr, "Error: can not open file: %s: %s\n", k, strerror(errno));
    return 1;
   }
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( (fh = roar_simple_stream(rate, channels, bits, codec, server, ROAR_DIR_BIDIR, "roarbidir")) == -1 ) {
  fprintf(stderr, "Error: can not start playback\n");
  return 1;
 }

 if ( in  == -1 )
  in  = ROAR_STDIN;
 if ( out == -1 )
  out = ROAR_STDOUT;

 max_fh = (in > fh ? in : fh) + 1;
 i      = 1;

 while (i > 0) {
  FD_ZERO(&sl);
  FD_SET(in, &sl);
  FD_SET(fh, &sl);

  tv.tv_sec  = 0;
  tv.tv_usec = 50000;

  if (select(max_fh, &sl, NULL, NULL, &tv) > 0) {
   if ( FD_ISSET(fh, &sl) ) {
    if ( (i = read(fh, buf, BUFSIZE)) == -1 )
     return -1;
    if ( write(out, buf, i) != i )
     return -1;
   }
   if ( FD_ISSET(in, &sl) ) {
    if ( (i = read(in, buf, BUFSIZE)) == -1 )
     return -1;
    if ( write(fh, buf, i) != i )
     return -1;
   }
  }
 }

 roar_simple_close(fh);

 close(in);

 return 0;
}

//ll
