//roarcatvio.c:

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
 printf("roarcat [OPTIONS]... [FILE]\n");

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
 int    rate     = ROAR_RATE_DEFAULT;
 int    bits     = ROAR_BITS_DEFAULT;
 int    channels = ROAR_CHANNELS_DEFAULT;
 int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 char * k;
 int    i;
 char * name = "roarcat";
 struct roar_vio_calls file, stream;
 struct roar_vio_defaults def;
 int file_opened = 0;

 if ( roar_vio_open_fh(&file, ROAR_STDIN) == -1 )
  return 1;

 if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, O_RDONLY, 0644) == -1 )
  return 1;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( !strcmp(k, "--server") || !strcmp(k, "-s") ) {
   server = argv[++i];
  } else if ( !strcmp(k, "-n") ) {
   name = argv[++i];
  } else if ( !strcmp(k, "--rate") || !strcmp(k, "-r") ) {
   rate = atoi(argv[++i]);
  } else if ( !strcmp(k, "--bits") ) {
   bits = atoi(argv[++i]);
  } else if ( !strcmp(k, "-b") ) {
   bits = 8;
  } else if ( !strcmp(k, "--channels") || !strcmp(k, "--chans") ) {
   channels = atoi(argv[++i]);
  } else if ( !strcmp(k, "-m") ) {
   channels = 1;
  } else if ( !strcmp(k, "--codec") ) {
   codec = roar_str2codec(argv[++i]);
  } else if ( !strcmp(k, "--help") ) {
   usage();
   return 0;
  } else if ( !file_opened ) {
   file_opened = 1;
   if ( roar_vio_open_dstr(&file, k, &def, 1) == -1 ) {
    fprintf(stderr, "Error: can not open file: %s: %s\n", k, strerror(errno));
    return 1;
   }
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( roar_vio_simple_stream(&stream, rate, channels, bits, codec, server, ROAR_DIR_PLAY, name) == -1 ) {
  fprintf(stderr, "Error: can not start playback\n");
  return 1;
 }

 roar_vio_copy_data(&stream, &file);

 roar_vio_close(&file);
 roar_vio_close(&stream);

 return 0;
}

//ll
