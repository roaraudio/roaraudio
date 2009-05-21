//roarmonhttp.c:

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

#if defined(ROAR_HAVE_SETENV) || defined(ROAR_HAVE_PUTENV)
#define _CAN_SET_ENV
#endif

#define BUFSIZE 1024

void print_header (int codec, int rate, int channels) {
 char * mime = "application/octet-stream";

 switch (codec) {
  case ROAR_CODEC_OGG_VORBIS:
    mime = "application/ogg";
   break;
  case ROAR_CODEC_RIFF_WAVE:
    mime = "audio/x-wav";
   break;
 }

 printf("Content-type: %s\r\n", mime);
 printf("ice-audio-info: ice-samplerate=%i;ice-channels=%i\r\n", rate, channels);
 printf("icy-pub:0\r\n");
 printf("Server: RoarAudio (roarmonhttp $Revision: 1.13 $)\r\n");
 printf("\r\n");

 fflush(stdout);
}

int stream (int dest, int src) {
 struct roar_buffer *ring = NULL, *cur;
 ssize_t len;
 size_t  todo;
 fd_set fsi[1], fso[1];
 struct timeval tv;
 int alive = 1;
 int maxfh = (dest > src ? dest : src) + 1;
 void * data;

 roar_socket_nonblock(src,  ROAR_SOCKET_NONBLOCK);
 roar_socket_nonblock(dest, ROAR_SOCKET_NONBLOCK);

 while (alive) {
  FD_ZERO(fsi);
  FD_ZERO(fso);
  FD_SET(src, fsi);
  if ( ring != NULL ) {
   FD_SET(dest, fso);
  }

  tv.tv_sec  = 0;
  tv.tv_usec = 100000; // 100ms

  if (select(maxfh, fsi, fso, NULL, &tv) > 0) {
   if ( FD_ISSET(src, fsi) ) { // we can read!
    if ( roar_buffer_new(&cur, BUFSIZE) == -1 )
     return -1;

    if ( roar_buffer_get_data(cur, &data) == -1 )
     return -1;

    len = read(src, data, BUFSIZE);

    switch (len) {
     case  0:
     case -1:
       roar_buffer_free(cur);

       if ( ring != NULL )
        roar_buffer_free(ring);

       return -1;
      break;
    }

    if ( roar_buffer_set_len(cur, len) == -1 )
     return -1;

    if ( ring == NULL ) {
     ring = cur;
    } else {
     roar_buffer_add(ring, cur);
    }
   } else if ( FD_ISSET(dest, fso) && ring != NULL ) { // we can write!
    if ( roar_buffer_get_data(ring, &data) == -1 )
     return -1;

    if ( roar_buffer_get_len(ring, &todo) == -1 )
     return -1;

    len = write(dest, data, todo);

    if ( len < 1 ) {
     if ( errno != EAGAIN ) {
      roar_buffer_free(ring);
      return -1;
     }
    }

    if ( todo == len ) { // we wrote all of the pkg
     if ( roar_buffer_next(&ring) == -1 )
      return -1;
    } else {
     if ( roar_buffer_set_offset(ring, len) == -1 )
      return -1;
    }

   }
  }
 }

 return 0;
}

#ifdef _CAN_SET_ENV
int parse_http (int * gopher) {
 char buf[1024];
 char * qs = buf, *str;
 ssize_t len;
 int dir = ROAR_DIR_MONITOR;

 if ( (len = read(ROAR_STDIN, buf, 1023)) == -1 )
  return -1;

 buf[len] = 0;

 if ( strncmp(buf, "GET /", 5) ) {
  if ( strncmp(buf, "SOURCE /", 8) ) {
   if ( buf[0] != '/' ) {
    return -1;
   } else {
    *gopher = 1;
   }
  } else {
   dir = ROAR_DIR_PLAY;
   qs += 3; 
  }
 }

 if ( !*gopher ) {
  qs += 5;

  if ( (str = strstr(qs, " ")) == NULL )
   return -1;

  *str = 0;
 } else {
  if ( (str = strstr(qs, "\r")) != NULL )
   *str = 0;
  if ( (str = strstr(qs, "\n")) != NULL )
   *str = 0;
 }

 for (; *qs != '?'; qs++)
  if ( !*qs )
   break;

 if ( *qs == '?' )
  qs++;

 if ( !*gopher )
  printf("HTTP/1.0 200 OK\r\n");
// printf("QS: %s\r\n", qs);

 fflush(stdout);

#ifdef ROAR_HAVE_SETENV
 setenv("QUERY_STRING", qs, 1);
#else
 // TODO: does this leak memory?
 if ( (str = malloc(strlen(qs) + strlen("QUERY_STRING=") + 1)) == NULL ) {
  return -1;
 }

 sprintf(str, "QUERY_STRING=%s", qs);

 putenv(str);
#endif

 return dir;
}
#endif

int main (int argc, char * argv[]) {
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_OGG_VORBIS;
// int    codec    = ROAR_CODEC_DEFAULT;
 char * server   = NULL;
 int    fh;
 char * c, * k, * v;
 char * sp0 = NULL, * sp1 = NULL;
 int dir = ROAR_DIR_MONITOR;
 int gopher = 0;

#ifdef ROAR_HAVE_ALARM
 alarm(0); // reset alarm timers from httpd 
#endif

 if ( argc > 1 )
  if ( ! strcmp(argv[1], "--inetd") )
#ifdef _CAN_SET_ENV
   if ( (dir = parse_http(&gopher)) == -1 )
    return 1;
#else
   return 1;
#endif

#ifdef ROAR_HAVE_STRTOK_R
 c = strtok_r(getenv("QUERY_STRING"), "&", &sp0);
#else
 c = strtok(getenv("QUERY_STRING"), "&");
#endif

 while (c != NULL) {
#ifdef ROAR_HAVE_STRTOK_R
  k = strtok_r(c,    "=", &sp1);
  v = strtok_r(NULL, "=", &sp1);
#else
  k = c;
  if ( (v = strstr(c, "=")) != NULL ) {
   *v = 0;
   v++;
  }
#endif

  if ( !strcmp(k, "codec") ) {
   if ( (codec = roar_str2codec(v)) == -1 )
    return 1;
  } else if ( !strcmp(k, "channels") ) {
   channels = atoi(v);
  } else if ( !strcmp(k, "rate") ) {
   rate = atoi(v);
  } else if ( !strcmp(k, "bits") ) {
   bits = atoi(v);
  } else {
   return 1;
  }

#ifdef ROAR_HAVE_STRTOK_R
  c = strtok_r(NULL, "&", &sp0);
#else
  c = strtok(NULL, "&");
#endif
 }


 if ( (fh = roar_simple_stream(rate, channels, bits, codec, server, dir, "roarmonhttp")) == -1 ) {
//  fprintf(stderr, "Error: can not start monitoring\n");
  return 1;
 }

 if ( !gopher )
  print_header(codec, rate, channels);

/*
 while((i = read(fh, buf, BUFSIZE)))
  if (write(out, buf, i) != i)
   break;
*/

 switch (dir) {
  case ROAR_DIR_PLAY:
    stream(fh, ROAR_STDIN);
   break;
  case ROAR_DIR_MONITOR:
    stream(ROAR_STDOUT, fh);
   break;
 }

 roar_simple_close(fh);

 return 0;
}

//ll
