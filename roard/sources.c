//sources.c:

#include "roard.h"

int sources_init (void) {
 g_source_client = -1;
 return 0;
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
 if ( strcmp(driver, "raw") == 0 ) {
  return sources_add_raw(driver, device, container, options, primary);
 } else if ( strcmp(driver, "wav") == 0 ) {
  return sources_add_wav(driver, device, container, options, primary);
 } else if ( strcmp(driver, "cf") == 0 ) {
  return sources_add_cf(driver, device, container, options, primary);
 }

 return -1;
}

int sources_add_raw (char * driver, char * device, char * container, char * options, int primary) {
 int stream;
 int fh;
 struct roar_stream * s;

 if ( (fh = open(device, O_RDONLY, 0644)) == -1 ) {
  return -1;
 }

 if ( (stream = streams_new()) == -1 ) {
  close(fh);
  return -1;
 }

 streams_get(stream, (struct roar_stream_server **)&s);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->dir        = ROAR_DIR_PLAY;
 s->pos_rel_id = -1;

 streams_set_fh(stream, fh);

 client_stream_add(g_source_client, stream);

 return 0;
}

int sources_add_wav (char * driver, char * device, char * container, char * options, int primary) {
 int stream;
 int fh;
 char buf[44];
 struct roar_stream * s;

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

 s->dir        = ROAR_DIR_PLAY;
 s->pos_rel_id = -1;

 streams_set_fh(stream, fh);

 client_stream_add(g_source_client, stream);

 return 0;
}

int sources_add_cf (char * driver, char * device, char * container, char * options, int primary) {
 int  stream;
 int  fh;
 int  codec;
 int  len;
 char buf[64];
 struct roar_stream * s;

 if ( (fh = open(device, O_RDONLY, 0644)) == -1 ) {
  return -1;
 }

 if ( !options ) {
  if ( (len = read(fh, buf, 64)) < 1 ) {
   close(fh);
   return -1;
  }

  if ( lseek(fh, -len, SEEK_CUR) == (off_t)-1 ) {
   close(fh);
   return -1;
  }

  if ( (codec = roar_file_codecdetect(buf, len)) == -1 ) {
   close(fh);
   return -1;
  }
 } else {
  if ( (codec = roar_str2codec(options)) == -1 ) {
   close(fh);
   return -1;
  }
 }

 if ( (stream = streams_new()) == -1 ) {
  close(fh);
  return -1;
 }

 streams_get(stream, (struct roar_stream_server **)&s);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->dir        = ROAR_DIR_PLAY;
 s->pos_rel_id = -1;
 s->info.codec = codec;

 streams_set_fh(stream, fh);

 client_stream_add(g_source_client, stream);

 return 0;
}

//ll
