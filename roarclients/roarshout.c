//roarshout.c:

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

#ifdef ROAR_HAVE_LIBSHOUT
#include <shout/shout.h>

#define BUFSIZE 2048

void usage (void) {
 printf("roarmon [OPTIONS]... [address [port [password [mountpoint]]]\n");

 printf("\nRoarAudio Options:\n\n");

 printf("    --server SERVER    - Set server hostname\n"
        "    --rate   RATE      - Set sample rate\n"
        "    --bits   BITS      - Set bits per sample\n"
        "    --chans  CHANNELS  - Set number of channels\n"
        "    --codec  CODEC     - Set the codec\n"
        " -h --help             - Show this help\n"
       );

 printf("\nlibshout Options:\n\n");

 printf(" -p --public           - Allow listing in stream directory\n"
        " -d          DESC      - Set stream description\n"
        " -g          GENRE     - Set stream genre\n"
        " -n          NAME      - Set stream name\n"
        " -u          URL       - Set stream URL/homepage\n"
       );

}

int main (int argc, char * argv[]) {
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_OGG_VORBIS;
 char * server   = NULL;
 char * k;
 char * s_server = NULL;
 char * s_mount  = NULL;
 char * s_pw     = NULL;
 int    s_port   = -1;
 char * s_desc   = NULL;
 char * s_genre  = NULL;
 char * s_name   = NULL;
 char * s_url    = NULL;
 int    s_public = 0;
 int    fh;
 int    i;
 char buf[BUFSIZE];
 shout_t * shout;

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
  } else if ( strcmp(k, "-p") == 0 || strcmp(k, "--public") == 0 ) {
   s_public = 1;
  } else if ( strcmp(k, "-d") == 0 ) {
   s_desc   = argv[++i];
  } else if ( strcmp(k, "-g") == 0 ) {
   s_genre  = argv[++i];
  } else if ( strcmp(k, "-n") == 0 ) {
   s_name   = argv[++i];
  } else if ( strcmp(k, "-u") == 0 ) {
   s_url    = argv[++i];
  } else if ( strcmp(k, "-h") == 0 || strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else if ( s_server == NULL ) {
   s_server = k;
  } else if ( s_port   == -1 ) {
   s_port   = atoi(k);
  } else if ( s_pw     == NULL ) {
   s_pw     = k;
  } else if ( s_mount  == NULL ) {
   s_mount  = k;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( s_server == NULL )
  s_server = "localhost";

 if ( s_mount == NULL )
  s_mount  = "/roar.ogg";

 if ( s_pw == NULL )
  s_pw     = "hackme";

 if ( s_port == -1 )
  s_port   = 8000;

 shout_init();

 if (!(shout = shout_new())) {
  ROAR_ERR("Can not clreate shout object");
  return 1;
 }

 if (shout_set_host(shout, s_server) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting hostname: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting protocol: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_port(shout, s_port) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting port: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_password(shout, s_pw) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting password: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_mount(shout, s_mount) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting mount: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_user(shout, "source") != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting user: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_format(shout, SHOUT_FORMAT_OGG) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting format: %s", shout_get_error(shout));
  return 1;
 }

 shout_set_public(shout, s_public);

 if (s_desc  != NULL)
  shout_set_description(shout, s_desc);

 if (s_genre != NULL)
  shout_set_genre(shout, s_genre);

 if (s_name  != NULL)
  shout_set_name(shout, s_name);

 if (s_url   != NULL)
  shout_set_url(shout, s_url);

 if ( (fh = roar_simple_monitor(rate, channels, bits, codec, server, "roarshout")) == -1 ) {
  fprintf(stderr, "Error: can not start monetoring\n");
  return 1;
 }

 if (shout_open(shout) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Can not open connection via libshout!");
  return -1;
 }

 while((i = read(fh, buf, BUFSIZE)))
  if (shout_send(shout, (unsigned char*)buf, i) != SHOUTERR_SUCCESS)
   break;

 roar_simple_close(fh);

 shout_sync(shout);

 shout_close(shout);

 shout_shutdown();

 return 0;
}

#else
int main (void) {
 fprintf(stderr, "No libshout support compiled in!\n");
 return 1;
}
#endif
//ll
