//sources.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of roard a part of RoarAudio,
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

#include "roard.h"

#ifndef ROAR_WITHOUT_DCOMP_SOURCES

#define ROAR_SOURCE_DEFAULT "cf"

struct roar_source g_source[] = {
 {"raw",  "Raw source",                  "/some/file",     SRC_FLAG_FHSEC, ROAR_SUBSYS_WAVEFORM, NULL,  sources_add_raw},
#ifdef ROAR_HAVE_IO_POSIX
 {"wav",  "Old RIFF/WAVE source",        "/some/file.wav", SRC_FLAG_NONE,  ROAR_SUBSYS_WAVEFORM, sources_add_wav,  NULL},
#endif
 {"cf",   "Old CF source",               "/some/file.ext", SRC_FLAG_NONE,  ROAR_SUBSYS_WAVEFORM, sources_add_cf,   NULL},
 {"roar", "Old simple RoarAudio source", "some.host",      SRC_FLAG_NONE,  ROAR_SUBSYS_WAVEFORM, sources_add_roar, NULL},
#ifndef ROAR_WITHOUT_DCOMP_CDRIVER
 {"oss",  "OSS CDriver",                 "/dev/audio",     SRC_FLAG_NONE,  ROAR_SUBSYS_WAVEFORM, NULL, sources_add_cdriver},
#endif
 {NULL, NULL, NULL, SRC_FLAG_NONE, 0, NULL, NULL} // EOL
};

int sources_init (void) {
 g_source_client = -1;
 return 0;
}

void print_sourcelist (void) {
 int i;
 char subsys[7] = "      ";

 printf("  Source   Flag Subsys - Description (devices)\n");
 printf("------------------------------------------------------\n");

 for (i = 0; g_source[i].name != NULL; i++) {
  strncpy(subsys, "      ", 6);

  if ( g_source[i].subsystems & ROAR_SUBSYS_WAVEFORM )
   subsys[0] = 'W';
  if ( g_source[i].subsystems & ROAR_SUBSYS_MIDI )
   subsys[1] = 'M';
  if ( g_source[i].subsystems & ROAR_SUBSYS_CB )
   subsys[2] = 'C';
  if ( g_source[i].subsystems & ROAR_SUBSYS_LIGHT )
   subsys[3] = 'L';
  if ( g_source[i].subsystems & ROAR_SUBSYS_RAW )
   subsys[4] = 'R';
  if ( g_source[i].subsystems & ROAR_SUBSYS_COMPLEX )
   subsys[5] = 'X';

  printf("  %-9s %c%c%c %6s - %s (devices: %s)\n", g_source[i].name,
                g_source[i].flags & SRC_FLAG_FHSEC      ? 's' : ' ',
                g_source[i].old_open != NULL            ? 'S' : ' ',
                g_source[i].new_open != NULL            ? 'N' : ' ',
                subsys,
                g_source[i].desc, g_source[i].devices);
 }
}


int sources_set_client (int client) {
 if ( client >= 0 ) {
  g_source_client = client;
  return 0;
 } else {
  return -1;
 }
}

int sources_free (void) {
 return 0;
}

int sources_add (char * driver, char * device, char * container, char * options, int primary) {
 int i;

 if ( driver == NULL )
  driver = ROAR_SOURCE_DEFAULT;

 for (i = 0; g_source[i].name != NULL; i++) {
  if ( !strcmp(g_source[i].name, driver) ) {
   if ( g_source[i].new_open != NULL ) {
    return sources_add_new(&(g_source[i]), driver, device, container, options, primary);
   } else if ( g_source[i].old_open != NULL ) {
    return g_source[i].old_open(driver, device, container, options, primary);
   } else {
    ROAR_ERR("sources_add(driver='%s', ...): Found source but did not find any open rutine", driver);
    return -1;
   }
  }
 }

 ROAR_ERR("sources_add(driver='%s', ...): Source not found", driver);
 return -1;
}

