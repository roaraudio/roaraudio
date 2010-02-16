//roarmon.c:

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
 printf("roarmon [OPTIONS]... [FILE]\n");

 printf("\nOptions:\n\n");

 printf("  --server   SERVER    - Set server hostname\n"
        "  --rate  -R RATE      - Set sample rate\n"
        "  --bits  -B BITS      - Set bits per sample\n"
        "  --chans -C CHANNELS  - Set number of channels\n"
        "  --codec    CODEC     - Set the codec\n"
        "  --wave               - Output Wave Audio (PCM)\n"
        "  --midi               - Output MIDI Audio\n"
        "  --light              - Output light control\n"
        "  --raw                - Output raw data\n"
        "  --complex            - Output complex data\n"
        "  --rdtcs              - Output Radio Data and Transmitter Control System data\n"
        "  --thru               - Output copy of other stream\n"
        "  --prethru            - Sets prethru flag on stream\n"
        "  --rel-id ID          - Set ID of relative stream\n"
        "  --help               - Show this help\n"
       );

}

int main (int argc, char * argv[]) {
 int    rate     = -1;
 int    bits     = -1;
 int    channels = -1;
 int    codec    = -1;
 int    dir      = ROAR_DIR_MONITOR;
 int    rel_id   = -1;
 char * server   = NULL;
 char * k;
 int    i;
 int    prethru  = 0;
 struct roar_connection    con;
 struct roar_stream        s;
 struct roar_vio_calls     file, stream;
 struct roar_vio_defaults  def;
 int file_opened = 0;

 if ( roar_vio_open_fh(&file, ROAR_STDOUT) == -1 )
  return 1;

 if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, O_CREAT|O_TRUNC|O_WRONLY, 0644) == -1 )
  return 1;

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
   if ( (codec = roar_str2codec(argv[++i])) == -1 ) {
    fprintf(stderr, "Error: Unknown codec: %s\n", argv[i]);
    return 1;
   }

  } else if ( !strcmp(k, "--wave") ) {
   dir   = ROAR_DIR_MONITOR;
  } else if ( !strcmp(k, "--midi") ) {
   dir      = ROAR_DIR_MIDI_OUT;
  } else if ( !strcmp(k, "--light") ) {
   dir   = ROAR_DIR_LIGHT_OUT;
  } else if ( !strcmp(k, "--raw") ) {
   dir   = ROAR_DIR_RAW_OUT;
  } else if ( !strcmp(k, "--complex") ) {
   dir   = ROAR_DIR_COMPLEX_OUT;
  } else if ( !strcmp(k, "--rdtcs") ) {
   dir   = ROAR_DIR_RDTCS_OUT;
  } else if ( !strcmp(k, "--thru") ) {
   dir   = ROAR_DIR_THRU;
  } else if ( !strcmp(k, "--rel-id") ) {
   rel_id = atoi(argv[++i]);

  } else if ( !strcmp(k, "--prethru") ) {
   prethru = 1;

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

 switch (dir) {
  case ROAR_DIR_MONITOR:
    if ( rate     == -1 ) rate     = ROAR_RATE_DEFAULT;
    if ( bits     == -1 ) bits     = ROAR_BITS_DEFAULT;
    if ( channels == -1 ) channels = ROAR_CHANNELS_DEFAULT;
    if ( codec    == -1 ) codec    = ROAR_CODEC_DEFAULT;
   break;
  case ROAR_DIR_MIDI_OUT:
    if ( rate     == -1 ) rate     = 0;
    if ( bits     == -1 ) bits     = ROAR_MIDI_BITS;
    if ( channels == -1 ) channels = ROAR_MIDI_CHANNELS_DEFAULT;
    if ( codec    == -1 ) codec    = ROAR_CODEC_MIDI;
   break;
  case ROAR_DIR_LIGHT_OUT:
    if ( rate     == -1 ) rate     = 0;
    if ( bits     == -1 ) bits     = ROAR_LIGHT_BITS;
    if ( channels == -1 ) channels = 0;
    if ( codec    == -1 ) codec    = ROAR_CODEC_DMX512;
   break;
  case ROAR_DIR_COMPLEX_OUT:
    if ( rate     == -1 ) rate     = ROAR_COMPLEX_RATE;
    if ( bits     == -1 ) bits     = ROAR_COMPLEX_BITS;
    if ( channels == -1 ) channels = ROAR_COMPLEX_CHANNELS;
    if ( codec    == -1 ) codec    = ROAR_COMPLEX_CODEC;
   break;
  case ROAR_DIR_RDTCS_OUT:
    if ( rate     == -1 ) rate     = ROAR_RDTCS_RATE;
    if ( bits     == -1 ) bits     = ROAR_RDTCS_BITS;
    if ( channels == -1 ) channels = ROAR_RDTCS_CHANNELS;
    if ( codec    == -1 ) codec    = ROAR_RDTCS_CODEC;
   break;
  case ROAR_DIR_RAW_OUT:
  case ROAR_DIR_THRU:
  default:
    if ( rate     == -1 ) rate     = 0;
    if ( bits     == -1 ) bits     = 0;
    if ( channels == -1 ) channels = 0;
    if ( codec    == -1 ) codec    = ROAR_CODEC_DEFAULT;
   break;
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

 if ( prethru ) {
  if ( roar_stream_set_flags(&con, &s, ROAR_FLAG_PRETHRU, 0) == -1 ) {
   fprintf(stderr, "Error: can not set prethru flag on stream\n");
   roar_disconnect(&con);
   return 14;
  }
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

// TODO: FIXME:
// ROAR_SHUTDOWN(fh, SHUT_WR); // we need to have something do do shutdowns here...

 roar_vio_copy_data(&file, &stream);

 roar_vio_close(&stream);
 roar_vio_close(&file);

 return 0;
}

//ll
