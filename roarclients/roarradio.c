//roarradio.c:

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

#define P_UNKNOWN 0
#define P_HTTP    1
#define P_GOPHER  2

void usage (void) {
 printf("roarradio [OPTIONS]... [FILE|URL]\n");

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
 int    codec    = ROAR_CODEC_OGG_VORBIS;
 char * server   = NULL;
 char * k;
 int    i;
 int    in = -1;
 char * file = NULL;
 char * host;
 int    port;
 FILE * http;
 struct roar_connection con[1];
 struct roar_stream     stream[1];
 char buf0[80], buf1[80];
 int proto = P_UNKNOWN;

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
  } else if ( file == NULL ) {
   file = argv[i];
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( file == NULL ) {
  file = "/dev/stdin";
  in   = ROAR_STDIN;
 } else {
  if ( strncmp(file, "http://", 7) == 0 ) {
   proto = P_HTTP;
   host  = file+7;
   port  = 80;
  } else if ( strncmp(file, "gopher://", 9) == 0 ) {
   proto = P_GOPHER;
   host  = file+9;
   port  = 70;
  }

  if ( proto == P_HTTP || proto == P_GOPHER ) {
   for (i = 0; host[i] != 0; i++) {
    if ( host[i] == ':' ) {
     port    = atoi(host+i+1); // atoi() ignores the rest after '/'
     host[i] = 0;
    } else if ( host[i] == '/' ) {
     file    = host+i;
     break;
    }
   }

   if ( !*file ) {
    file = "/";

    if ( (in = roar_socket_connect(host, port)) == -1 ) {
     ROAR_ERR("can not connect to remote server %s (port %i): %s", host, port, strerror(errno));
     return 0;
    }
   } else {
    *file = 0;

    if ( (in = roar_socket_connect(host, port)) == -1 ) {
     ROAR_ERR("can not connect to remote server %s (port %i): %s", host, port, strerror(errno));
     return 0;
    }

    *file = '/';
   }

   switch (proto) {
    case P_HTTP:
      if ( (http = fdopen(in, "r+")) == NULL ) {
       ROAR_ERR("can not create FILE* object: %s", strerror(errno));
       return 0;
      }

      fprintf(http, "GET %s HTTP/1.1\r\n", file);
      fprintf(http, "Host: %s\r\n", host);
      fprintf(http, "User-Agent: roarradio $Revision: 1.4 $\r\n");
      fprintf(http, "Connection: close\r\n");
      fprintf(http, "\r\n");
      fflush(http);

      if ( fscanf(http, "%79s %i %79s\n", buf0, &port, buf1) != 3 ) {
       ROAR_ERR("HTTP protocoll error!, no initial HTTP/1.x-line!");
       return 1;
      }
      if ( port != 200 ) { // 200 = HTTP OK
       ROAR_ERR("HTTP Error: %i - %s", port, buf1);
       return 1;
      }

      *buf0 = 0;

      while (*buf0 != '\r' && *buf0 != '\n') {
       fgets(buf0, 80, http);
      }

      fflush(http);
     break;
    case P_GOPHER:
      if ( file[0] == 0 ) {
       file[0] = '/';
       file[1] = 0;
      } else if ( file[0] == '/' && file[1] != 0 && file[2] == '/' ) { // strip the type prefix
       file += 2;
      }
      // TODO: do some error checks here
      write(in, file, strlen(file));
      write(in, "\r\n", 2);
      ROAR_SHUTDOWN(in, SHUT_WR);
     break;
   }
  } else {
   if ( (in = open(file, O_RDONLY, 0644)) == -1 ) {
    ROAR_ERR("can not open file: %s: %s", file, strerror(errno));
    return 1;
   }
  }
 }

 if ( in == -1 ) {
  ROAR_ERR("No open file, bad. This should not happen");
  return 1;
 }

 if ( roar_simple_connect(con, server, "roarradio") == -1 ) {
  ROAR_ERR("Can not connect to server");
  return 0;
 }

 if ( roar_stream_new(stream, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(con);
  return -1;
 }

 if ( roar_stream_connect(con, stream, ROAR_DIR_PLAY) == -1 ) {
  roar_disconnect(con);
  return -1;
 }

 if ( roar_stream_passfh(con, stream, in) == -1 ) {
  roar_disconnect(con);
  return -1;
 }

 close(in);

 if ( roar_stream_attach_simple(con, stream, 0) == -1 ) {
  ROAR_ERR("Can not attach stream to server");
 }

 roar_disconnect(con);

 return 0;
}

//ll