int sources_add_new (struct roar_source * source,
                     char * driver, char * device,
                     char * container,
                     char * options, int primary) {
 int  stream;
 int  fh = -1;
 struct roar_stream        *  s;
 struct roar_stream_server * ss;
 char * k, * v;
 int error = 0;
 int f_sync = 0, f_mmap = 0;
 int codec;

 if ( source == NULL )
  return -1;

 if ( (stream = streams_new()) == -1 ) {
  return -1;
 }

 streams_get(stream, &ss);
 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 codec = s->info.codec;

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 s->pos_rel_id = -1;

 k = strtok(options, ",");
 while (k != NULL) {
  if ( (v = strstr(k, "=")) != NULL ) {
   *v++ = 0;
  }

  if ( strcmp(k, "rate") == 0 ) {
   s->info.rate = atoi(v);
  } else if ( strcmp(k, "channels") == 0 ) {
   s->info.channels = atoi(v);
  } else if ( strcmp(k, "bits") == 0 ) {
   s->info.bits = atoi(v);
  } else if ( strcmp(k, "codec") == 0 ) {
   if ( (codec = roar_str2codec(v)) == -1 ) {
    ROAR_ERR("sources_add_new(*): unknown codec '%s'", v);
    error++;
   }

  } else if ( strcmp(k, "name") == 0 ) {
   if ( streams_set_name(stream, v) == -1 ) {
    ROAR_ERR("add_output(*): Can not set Stream name");
    error++;
   }

  } else if ( strcmp(k, "mmap") == 0 ) {
   f_mmap = 1;
  } else if ( strcmp(k, "sync") == 0 ) {
   f_sync = 1;
  } else if ( strcmp(k, "primary") == 0 ) {
   primary = 1;
  } else if ( strcmp(k, "meta") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_META);
  } else if ( strcmp(k, "cleanmeta") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_CLEANMETA);
  } else if ( strcmp(k, "autoconf") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_AUTOCONF);

  } else {
   ROAR_ERR("sources_add_new(*): unknown option '%s'", k);
   error++;
  }

  if ( error ) {
   streams_delete(stream);
   if ( primary ) alive = 0;
   return -1;
  }

  k = strtok(NULL, ",");
 }

 if ( primary )
  streams_mark_primary(stream);

 streams_set_flag(stream, ROAR_FLAG_SOURCE);
 client_stream_add(g_source_client, stream);

 if ( codec == ROAR_CODEC_ALAW || codec == ROAR_CODEC_MULAW )
  s->info.bits = 8; // needed to open OSS driver, will be overriden by codecfilter

 s->info.codec = codec;
 ROAR_STREAM_SERVER(s)->codec_orgi = codec;

 if ( source->new_open(stream, device, fh, driver) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_SSTREAMID, &stream); // ignore errors here
 roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_SSTREAM,   s); // ignore errors here

 if ( f_sync ) {
  streams_set_flag(stream, ROAR_FLAG_SYNC);
 } else {
  streams_reset_flag(stream, ROAR_FLAG_SYNC);
 }

 if ( f_mmap )
  streams_set_flag(stream, ROAR_FLAG_MMAP);

 return 0;
}

int sources_add_raw  (int stream   , char * device, int fh, char * driver) {
 struct roar_stream_server * ss;

 if ( fh > -1 )
  return streams_set_fh(stream, fh);

 streams_get(stream, &ss);

 if ( roar_vio_open_file(&(ss->vio), device, O_RDONLY, 0644) == -1 )
  return -1;

 return streams_set_fh(stream, -2);
}

#ifdef ROAR_HAVE_IO_POSIX
int sources_add_wav (char * driver, char * device, char * container, char * options, int primary) {
 int stream;
 int fh;
 char buf[44];
 struct roar_stream * s;

 ROAR_WARN("sources_add_raw(*): The wav(e) source is obsolete, use source 'cf' (default)!");

 if ( (fh = open(device, O_RDONLY, 0644)) == -1 ) {
  return -1;
 }

 if (read(fh, buf, 44) != 44) {
  close(fh);
  return -1;
 }

 if ( (stream = streams_new()) == -1 ) {
  close(fh);
  return -1;
 }

 streams_get(stream, (struct roar_stream_server **)&s);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 memcpy(&(s->info.rate    ), buf+24, 4);
 memcpy(&(s->info.channels), buf+22, 2);
 memcpy(&(s->info.bits    ), buf+34, 2);

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  close(fh);
  return -1;
 }
 s->pos_rel_id = -1;

 streams_set_fh(stream, fh);

 streams_set_flag(stream, ROAR_FLAG_SOURCE);
 client_stream_add(g_source_client, stream);

 return 0;
}
#endif

#define _ret(x) streams_delete(stream); return (x)

