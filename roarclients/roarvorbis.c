//roarvorbis.c:

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

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#ifdef ROAR_HAVE_LIBVORBISFILE
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#endif

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

#define BUFSIZE 1024

void usage (void) {
 printf("roarvorbis [OPTIONS]... FILE\n");

 printf("\nOptions:\n\n");

 printf("  --server SERVER    - Set server hostname\n"
        "  --help             - Show this help\n"
       );

}


#ifdef ROAR_HAVE_LIBVORBISFILE
FILE * open_http (char * file) {
 char cmd[1024];

 snprintf(cmd, 1023, "wget -qO - '%s'", file);

 return popen(cmd, "r");
}

int update_stream (struct roar_connection * con, struct roar_stream * s, int * out, OggVorbis_File * vf, char * file, struct roar_audio_info * info) {
 vorbis_info *vi = ov_info(vf, -1);
 int    bits     = 16;
 int    codec    = ROAR_CODEC_DEFAULT;
 char **ptr = ov_comment(vf, -1)->user_comments;
 char key[ROAR_META_MAX_NAMELEN], value[LIBROAR_BUFFER_MSGDATA] = {0};
 int j, h = 0;
 struct roar_meta   meta;
 int need_new_stream = 0;
 int meta_ok;

 fprintf(stderr, "\n");

 if ( *out == -1 ) {
  need_new_stream = 1;
 } else if ( info->rate != vi->rate || info->channels != vi->channels ) {
  need_new_stream = 1;
 }

 if ( need_new_stream ) {
  if ( *out != -1 )
  close(*out);

  fprintf(stderr, "Audio: %i channel, %liHz\n\n", vi->channels, vi->rate);

  info->rate     = vi->rate;
  info->channels = vi->channels;

  if ( (*out = roar_simple_new_stream_obj(con, s, vi->rate, vi->channels, bits, codec, ROAR_DIR_PLAY)) == -1 ) {
   roar_disconnect(con);
   return -1;
  }
 }


 meta.value = value;
 meta.key[0] = 0;
 meta.type = ROAR_META_TYPE_NONE;

 roar_stream_meta_set(con, s, ROAR_META_MODE_CLEAR, &meta);

 if ( strncmp(file, "http:", 5) == 0 )
  meta.type = ROAR_META_TYPE_FILEURL;
 else
  meta.type = ROAR_META_TYPE_FILENAME;


 strncpy(value, file, LIBROAR_BUFFER_MSGDATA-1);
 value[LIBROAR_BUFFER_MSGDATA-1] = 0;
 roar_stream_meta_set(con, s, ROAR_META_MODE_SET, &meta);

 while(*ptr){
  meta_ok = 1;

   for (j = 0; (*ptr)[j] != 0 && (*ptr)[j] != '='; j++) {
    if ( j == ROAR_META_MAX_NAMELEN ) {
     ROAR_ERR("update_stream(*): invalid meta data: meta data key too long");
     meta_ok = 0;
     j = 0;
     break;
    }
    key[j] = (*ptr)[j];
   }
   key[j] = 0;

   if ( meta_ok ) {
    for (j++, h = 0; (*ptr)[j] != 0 && (*ptr)[j] != '='; j++) {
     if ( h == LIBROAR_BUFFER_MSGDATA ) {
      ROAR_ERR("update_stream(*): invalid meta data: meta data value for key '%s' too long", key);
      meta_ok = 0;
      h = 0;
      break;
     }
     value[h++] = (*ptr)[j];
    }
    value[h]   = 0;
   }

   if ( meta_ok ) {
    fprintf(stderr, "Meta %-16s: %s\n", key, value);

    meta.type = roar_meta_inttype(key);
    if ( meta.type != -1 )
     roar_stream_meta_set(con, s, ROAR_META_MODE_SET, &meta);
   }

   ptr++;
 }

 return 0;
}

#endif

int main (int argc, char * argv[]) {
#ifndef ROAR_HAVE_LIBVORBISFILE
 fprintf(stderr, "Error: no Vorbis support!\n");
 return 1;
#else
 char * server   = NULL;
 char * file     = NULL;
 char * k;
 int    i;
 FILE * in;
 int    out = -1;
 struct roar_connection con;
 struct roar_stream     s;
 OggVorbis_File vf;
 int eof=0;
 int current_section = -1;
 int last_section = -1;
 struct roar_audio_info info;
 char pcmout[4096];


 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "--server") == 0 ) {
   server = argv[++i];
  } else if ( strcmp(k, "--help") == 0 ) {
   usage();
   return 0;
  } else if ( file == NULL ) {
   file = k;
  } else {
   fprintf(stderr, "Error: unknown argument: %s\n", k);
   usage();
   return 1;
  }
 }

 if ( roar_simple_connect(&con, server, "roarvorbis") == -1 ) {
  ROAR_DBG("roar_simple_play(*): roar_simple_connect() faild!");
  return -1;
 }

 if ( strncmp(file, "http:", 5) == 0 ) {
  in = open_http(file);
 } else {
  in = fopen(file, "rb");
 }

 if ( in == NULL ) {
  roar_disconnect(&con);
  return -1;
 }

#ifdef _WIN32
  _setmode(_fileno(in), _O_BINARY);
#endif

 if(ov_open(in, &vf, NULL, 0) < 0) {
  fprintf(stderr,"Input does not appear to be an Ogg bitstream.\n");
  roar_disconnect(&con);
  return -1;
 }

// if ( update_stream(&con, &s, &out, &vf, file) == -1 )
//  return -1;

 while (!eof) {
  long ret = ov_read(&vf, pcmout, sizeof(pcmout), 0, 2, 1, &current_section);

  if ( last_section != current_section )
   if ( update_stream(&con, &s, &out, &vf, file, &info) == -1 )
    return 1;

  last_section = current_section;

  if (ret == 0) {
   /* EOF */
   eof=1;
  } else if (ret < 0) {
   /* error in the stream.  Not a problem, just reporting it in
      case we (the app) cares.  In this case, we don't. */
  } else {
     /* we don't bother dealing with sample rate changes, etc, but
        you'll have to */
//    write(out, pcmout, ret);
   roar_stream_send_data(&con, &s, pcmout, ret);
  }
 }

  ov_clear(&vf);

// fclose(in);
 close(out);
 roar_disconnect(&con);

 return 0;
#endif
}

//ll
