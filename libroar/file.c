//file.c:

#include "libroar.h"

#define BUFSIZE 8192
#define BUFMAX  65536

int roar_file_codecdetect(char * buf, int len) {
 int codec = -1;

 if ( len > 3 ) {
  if ( strncmp(buf, "OggS", 4) == 0 ) {
   codec = ROAR_CODEC_OGG_GENERAL;
   if ( len > 34 ) { // this is 7 bytes after the end of the header
    if ( strncmp(buf+28, "\001vorbis", 7) == 0 )
     codec = ROAR_CODEC_OGG_VORBIS;
   }
  } else if ( strncmp(buf, "MThd", 4) == 0 ) {
   codec = ROAR_CODEC_MIDI_FILE;
  } else if ( strncmp(buf, "RIFF", 4) == 0 ) {
   if ( len > 15 ) {
    if ( strncmp(buf+8, "WAVEfmt ", 8) == 0 )
     codec = ROAR_CODEC_RIFF_WAVE;
   }
  } else if ( strncmp(buf, "Roar", 4) == 0 ) {
   if ( len > ROAR_SPEEX_MAGIC_LEN ) {
    if ( strncmp(buf, ROAR_SPEEX_MAGIC, ROAR_SPEEX_MAGIC_LEN) == 0 )
     codec = ROAR_CODEC_ROAR_SPEEX;
   }
  }
 }

 return codec;
}

ssize_t roar_file_send_raw (int out, int in) {
 ssize_t r = 0;
 ssize_t ret;
 int len;
 char buf[BUFSIZE];
#ifdef __linux__
 int cork_new = 1, cork_old;
 socklen_t cork_len = sizeof(int);

 if ( getsockopt(out, IPPROTO_TCP, TCP_CORK, &cork_old, &cork_len) == -1 ) {
  cork_old = -1;
 } else {
  setsockopt(out, IPPROTO_TCP, TCP_CORK, &cork_new, sizeof(int));
 }
#endif

#ifdef ROAR_HAVE_LINUX_SENDFILE
 while ((ret = sendfile(out, in, NULL, BUFMAX)) > 0)
  r += ret;
#endif

 // TODO: try mmap here!

 while ((len = read(in, buf, BUFSIZE)) > 0)
  r += write(out, buf, len);

#ifdef __linux__
 if ( cork_old != -1 )
  setsockopt(out, IPPROTO_TCP, TCP_CORK, &cork_old, cork_len);
#endif
 return r;
}

ssize_t roar_file_play (struct roar_connection * con, char * file, int exec) {
 int codec = -1;
 int in, out = -1;
 struct roar_stream s;
 ssize_t r = 0;
 int len;
 char buf[BUFSIZE];
 int rate = ROAR_RATE_DEFAULT, channels = ROAR_CHANNELS_DEFAULT, bits = ROAR_BITS_DEFAULT;

 if ( !con )
  return -1;

 if ( !file )
  return -1;

 if ( (in = open(file, O_RDONLY, 0644)) == -1 ) {
  return -1;
 }

 if ((len = read(in, buf, BUFSIZE)) < 1) {
  close(in);
  return -1;
 }

 codec = roar_file_codecdetect(buf, len);

 if ( codec == -1 ) {
  close(in);
  return -1;
 }

 if ( exec ) {
  if ( roar_stream_new(&s, rate, channels, bits, codec) == -1 ) {
   close(in);
   return -1;
  }

  if ( roar_stream_connect(con, &s, ROAR_DIR_PLAY) == -1 ) {
   close(in);
   return -1;
  }

  if ( roar_stream_exec(con, &s) == -1 ) {
   close(in);
   return -1;
  }

  shutdown(con->fh, SHUT_RD);

  out = con->fh;
 } else {
  if ( (out = roar_simple_new_stream(con, rate, channels, bits, codec, ROAR_DIR_PLAY)) == -1 ) {
   close(in);
   return -1;
  }
 }

 write(out, buf, len);

 r = roar_file_send_raw(out, in);

 close(out);

 if ( exec ) {
  con->fh = -1;
 }

 close(in);

 return r;
}

//ll
