//roarcatplay.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008, 2009
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
 printf("roarcatplay [OPTIONS]... [FILE]\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --simple           - Use the simple interface (default)\n"
        "  --passive          - Use passiv playback (experimental, works only localy)\n"
        "  --background       - Use background playback, impleys passive mode\n"
        "  --verbose          - Use verbose output\n"
        "  --help             - Show this help\n"
       );

}

#define MODE_SIMPLE  1
#define MODE_PASSIVE 2

int main (int argc, char * argv[]) {
 char * server   = NULL;
 char * k;
 int    i;
 char * file    = NULL;
 int    mode    = MODE_SIMPLE;
 int    bg      = 0;
 int    verbose = 0;
#ifdef ROAR_HAVE_UNIX
 struct roar_connection con[1];
 struct roar_stream     stream[1];
#endif

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 || strcmp(k, "-s") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--simple") == 0 ) {
   mode = MODE_SIMPLE;
  } else if ( strcmp(k, "--passive") == 0 ) {
   mode = MODE_PASSIVE;
  } else if ( strcmp(k, "--background") == 0 ) {
   bg = 1;
  } else if ( strcmp(k, "--verbose") == 0 || strcmp(k, "-v") == 0 ) {
   verbose++;
  } else if ( strcmp(k, "--help") == 0 || strcmp(k, "-h") == 0 ) {
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

 if ( bg )
  mode = MODE_PASSIVE;

#ifndef ROAR_HAVE_UNIX
 if ( mode == MODE_PASSIVE ) {
  fprintf(stderr, "Error: passive mode is not supported on this system.\n");
  return 1;
 }
#endif

 if ( file == NULL )
  file = "/dev/stdin";

#ifdef ROAR_HAVE_UNIX
 if ( mode == MODE_PASSIVE ) {
  if ( roar_simple_connect(con, server, "roarcatplay") == -1 ) {
   ROAR_ERR("Can not connect to server");
   return 0;
  }

  if ( roar_file_play_full(con, file, 0, 1, stream) == -1 ) {
   ROAR_ERR("Can not start playback");
   return 1;
  }

  if ( bg ) {
   if ( roar_stream_attach_simple(con, stream, 0) == -1 ) {
    ROAR_ERR("Can not attach stream to server");
   }
  } else {
   sleep(10);
  }

  roar_disconnect(con);

 } else { // MODE_SIMPLE
#endif
  if ( roar_simple_play_file(file, server, "roarcatplay") == -1 ) {
   ROAR_ERR("Can not start playback");
   return 1;
  }
#ifdef ROAR_HAVE_UNIX
 }
#endif

 return 0;
}

//ll
