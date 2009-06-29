//roarlight.c:

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
#include <libroarlight/libroarlight.h>

#define CONVAR struct roar_connection * con

void usage (void) {
 printf("roarlight [OPTIONS]... command [command...]\n");

 printf("\nOptions:\n\n");

 printf("  --server    SERVER    - Set server hostname\n"
        "  --help                - Show this help\n"
       );

 printf("\nCommands:\n\n");
 printf(
        "  help                    - Show this help\n"
        "  sleep TIME              - Sleeps for TIME seconds\n"
        "  set   chan=val          - Set a DMX Channel\n"
       );
}

int cmd_set (CONVAR, char * arg) {
 char * next = arg;
 char * k, * v;
 int32_t chan, val;
 struct roar_roardmx_message mes;
 int fh;                    //TODO: we should use pure VIO here
 struct roar_vio_calls vio;

 roar_roardmx_message_new_sset(&mes);

 while (next != NULL) {
  arg  = next;
  next = strstr(next, ",");
  if ( next != NULL ) {
   *next = 0;
    next++;
  }

  k = arg;
  v = strstr(arg, "=");
  if ( v == NULL )
   return -1;

  *v = 0;
   v++;

  chan = atoi(k);
  val  = atoi(v);
//  printf("k='%s'(%i), v='%s'(%i)\n", k, chan, v, val);
  if ( roar_roardmx_message_add_chanval(&mes, chan, val) == -1 )
   return -1;
 }

 if ( (fh = roar_simple_new_stream(con,
                                   ROAR_RATE_DEFAULT, ROAR_CHANNELS_DEFAULT, ROAR_BITS_DEFAULT,
                                   ROAR_CODEC_ROARDMX, ROAR_DIR_LIGHT_IN
                                  )) == -1 )
  return -1;

 if ( roar_vio_open_fh_socket(&vio, fh) == -1 ) {
  close(fh);
  return -1;
 }

 if ( roar_roardmx_message_send(&mes, &vio) == -1 ) {
  roar_vio_close(&vio);
  return -1;
 }

 roar_vio_close(&vio);

 return 0;
}

int main (int argc, char * argv[]) {
 char * server   = NULL;
 char * k;
 int    i;
 struct roar_connection con;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( !strcmp(k, "--server") || !strcmp(k, "-s") ) {
   server = argv[++i];
  } else if ( !strcmp(k, "--codec") ) {

  } else if ( !strcmp(k, "--help") || !strcmp(k, "-h") ) {
   usage();
   return 0;
  } else if ( *k == '-' ) {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  } else {
   break;
  }
 }

 if ( roar_connect(&con, server) == -1 ) {
  fprintf(stderr, "Error: Can not connect to server\n");
  return 1;
 }

 if ( roar_identify(&con, "roarlight") == -1 ) {
  fprintf(stderr, "Error: Can not identify to server\n");
  roar_disconnect(&con);
  return 1;
 }

 if ( i == argc ) {
  fprintf(stderr, "Error: No Commands given\n");
  return 0; // this is not a fatal error...
 }

 for (; i < argc; i++) {
  k = argv[i];
  // cmd is in k

  printf("--- [ %s ] ---\n", k);

  if ( !strcmp(k, "help") ) {
   usage();

  } else if ( !strcmp(k, "sleep") ) {
   sleep(atoi(argv[++i]));

  } else if ( !strcmp(k, "set") ) {
   i++;
   if ( cmd_set(&con, argv[i]) == -1 ) {
    fprintf(stderr, "Error: can not set channels\n");
   } else {
    printf("channels changed\n");
   }

  } else {
   fprintf(stderr, "Error: invalid command: %s\n", k);
  }
 }

 roar_disconnect(&con);

 return 0;
}

//ll
