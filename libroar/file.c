//file.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

#define BUFSIZE 8192
#define BUFMAX  65536

#ifdef ROAR_HAVE_IO_POSIX
#define _CAN_OPERATE
#endif

int roar_file_codecdetect(char * buf, int len) {
 int codec = -1;

 if ( len > 3 ) {
  if ( strncmp(buf, "OggS", 4) == 0 ) {
   codec = ROAR_CODEC_OGG_GENERAL;
   if ( len > 32 ) { // this is 5 bytes after the end of the header
    if ( strncmp(buf+28, "\177FLAC", 5) == 0 ) {
     codec = ROAR_CODEC_OGG_FLAC;
    } else if ( strncmp(buf+28, "Speex", 5) == 0 ) {
     codec = ROAR_CODEC_OGG_SPEEX;
    } else if ( len > 34 ) { // this is 7 bytes after the end of the header
     if ( strncmp(buf+28, "\001vorbis", 7) == 0 )
      codec = ROAR_CODEC_OGG_VORBIS;
    }
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
#if ROAR_SPEEX_MAGIC_LEN < ROAR_CELT_MAGIC_LEN
   }
   if ( len > ROAR_CELT_MAGIC_LEN ) {
#endif
    if ( strncmp(buf, ROAR_CELT_MAGIC_0, ROAR_CELT_MAGIC_LEN) == 0 ||
         strncmp(buf, ROAR_CELT_MAGIC_1, ROAR_CELT_MAGIC_LEN) == 0  )
     codec = ROAR_CODEC_ROAR_CELT;
   }
  } else if ( strncmp(buf, "fLaC", 4) == 0 ) {
   codec = ROAR_CODEC_FLAC;
  } else if ( strncmp(buf, ".snd", 4) == 0 ) {
   codec = ROAR_CODEC_AU;
  } else if ( len > 7 && strncmp(buf, "RAUM-CF0", 8) == 0 ) {
   codec = ROAR_CODEC_RAUM;
  }
 }

 return codec;
}

ssize_t roar_file_send_raw (int out, int in) {
#ifdef _CAN_OPERATE
 ssize_t r = 0;
#ifdef ROAR_HAVE_LINUX_SENDFILE
 ssize_t ret;
#endif
 int len;
 char buf[BUFSIZE];
#if defined(__linux__) && defined(ROAR_HAVE_IPV4)
 int cork_new = 1, cork_old;
 socklen_t cork_len = sizeof(int);

 if ( getsockopt(out, IPPROTO_TCP, TCP_CORK, &cork_old, &cork_len) == -1 ) {
  cork_old = -1;
 } else {
  setsockopt(out, IPPROTO_TCP, TCP_CORK, &cork_new, sizeof(int));
 }
#endif

 roar_debug_warn_obsolete("roar_file_send_raw", "roar_vio_copy_data", NULL);

#ifdef ROAR_HAVE_LINUX_SENDFILE
 while ((ret = sendfile(out, in, NULL, BUFMAX)) > 0)
  r += ret;
#endif

 // TODO: try mmap here!

 while ((len = read(in, buf, BUFSIZE)) > 0)
  r += write(out, buf, len);

#if defined(__linux__) && defined(ROAR_HAVE_IPV4)
 if ( cork_old != -1 )
  setsockopt(out, IPPROTO_TCP, TCP_CORK, &cork_old, cork_len);
#endif
 return r;
#else
 return -1;
#endif
}

ssize_t     roar_file_map        (char * filename, int flags, mode_t mode, size_t len, void ** mem) {
#ifdef ROAR_HAVE_MMAP
 int fh;
 int mmap_flags = 0;
 struct stat stat;

 if ( mem == NULL || filename == NULL )
  return -1;

 *mem = NULL;

 if ( flags & O_RDWR ) {
  mmap_flags = PROT_READ|PROT_WRITE;
 } else if ( flags & O_WRONLY ) {
  mmap_flags = PROT_WRITE;
 } else {
  mmap_flags = PROT_READ;
 }

 if ( (fh = open(filename, flags, mode)) == -1 ) {
  return -1;
 }

 if ( fstat(fh, &stat) == -1 ) {
  close(fh);
  return -1;
 }

 if ( stat.st_size < len ) {
  if ( ftruncate(fh, len) == -1 ) {
   close(fh);
   return -1;
  }
 }

 if ( (*mem = mmap(NULL, len, mmap_flags, MAP_SHARED, fh, 0)) == NULL ) {
  close(fh);
  return -1;
 }

 close(fh);

 return len;
#else
#ifdef ROAR_TARGET_WIN32
 ROAR_ERR("roar_file_map(*): There is no support to fast access files via mmap() within win32, download a real OS...");
#endif
 return -1;
#endif
}

