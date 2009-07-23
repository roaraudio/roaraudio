//roarbidir.c:

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
#include "driver.h"

#define BUFSIZE 1024

#define DRIVER  "oss"

void usage (void) {
 printf("roarcat [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
        "  --codec  CODEC     - Set the codec\n"
        "  --driver DRIVER    - Set the driver\n"
        "  --device DEVICE    - Set the device\n"
        "  --help             - Show this help\n"
       );

}

int open_stream (struct roar_vio_calls * vio, char * server, struct roar_audio_info * info) {
 return roar_vio_simple_stream(vio,
                               info->rate, info->channels, info->bits, info->codec,
                               server,
                               ROAR_DIR_BIDIR,
                               "roarphone");
}

int run_stream (struct roar_vio_calls * s0, struct roar_vio_calls * s1, struct roar_audio_info * info) {
 return -1;
}

int main (int argc, char * argv[]) {
 struct roar_audio_info info = {.rate     = ROAR_RATE_DEFAULT,
                                .bits     = ROAR_BITS_DEFAULT,
                                .channels = ROAR_CHANNELS_DEFAULT,
                                .codec    = ROAR_CODEC_DEFAULT
                               };
 struct roar_vio_calls dvio, svio;
 char * driver   = DRIVER;
 char * device   = NULL;
 char * server   = NULL;
 char * k;
 int    i;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--rate") == 0 ) {
   info.rate = atoi(argv[++i]);
  } else if ( strcmp(k, "--bits") == 0 ) {
   info.bits = atoi(argv[++i]);
  } else if ( strcmp(k, "--channels") == 0 || strcmp(k, "--chans") == 0 ) {
   info.channels = atoi(argv[++i]);
  } else if ( strcmp(k, "--codec") == 0 ) {
   info.codec = roar_str2codec(argv[++i]);
  } else if ( strcmp(k, "--driver") == 0 ) {
   driver = argv[++i];
  } else if ( strcmp(k, "--device") == 0 ) {
   device = argv[++i];
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( roar_cdriver_open(&dvio, driver, device, &info, ROAR_DIR_BIDIR) == -1 ) {
  return 1;
 }

 if ( open_stream(&svio, server, &info) == -1 ) {
  roar_vio_close(&dvio);
  return 2;
 }

 run_stream(&svio, &dvio, &info);

 roar_vio_close(&svio);
 roar_vio_close(&dvio);

 return 0;
}

//ll