int sources_add_cf (char * driver, char * device, char * container, char * options, int primary) {
 int  stream;
 int  codec;
 int  len;
 char buf[64];
 struct roar_stream    * s;
 struct roar_vio_calls * vio;
 struct roar_vio_defaults def;

 if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, O_RDONLY, 0644) == -1 )
  return -1;

 if ( (stream = streams_new()) == -1 ) {
  return -1;
 }

 streams_get(stream, (struct roar_stream_server **)&s);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

 s->pos_rel_id = -1;

/*
 if ( (fh = open(device, O_RDONLY, 0644)) == -1 ) {
  return -1;
 }
*/

 vio = &(ROAR_STREAM_SERVER(s)->vio);

 //if ( roar_vio_open_file(vio, device, O_RDONLY, 0644) == -1 ) {
 if ( roar_vio_open_dstr(vio, device, &def, 1) == -1 ) {
  roar_vio_init_calls(vio); // clear the VIO object again
                            // from things roar_vio_open_dstr() left.
  _ret(-1);
 }

 ROAR_DBG("sources_add_cf(*) = ?");

 // TODO: finy out a better way of doing auto detetion without need for seek!
 if ( options == NULL ) {
  if ( (len = roar_vio_read(vio, buf, 64)) < 1 ) {
   _ret(-1);
  }

  if ( roar_vio_lseek(vio, -len, SEEK_CUR) == (off_t)-1 ) {
   _ret(-1);
  }

  if ( (codec = roar_file_codecdetect(buf, len)) == -1 ) {
   _ret(-1);
  }
 } else {
  if ( !strncmp(options, "codec=", 6) )
   options += 6;

  if ( (codec = roar_str2codec(options)) == -1 ) {
   _ret(-1);
  }
 }

 s->info.codec = codec;

 ROAR_STREAM_SERVER(s)->codec_orgi = codec;

 ROAR_DBG("sources_add_cf(*) = ?");
 streams_set_fh(stream, -2);
 ROAR_DBG("sources_add_cf(*) = ?");
 streams_set_socktype(stream, ROAR_SOCKET_TYPE_FILE);

 if ( primary )
  streams_mark_primary(stream);

 streams_set_flag(stream, ROAR_FLAG_SOURCE);
 client_stream_add(g_source_client, stream);

 return 0;
}

#undef _ret


int sources_add_roar (char * driver, char * device, char * container, char * options, int primary) {
 int  stream;
 int  fh;
 int  codec = ROAR_CODEC_DEFAULT;
 struct roar_stream * s;

 if ( options != NULL && *options ) {
  if ( !strncmp(options, "codec=", 6) )
   options += 6;

  if ( (codec = roar_str2codec(options)) == -1 ) {
   return -1;
  }
 }

 if ( (fh = roar_simple_monitor(g_sa->rate, g_sa->channels, g_sa->bits, codec, device, "roard")) == -1 ) {
  return -1;
 }

 if ( (stream = streams_new()) == -1 ) {
  close(fh);
  return -1;
 }

 streams_get(stream, (struct roar_stream_server **)&s);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 if ( streams_set_dir(stream, ROAR_DIR_PLAY, 1) == -1 ) {
  streams_delete(stream);
  close(fh);
  return -1;
 }

 s->pos_rel_id = -1;
 s->info.codec = codec;

 ROAR_STREAM_SERVER(s)->codec_orgi = codec;

 streams_set_fh(stream, fh);

 if ( primary )
  streams_mark_primary(stream);

 streams_set_flag(stream, ROAR_FLAG_SOURCE);
 client_stream_add(g_source_client, stream);

 return 0;
}

#ifndef ROAR_WITHOUT_DCOMP_CDRIVER
int sources_add_cdriver (int stream   , char * device, int fh, char * driver) {
 int roar_cdriver_open(struct roar_vio_calls * calls, char * name, char * dev, struct roar_audio_info * info, int dir);
 struct roar_stream_server * ss;

 if ( fh > -1 )
  return -1;

 streams_get(stream, &ss);

 if ( !strncmp(driver, "cdriver:", 8) )
  driver += 8;

 ROAR_DBG("sources_add_cdriver(stream=%i, device='%s', fh=%i, driver='%s') = ?", stream, device, fh, driver);

 if ( roar_cdriver_open(&(ss->vio), driver, device, &(ROAR_STREAM(ss)->info), ROAR_DIR_RECORD) == -1 )
  return -1;

 return streams_set_fh(stream, -2);
}
#endif

#endif

//ll