int     roar_file_unmap      (size_t len, void * mem) {
#ifdef ROAR_HAVE_MMAP
 return munmap(mem, len);
#else
#ifdef ROAR_TARGET_WIN32
 ROAR_ERR("roar_file_map(*): There is no support to fast access files via mmap() within win32, download a real OS...");
#endif
 return -1;
#endif
}


ssize_t roar_file_play (struct roar_connection * con, char * file, int exec) {
 return roar_file_play_full(con, file, exec, 0, NULL);
}

ssize_t roar_file_play_full  (struct roar_connection * con, char * file, int exec, int passfh, struct roar_stream * s) {
#ifdef _CAN_OPERATE
 int codec = -1;
 int in, out = -1;
 ssize_t r = 0;
 int seek;
 int len;
 char buf[BUFSIZE];
 int rate = ROAR_RATE_DEFAULT, channels = ROAR_CHANNELS_DEFAULT, bits = ROAR_BITS_DEFAULT;
 struct roar_stream localstream[1];

 // FIXME: check error cases

 ROAR_DBG("roar_file_play_full(*) = ?");

 if ( !s )
  s = localstream;

 if ( !con )
  return -1;

 if ( !file )
  return -1;

 if ( exec && passfh )
  return -1;

#ifdef ROAR_TARGET_WIN32
 if ( (in = open(file, O_RDONLY|O_BINARY, 0644)) == -1 ) {
#else
 if ( (in = open(file, O_RDONLY, 0644)) == -1 ) {
#endif
  return -1;
 }

 if ((len = read(in, buf, BUFSIZE)) < 1) {
  close(in);
  return -1;
 }

 codec = roar_file_codecdetect(buf, len);

 ROAR_DBG("roar_file_play_full(*): codec=%i(%s)", codec, roar_codec2str(codec));

 seek = lseek(in, 0, SEEK_SET) == (off_t) -1 ? 0 : 1;

 if ( codec == -1 ) {
  ROAR_ERR("roar_file_play_full(*): Unknown codec of file: %s", file);
  close(in);
  return -1;
 }

 if ( passfh && !seek ) {
  ROAR_WARN("roar_file_play_full(*): passfh on non seekable file: this may produce incorrect playback");
 }

 if ( exec ) {
  if ( roar_stream_new(s, rate, channels, bits, codec) == -1 ) {
   ROAR_ERR("roar_file_play_full(*): Can not create new stream. This is realy BAD!");
   close(in);
   return -1;
  }

  if ( roar_stream_connect(con, s, ROAR_DIR_PLAY) == -1 ) {
   ROAR_ERR("roar_file_play_full(*): Can not connect new stream to server.");
   close(in);
   return -1;
  }

  if ( roar_stream_exec(con, s) == -1 ) {
   ROAR_ERR("roar_file_play_full(*): Can not exec new stream on server.");
   close(in);
   return -1;
  }

  roar_libroar_nowarn();
  if ( (out = roar_get_connection_fh(con)) == -1 ) {
   ROAR_ERR("roar_file_play_full(*): Can not get socket of server connection for exec data transmition.");
   close(in);
   roar_libroar_warn();
   return -1;
  }
  roar_libroar_warn();

  ROAR_SHUTDOWN(out, SHUT_RD);
 } else {
  if ( !(passfh && seek) ) {
   if ( (out = roar_simple_new_stream_obj(con, s, rate, channels, bits, codec, ROAR_DIR_PLAY)) == -1 ) {
    close(in);
    return -1;
   }
  } else {
   if ( roar_stream_new(s, rate, channels, bits, codec) == -1 ) {
    close(in);
    return -1;
   }

   if ( roar_stream_connect(con, s, ROAR_DIR_PLAY) == -1 ) {
    close(in);
    return -1;
   }
  }
 }

 if ( !seek )
  ROAR_NETWORK_WRITE(out, buf, len);

 if ( !passfh ) {
#ifndef ROAR_TARGET_WIN32
  r = roar_file_send_raw(out, in);

  close(out);
#else
 while ((len = read(in, buf, BUFSIZE)) > 0)
  if ( send(out, buf, len, 0) != len )
   break;

 closesocket(out);
#endif

  if ( exec ) {
   // TODO: FIXME: this ma cause a memory leak in future
   // OLD: con->fh = -1;
   roar_connect_fh(con, -2);
  }

  close(in);
 } else {
  if ( roar_stream_passfh(con, s, in) == -1 ) {
   return -1;
  }
  close(out);
  close(in);
  return 0;
 }

 return r;
#else
 return -1;
#endif
}

char  * roar_cdromdevice     (void) {
 char * k;

 if ( (k = getenv("CDDA_DEVICE")) != NULL )
  return k;

#ifdef ROAR_DEFAULT_CDROM
 return ROAR_DEFAULT_CDROM;
#endif

 return NULL;
}

//ll
