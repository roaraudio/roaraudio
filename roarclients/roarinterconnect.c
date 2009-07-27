//roarinterconnect.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

void usage (void) {
 printf("roarinterconnect [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --remote SERVER    - Set remote server\n"
        "  --type   TYPE      - Set type of remote server\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
        "  --codec  CODEC     - Set the codec\n"
        "  --help             - Show this help\n"
       );

}

int main (int argc, char * argv[]) {
 struct roar_connection con[1];
 struct roar_stream     stream[1];
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 char * remote   = NULL;
 char * k;
 int    rfh;
 int    i;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--remote") == 0 ) {
   remote = argv[++i];
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
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 rfh = roar_simple_stream(rate, channels, bits, codec, remote, ROAR_DIR_BIDIR, "roarinterconnect");

 if ( rfh == -1 ) {
  fprintf(stderr, "Error: can not connect to remote server\n");
  return 10;
 }

 if ( roar_simple_connect(con, server, "roarinterconnect") == -1 ) {
  fprintf(stderr, "Can not connect to local server\n");
  return 20;
 }

 if ( roar_stream_new(stream, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(con);
  return 21;
 }

 if ( roar_stream_connect(con, stream, ROAR_DIR_BIDIR) == -1 ) {
  roar_disconnect(con);
  return 22;
 }

 if ( roar_stream_passfh(con, stream, rfh) == -1 ) {
  roar_disconnect(con);
  return 23;
 }

 roar_simple_close(rfh);

 if ( roar_stream_attach_simple(con, stream, 0) == -1 ) {
  fprintf(stderr, "Can not attach remote stream to local server\n");
 }

 roar_disconnect(con);

 return 0;
}

//ll
