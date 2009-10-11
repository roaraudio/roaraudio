//roarinterconnect.c:

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

#ifdef ROAR_HAVE_ESD
#include <esd.h>
#endif

#define MT_NONE     0x00
#define MT_MASK    0xF0
#define MT_ROAR     0x10
#define MT_ESD      0x20
#define MT_DEFAULT  MT_ROAR

#define ST_NONE     0x00
#define ST_MASK     0x0F
#define ST_BIDIR    0x01
#define ST_FILTER   0x02
#define ST_TRANSMIT 0x03
#define ST_RECEIVE  0x04
// no default here as the default depend on the server type

void usage (void) {
 printf("roarinterconnect [OPTIONS]...\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --remote SERVER    - Set remote server\n"
        "  --type   TYPE      - Set type of remote server\n"
        "  --rate   RATE      - Set sample rate\n"
        "  --bits   BITS      - Set bits per sample\n"
        "  --chans  CHANNELS  - Set number of channels\n"
        "  --codec  CODEC     - Set the codec\n"
        "  --help             - Show this help\n"
       );

}

int parse_type (char * type) {
 int ret = MT_NONE|ST_NONE;
 char * colon;

 if ( type != NULL ) {
  while (type != NULL && *type) {
   if ( (colon = strstr(type, ":")) != NULL ) {
    *colon = 0;
     colon++;
   }

   if ( !strcmp(type, "roar") ) {
    ret -= ret & MT_MASK;
    ret += MT_ROAR;
   } else if ( !strcmp(type, "esd") ) {
    ret -= ret & MT_MASK;
    ret += MT_ESD;
   } else if ( !strcmp(type, "bidir") ) {
    ret -= ret & ST_MASK;
    ret += ST_BIDIR;
   } else if ( !strcmp(type, "filter") ) {
    ret -= ret & ST_MASK;
    ret += ST_FILTER;
   } else if ( !strcmp(type, "transmit") ) {
    ret -= ret & ST_MASK;
    ret += ST_TRANSMIT;
   } else if ( !strcmp(type, "receive") ) {
    ret -= ret & ST_MASK;
    ret += ST_RECEIVE;
   } else {
    return MT_NONE|ST_NONE;
   }

   type = colon;
  }
 }

 if ( (ret & MT_MASK) == MT_NONE )
  ret |= MT_DEFAULT;

 if ( (ret & ST_MASK) == ST_NONE ) {
  switch (ret & MT_MASK) {
   case MT_ROAR: ret |= ST_BIDIR;  break;
   case MT_ESD:  ret |= ST_FILTER; break;
   default:
     return MT_NONE|ST_NONE; // error case
    break;
  }
 }

 return ret;
}

int main (int argc, char * argv[]) {
 struct roar_connection con[1];
 struct roar_stream     stream[1];
 int    rate     = 44100;
 int    bits     = 16;
 int    channels = 2;
 int    codec    = ROAR_CODEC_DEFAULT;
 int    type     = parse_type(NULL);
 int    tmp;
 char * server   = NULL;
 char * remote   = NULL;
 char * k;
 int    rfh;
 int    i;
 int    localdir = ROAR_DIR_BIDIR;

 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--remote") == 0 ) {
   remote = argv[++i];
  } else if ( strcmp(k, "--type") == 0 ) {
   type = parse_type(argv[++i]);
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
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 switch (type & MT_MASK) {
  case MT_ROAR:
    switch (type & ST_MASK) {
     case ST_BIDIR:
       tmp      = ROAR_DIR_BIDIR;
      break;
     case ST_FILTER:
       tmp      = ROAR_DIR_FILTER;
      break;
     case ST_TRANSMIT:
       tmp      = ROAR_DIR_PLAY;
       localdir = ROAR_DIR_MONITOR;
      break;
     case ST_RECEIVE:
       tmp      = ROAR_DIR_MONITOR;
       localdir = ROAR_DIR_PLAY;
      break;
     default:
       fprintf(stderr, "Error: unknown stream type\n");
       return 2;
    }
    rfh = roar_simple_stream(rate, channels, bits, codec, remote, tmp, "roarinterconnect");
   break;
#ifdef ROAR_HAVE_ESD
  case MT_ESD:
    if ( (type & ST_MASK) != ST_FILTER ) {
     fprintf(stderr, "Error: server type only supports stream type filter\n");
     return 2;
    }

    tmp = ESD_STREAM|ESD_PLAY;

    switch (bits) {
     case  8: tmp |= ESD_BITS8;  break;
     case 16: tmp |= ESD_BITS16; break;
     default:
       fprintf(stderr, "Error: EsounD only supports 8 and 16 bit streams\n");
       return 2;
    }

    switch (channels) {
     case 1: tmp |= ESD_MONO;   break;
     case 2: tmp |= ESD_STEREO; break;
     default:
       fprintf(stderr, "Error: EsounD only supports mono and stereo streams\n");
       return 2;
    }

    // this is only true if the esd runs on a LE system,...
    if ( bits == 8 && codec != ROAR_CODEC_PCM_U_LE ) {
     fprintf(stderr, "Error: EsounD only supports unsigned PCM in 8 bit mode\n");
     return 2;
    } else if ( bits == 16 && codec != ROAR_CODEC_DEFAULT ) {
     fprintf(stderr, "Error: EsounD only supports signed PCM in 16 bit mode\n");
     return 2;
    }

    rfh = esd_filter_stream(tmp, rate, remote, "roarinterconnect");
   break;
#endif
  default:
    fprintf(stderr, "Error: unknown/not supported server type\n");
    return 2;
 }

 if ( rfh == -1 ) {
  fprintf(stderr, "Error: can not connect to remote server\n");
  return 10;
 }

 if ( roar_simple_connect(con, server, "roarinterconnect") == -1 ) {
  fprintf(stderr, "Can not connect to local server\n");
  return 20;
 }

 if ( roar_stream_new(stream, rate, channels, bits, codec) == -1 ) {
  roar_disconnect(con);
  return 21;
 }

 if ( roar_stream_connect(con, stream, localdir) == -1 ) {
  roar_disconnect(con);
  return 22;
 }

 if ( roar_stream_passfh(con, stream, rfh) == -1 ) {
  roar_disconnect(con);
  return 23;
 }

 roar_simple_close(rfh);

 if ( roar_stream_attach_simple(con, stream, 0) == -1 ) {
  fprintf(stderr, "Can not attach remote stream to local server\n");
 }

 roar_disconnect(con);

 return 0;
}

//ll
