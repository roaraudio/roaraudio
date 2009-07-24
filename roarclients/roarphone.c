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

#ifdef ROAR_HAVE_LIBSPEEX
#include <speex/speex_echo.h>
#endif

#define TIMEDIV  100

#define DRIVER  "oss"

// anti echo:
#define AE_NONE      0
#define AE_SIMPLE    1
#define AE_SPEEX     2
#define AE_ROARD     3

struct {
 int antiecho;
} g_conf;

void usage (void) {
 printf("roarcat [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server   SERVER    - Set server hostname\n"
        "  --rate     RATE      - Set sample rate\n"
        "  --bits     BITS      - Set bits per sample\n"
        "  --chans    CHANNELS  - Set number of channels\n"
        "  --codec    CODEC     - Set the codec\n"
        "  --driver   DRIVER    - Set the driver\n"
        "  --device   DEVICE    - Set the device\n"
        "  --antiecho AEMODE    - Set the anti echo mode\n"
        "  --help               - Show this help\n"
       );

}

int open_stream (struct roar_vio_calls * vio, char * server, struct roar_audio_info * info) {
 return roar_vio_simple_stream(vio,
                               info->rate, info->channels, info->bits, info->codec,
                               server,
                               ROAR_DIR_BIDIR,
                               "roarphone");
}

#ifdef ROAR_HAVE_LIBSPEEX
int anti_echo_speex16(int16_t * buf, int16_t * aebuf, size_t len, struct roar_audio_info * info) {
 static SpeexEchoState * state = NULL;
 size_t samples = info->rate / TIMEDIV;
 static int16_t * obuf = NULL;

 if ( info->channels != 1 )
  return -1;

 if (len != samples)
  return -1;

 if ( state == NULL ) {
  if ( (state = speex_echo_state_init(samples, 100*samples)) == NULL )
   return -1;

  // todo: set sample rate.
 }

 if ( obuf == NULL ) {
  if ( (obuf = malloc(2*samples)) == NULL )
   return -1;
 }

/*
 speex_echo_cancellation(state, buf, aebuf, obuf);
*/

 speex_echo_cancel(state, buf, aebuf, obuf, NULL);

 memcpy(buf, obuf, 2*samples);

 return 0;
}
#endif

int anti_echo16(int16_t * buf, int16_t * aebuf, size_t len, struct roar_audio_info * info) {
 size_t i;

 switch (g_conf.antiecho) {
  case AE_NONE:
    return 0;
   break;
  case AE_SIMPLE:
    for (i = 0; i < len; i++)
     buf[i] -= aebuf[i];
   break;
#ifdef ROAR_HAVE_LIBSPEEX
  case AE_SPEEX:
    return anti_echo_speex16(buf, aebuf, len, info);
   break;
#endif
  default:
    return -1;
   break;
 }

 return -1;
}

int run_stream (struct roar_vio_calls * s0, struct roar_vio_calls * s1, struct roar_audio_info * info) {
 size_t len;
 void * outbuf, * micbuf;
 ssize_t outlen, miclen;

 len = (info->rate / TIMEDIV) * info->channels * info->bits / 8;

 if ( (outbuf = malloc(2*len)) == NULL )
  return -1;

 micbuf = outbuf + len;

 while (1) {
  if ( (miclen = roar_vio_read(s0, micbuf, len)) <= 0 )
   break;
  if ( roar_vio_write(s1, micbuf, miclen) != miclen )
   break;
  if ( (outlen = roar_vio_read(s1, outbuf, len)) <= 0 )
   break;

  if ( g_conf.antiecho != AE_NONE )
   anti_echo16(outbuf, micbuf, ROAR_MIN(miclen, outlen)/2, info);

  if ( roar_vio_write(s0, outbuf, outlen) != outlen )
   break;
 }

 free(outbuf);

 return 0;
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

 memset(&g_conf, 0, sizeof(g_conf));

 g_conf.antiecho = AE_NONE;

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
  } else if ( strcmp(k, "--antiecho") == 0 ) {
   k = argv[++i];
   if ( !strcmp(k, "none") ) {
    g_conf.antiecho = AE_NONE;
   } else if ( !strcmp(k, "simple") ) {
    g_conf.antiecho = AE_SIMPLE;
   } else if ( !strcmp(k, "speex") ) {
    g_conf.antiecho = AE_SPEEX;
   } else if ( !strcmp(k, "roard") ) {
    g_conf.antiecho = AE_ROARD;
   } else {
    fprintf(stderr, "Error: unknown mode: %s\n", k);
    return 1;
   }
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

 run_stream(&dvio, &svio, &info);

 roar_vio_close(&svio);
 roar_vio_close(&dvio);

 return 0;
}

//ll
