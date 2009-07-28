//roarcat.c:

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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <roaraudio.h>

#define BUFSIZE 1024

void usage (void) {
 printf("roarcat [OPTIONS]... [FILE]\n");

 printf("\nOptions:\n\n");

 printf("  --server    SERVER    - Set server hostname\n"
        "  --rate  -R  RATE      - Set sample rate\n"
        "  --bits  -B  BITS      - Set bits per sample\n"
        "  --chans -C  CHANNELS  - Set number of channels\n"
        "  --codec     CODEC     - Set the codec\n"
        "  --wave                - Use Wave Audio (PCM) as input\n"
        "  --midi                - Use MIDI Audio as input\n"
        "  --light               - Use light control input\n"
        "  --raw                 - Use raw input\n"
        "  --rel-id ID          - Set ID of relative stream\n"
        "  --help                - Show this help\n"
       );

}

int main (int argc, char * argv[]) {
 int    rate     = ROAR_RATE_DEFAULT;
 int    bits     = ROAR_BITS_DEFAULT;
 int    channels = ROAR_CHANNELS_DEFAULT;
 int    codec    = ROAR_CODEC_DEFAULT;
 int    dir      = ROAR_DIR_PLAY;
 int    rel_id   = -1;
 char * server   = NULL;
 char * k;
 int    i;
 char * name = "roarcat";
 struct roar_connection    con;
 struct roar_stream        s;
 struct roar_vio_calls     file, stream;
 struct roar_vio_defaults  def;
 int file_opened = 0;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( !strcmp(k, "--server") || !strcmp(k, "-s") ) {
   server = argv[++i];
  } else if ( !strcmp(k, "-n") ) {
   name = argv[++i];
  } else if ( !strcmp(k, "--rate") || !strcmp(k, "-r") || !strcmp(k, "-R") ) {
   rate = atoi(argv[++i]);
  } else if ( !strcmp(k, "--bits") || !strcmp(k, "-B") ) {
   bits = atoi(argv[++i]);
  } else if ( !strcmp(k, "-b") ) {
   bits = 8;
  } else if ( !strcmp(k, "--channels") || !strcmp(k, "--chans") || !strcmp(k, "-C") ) {
   channels = atoi(argv[++i]);
  } else if ( !strcmp(k, "-m") ) {
   channels = 1;
  } else if ( !strcmp(k, "--codec") ) {
   if ( (codec = roar_str2codec(argv[++i])) == -1 ) {
    fprintf(stderr, "Error: Unknown codec: %s\n", argv[i]);
    return 1;
   }

  } else if ( !strcmp(k, "--wave") ) {
   dir   = ROAR_DIR_PLAY;
  } else if ( !strcmp(k, "--midi") ) {
   dir   = ROAR_DIR_MIDI_IN;
   if ( codec == ROAR_CODEC_DEFAULT )
    codec = ROAR_CODEC_MIDI;
  } else if ( !strcmp(k, "--light") ) {
   dir   = ROAR_DIR_LIGHT_IN;
   if ( codec == ROAR_CODEC_DEFAULT )
    codec = ROAR_CODEC_DMX512;
  } else if ( !strcmp(k, "--raw") ) {
   dir   = ROAR_DIR_RAW_IN;

  } else if ( !strcmp(k, "--rel-id") ) {
   rel_id = atoi(argv[++i]);

  } else if ( !strcmp(k, "--help") || !strcmp(k, "-h") ) {
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

 if ( roar_simple_connect(&con, server, "roarmon") == -1 ) {
  fprintf(stderr, "Error: can not connect to server\n");
  return 10;
 }

 if ( roar_stream_new(&s, rate, channels, bits, codec) == -1 ) {
  fprintf(stderr, "Error: can not create stream\n");
  roar_disconnect(&con);
  return 20;
 }

 if ( rel_id != -1 ) {
  if ( roar_stream_set_rel_id(&s, rel_id) ) {
   fprintf(stderr, "Error: can not set id or realative stream\n");
   roar_disconnect(&con);
   return 21;
  }
 }

 if ( roar_stream_connect(&con, &s, dir) == -1 ) {
  fprintf(stderr, "Error: can not connect stream to server\n");
  roar_disconnect(&con);
  return 11;
 }

 if ( roar_stream_exec(&con, &s) == -1 ) {
  fprintf(stderr, "Error: can not exec stream\n");
  roar_disconnect(&con);
  return 12;
 }

 if ( roar_get_connection_vio(&con, &stream) == -1 ) {
  fprintf(stderr, "Error: can not get stream vio\n");
  roar_disconnect(&con);
  return 13;
 }

 roar_vio_copy_data(&stream, &file);

 roar_vio_close(&stream);
 roar_vio_close(&file);

 return 0;
}

//ll
