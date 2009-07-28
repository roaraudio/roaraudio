//roarcatpassfh.c:

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

#define BUFSIZE (1024*2)

void usage (void) {
 printf("roarcatpassfh [OPTIONS]...\n");

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
 int    i;
 int    in = ROAR_STDIN;
 struct roar_connection con;
 struct roar_stream     s;


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
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( roar_simple_connect(&con, server, "roarcatad") == -1 ) {
  ROAR_DBG("roar_simple_play(*): roar_simple_connect() faild!");
  return 1;
 }

 if ( roar_stream_new(&s, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(&con);
  return 1;
 }

 if ( roar_stream_connect(&con, &s, ROAR_DIR_PLAY) == -1 ) {
  roar_disconnect(&con);
  return 1;
 }

 if ( roar_stream_passfh(&con, &s, in) == -1 ) {
  roar_disconnect(&con);
  return 1;
 }

 close(in);

 sleep(1000);

 roar_disconnect(&con);

 return 0;
}

//ll
