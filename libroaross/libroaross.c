//libroaross.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

#include "roaraudio.h"

#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
#if defined(__OpenBSD__) || defined(__NetBSD__)
#include <soundcard.h>
#else
#include <sys/soundcard.h>
#endif
#include <sys/ioctl.h>

#ifdef ROAR_HAVE_H_SYS_TYPES
#include <sys/types.h>
#endif

#ifdef ROAR_HAVE_H_FCNTL
#include <fcntl.h>
#endif

#ifdef ROAR_HAVE_H_UNISTD
#include <unistd.h>
#endif

#include <sys/stat.h>
#include <dlfcn.h>

#if defined(RTLD_NEXT)
#define REAL_LIBC RTLD_NEXT
#else
#define REAL_LIBC ((void *) -1L)
#endif

#define _MAX_POINTER  8

// handle type:
#define HT_NONE       0
#define HT_STREAM     1
#define HT_MIXER      2

struct session {
 int refc;
 struct roar_connection con;
};

static struct session _session = {.refc = 0};

struct handle {
 int refc; // refrence counter
 struct session * session;
 int type;
 struct roar_stream    stream;
 struct roar_vio_calls stream_vio;
 int                   stream_dir;
 int                   stream_opened;
};

static struct {
 int     (*open)(const char *pathname, int flags, mode_t mode);
 int     (*close)(int fd);
 ssize_t (*write)(int fd, const void *buf, size_t count);
 ssize_t (*read)(int fd, void *buf, size_t count);
 int     (*ioctl)(int d, int request, ...);
} _os;

static struct {
 struct {
  int volume;
  int pcm;
  int line;
  int line1;
  int line2;
  int line3;
  int digital1;
  int digital2;
  int digital3;
 } sid;
} _mix_settings = {
                   .sid = {
                           .volume   = -1,
                           .pcm      = -1,
                           .line     =  0,
                           .line1    =  1,
                           .line2    =  2,
                           .line3    =  3,
                           .digital1 =  1,
                           .digital2 =  2,
                           .digital3 =  3
                          }
                  };

static struct pointer {
 int fh;
 struct handle * handle;
} _ptr[_MAX_POINTER];

static void _init_os (void) {
 memset(&_os, 0, sizeof(_os));

 _os.open  = dlsym(REAL_LIBC, "open");
 _os.close = dlsym(REAL_LIBC, "close");
 _os.write = dlsym(REAL_LIBC, "write");
 _os.read  = dlsym(REAL_LIBC, "read");
 _os.ioctl = dlsym(REAL_LIBC, "ioctl");
}

static void _init_ptr (void) {
 int i;

 for (i = 0; i < _MAX_POINTER; i++) {
  _ptr[i].fh = -1;
 }
}

static void _init (void) {
 static int inited = 0;

 if ( !inited ) {
  _init_os();
  _init_ptr();
  inited++;
 }
}

static int _open_dummy (void) {
 int p[2];

 if ( pipe(p) == -1 )
  return -1;

 close(p[1]);

 return p[0];
}

static struct session * _open_session (char * server, char * name) {
 if ( _session.refc == 0 ) {

  if ( name == NULL )
   name = "libroaross client";

  if ( roar_simple_connect(&(_session.con), server, name) == -1 )
   return NULL;
 }

 _session.refc++;
 return &_session;
}

static void _close_session(struct session * session) {
 if ( session == NULL )
  return;

 session->refc--;

 ROAR_DBG("_close_session(session=%p): session->refc=%i", session, session->refc);

 if ( session->refc == 0 ) {
  roar_disconnect(&(session->con));
 }
}

static struct handle * _open_handle(struct session * session) {
 struct handle * handle;

 if ( (handle = roar_mm_malloc(sizeof(struct handle))) == NULL )
  return NULL;

 memset(handle, 0, sizeof(struct handle));

 handle->refc = 1;
 handle->session = session;
 session->refc++; // TODO: better warp this
 handle->type = HT_NONE;
 handle->stream_dir = ROAR_DIR_PLAY;
 roar_stream_new(&(handle->stream), ROAR_RATE_DEFAULT, ROAR_CHANNELS_DEFAULT, ROAR_BITS_DEFAULT, ROAR_CODEC_DEFAULT);

 return handle;
}

static void _close_handle(struct handle * handle) {
 if (handle == NULL)
  return;

 handle->refc--;

 ROAR_DBG("_close_handle(handle=%p): handle->refc=%i", handle, handle->refc);

 if ( handle->refc == 0 ) {
  if ( handle->stream_opened )
   roar_vio_close(&(handle->stream_vio));

  handle->session->refc--;

  _close_session(handle->session);

  roar_mm_free(handle);
 }
}

static struct pointer * _get_pointer_by_fh (int fh) {
 int i;

 for (i = 0; i < _MAX_POINTER; i++) {
  if ( _ptr[i].fh == fh )
   return &(_ptr[i]);
 }

 return NULL;
}

static struct pointer * _open_pointer(struct handle * handle) {
 struct pointer * ret = _get_pointer_by_fh(-1);

 if ( ret == NULL )
  return NULL;

 if ( (ret->fh = _open_dummy()) == -1 )
  return NULL;

 ret->handle = handle;

 return ret;
}

static void _close_pointer(struct pointer * pointer) {
 if ( pointer == NULL )
  return;

 _os.close(pointer->fh);

 pointer->fh = -1;

 _close_handle(pointer->handle);
}

// -------------------------------------
// central open function:
// -------------------------------------

static int _open_file (const char *pathname, int flags) {
 struct session * session;
 struct handle  * handle;
 struct pointer * pointer;
 struct {
  char * prefix;
  int type;
 } * ptr = NULL, p[] = {
  {"/dev/dsp",   HT_STREAM},
  {"/dev/mixer", HT_MIXER},
  {NULL, HT_NONE},
 };
 int i;

 for (i = 0; p[i].prefix != NULL; i++) {
  if ( !strcmp(pathname, p[i].prefix) ) {
   ptr = &(p[i]);
  }
 }

 if ( ptr == NULL )
  return -2;

 if ( (session = _open_session(NULL, NULL)) == NULL ) {
  return -1;
 }

 if ( (handle = _open_handle(session)) == NULL ) {
  _close_session(session);
  return -1;
 }

 handle->type = ptr->type;

 switch (flags & (O_RDONLY|O_WRONLY|O_RDWR)) {
  case O_RDONLY:
    handle->stream_dir = ROAR_DIR_MONITOR;
   break;
  case O_WRONLY:
    handle->stream_dir = ROAR_DIR_PLAY;
   break;
  case O_RDWR:
    handle->stream_dir = ROAR_DIR_BIDIR;
   break;
 }

 if ( (pointer = _open_pointer(handle)) == NULL ) {
  _close_handle(handle);
  return -1;
 }

 return pointer->fh;
}

// -------------------------------------
// open function for streams:
// -------------------------------------

static int _open_stream (struct handle * handle) {
  // FIXME: this should be re-written much more cleanly:

 if ( handle == NULL )
  return -1;

 if ( roar_vio_simple_new_stream_obj(&(handle->stream_vio),
                                     &(handle->session->con), &(handle->stream),
                                     handle->stream.info.rate,
                                     handle->stream.info.channels,
                                     handle->stream.info.bits,
                                     handle->stream.info.codec,
                                     handle->stream_dir
                                    ) == -1 )
  return -1;

 handle->stream_opened++;

 _mix_settings.sid.pcm = roar_stream_get_id(&(handle->stream));

 return 0;
}

// -------------------------------------
// function to parse format:
// -------------------------------------

static int _ioctl_stream_format (struct handle * handle, int format) {
 struct roar_audio_info * info = &(handle->stream.info);

 switch (format) {
  case AFMT_S8:
    info->bits  = 8;
    info->codec = ROAR_CODEC_PCM_S_LE;
   break;
  case AFMT_U8:
    info->bits  = 8;
    info->codec = ROAR_CODEC_PCM_U_LE;
   break;
  case AFMT_S16_BE:
    info->bits  = 16;
    info->codec = ROAR_CODEC_PCM_S_BE;
   break;
  case AFMT_S16_LE:
    info->bits  = 16;
    info->codec = ROAR_CODEC_PCM_S_LE;
   break;
  case AFMT_U16_BE:
    info->bits  = 16;
    info->codec = ROAR_CODEC_PCM_U_BE;
   break;
  case AFMT_U16_LE:
    info->bits  = 16;
    info->codec = ROAR_CODEC_PCM_U_LE;
   break;
#ifdef AFMT_S32_BE
  case AFMT_S32_BE:
    info->bits  = 32;
    info->codec = ROAR_CODEC_PCM_S_BE;
   break;
#endif
#ifdef AFMT_S32_LE
  case AFMT_S32_LE:
    info->bits  = 32;
    info->codec = ROAR_CODEC_PCM_S_LE;
   break;
#endif
  case AFMT_A_LAW:
    info->bits  = 8;
    info->codec = ROAR_CODEC_ALAW;
   break;
  case AFMT_MU_LAW:
    info->bits  = 8;
    info->codec = ROAR_CODEC_MULAW;
   break;
#ifdef AFMT_VORBIS
  case AFMT_VORBIS:
    info->codec = ROAR_CODEC_OGG_VORBIS;
   break;
#endif
  default:
    errno = ENOSYS;
    return -1;
   break;
 }

 return 0;
}

static inline int _ioctl_stream_format_list (void) {
 int format = 0;

 format |= AFMT_S8;
 format |= AFMT_U8;

 format |= AFMT_S16_BE;
 format |= AFMT_S16_LE;

 format |= AFMT_U16_BE;
 format |= AFMT_U16_LE;

#ifdef AFMT_S32_BE
 format |= AFMT_S32_BE;
#endif
#ifdef AFMT_S32_LE
 format |= AFMT_S32_LE;
#endif

 format |= AFMT_A_LAW;
 format |= AFMT_MU_LAW;

#ifdef AFMT_VORBIS
 format |= AFMT_VORBIS;
#endif

 return format;
}

// -------------------------------------
// mixer ioctls:
// -------------------------------------

static int _ioctl_mixer (struct handle * handle, long unsigned int req, int * ip) {
 int channels;
 struct roar_mixer_settings mixer;
 char * name = NULL;
 int o_w    = 0;
 int o_sid  = -1;

 switch (req) {
#if 0
  case SNDCTL_MIX_DESCRIPTION: name = "SNDCTL_MIX_DESCRIPTION"; break;
  case SNDCTL_MIX_ENUMINFO:    name = "SNDCTL_MIX_ENUMINFO";    break;
  case SNDCTL_MIX_EXTINFO:     name = "SNDCTL_MIX_EXTINFO";     break;
  case SNDCTL_MIX_NREXT:       name = "SNDCTL_MIX_NREXT";       break;
  case SNDCTL_MIX_NRMIX:       name = "SNDCTL_MIX_NRMIX";       break;
  case SNDCTL_MIX_READ:        name = "SNDCTL_MIX_READ";        break;
  case SNDCTL_MIX_WRITE:       name = "SNDCTL_MIX_WRITE";       break;
#endif
  case SOUND_MIXER_READ_DEVMASK: name = "SOUND_MIXER_READ_DEVMASK"; break;
  case SOUND_MIXER_READ_PCM:     name = "SOUND_MIXER_READ_PCM";     break;
  case SOUND_MIXER_WRITE_PCM:    name = "SOUND_MIXER_WRITE_PCM";    break;
 }
 if ( name != NULL ) {
  ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p): unspported mixer command %s", handle, req, ip, name);
  ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p) = -1 // errno = ENOSYS", handle, req, ip);
  errno = ENOSYS;
  return -1;
 }

 switch (req) {
  case SOUND_MIXER_READ_VOLUME:    o_w = 0; o_sid = _mix_settings.sid.volume;   break;
  case SOUND_MIXER_READ_PCM:       o_w = 0; o_sid = _mix_settings.sid.pcm;      break;
  case SOUND_MIXER_READ_LINE:      o_w = 0; o_sid = _mix_settings.sid.line;     break;
  case SOUND_MIXER_READ_LINE1:     o_w = 0; o_sid = _mix_settings.sid.line1;    break;
  case SOUND_MIXER_READ_LINE2:     o_w = 0; o_sid = _mix_settings.sid.line2;    break;
  case SOUND_MIXER_READ_LINE3:     o_w = 0; o_sid = _mix_settings.sid.line3;    break;
#if 0
  case SOUND_MIXER_READ_DIGITAL1:  o_w = 0; o_sid = _mix_settings.sid.digital1; break;
  case SOUND_MIXER_READ_DIGITAL2:  o_w = 0; o_sid = _mix_settings.sid.digital2; break;
  case SOUND_MIXER_READ_DIGITAL3:  o_w = 0; o_sid = _mix_settings.sid.digital3; break;
#endif
  case SOUND_MIXER_WRITE_VOLUME:   o_w = 1; o_sid = _mix_settings.sid.volume;   break;
  case SOUND_MIXER_WRITE_PCM:      o_w = 1; o_sid = _mix_settings.sid.pcm;      break;
  case SOUND_MIXER_WRITE_LINE:     o_w = 1; o_sid = _mix_settings.sid.line;     break;
  case SOUND_MIXER_WRITE_LINE1:    o_w = 1; o_sid = _mix_settings.sid.line1;    break;
  case SOUND_MIXER_WRITE_LINE2:    o_w = 1; o_sid = _mix_settings.sid.line2;    break;
  case SOUND_MIXER_WRITE_LINE3:    o_w = 1; o_sid = _mix_settings.sid.line3;    break;
#if 0
  case SOUND_MIXER_WRITE_DIGITAL1: o_w = 1; o_sid = _mix_settings.sid.digital1; break;
  case SOUND_MIXER_WRITE_DIGITAL2: o_w = 1; o_sid = _mix_settings.sid.digital2; break;
  case SOUND_MIXER_WRITE_DIGITAL3: o_w = 1; o_sid = _mix_settings.sid.digital3; break;
#endif
 }
 if ( o_sid != -1 ) {
  // set/get volume
  if ( o_w ) {
   mixer.scale    = 65535;
   mixer.mixer[0] = ( *ip       & 0xFF)*65535/50;
   mixer.mixer[1] = ((*ip >> 8) & 0xFF)*65535/50;
   if ( roar_set_vol(&(handle->session->con), o_sid, &mixer, 2) == -1 ) {
    errno = ENOSYS;
    return -1;
   }
   return 0;
  } else {
   if ( roar_get_vol(&(handle->session->con), o_sid, &mixer, &channels) == -1 ) {
    errno = ENOSYS;
    return -1;
   }
   *ip = ((50*mixer.mixer[0])/mixer.scale) | (((50*mixer.mixer[0])/mixer.scale)<<8);
   return 0;
  }
 }

 switch (req) {
  case SOUND_MIXER_READ_DEVMASK:
    *ip = 0;

    if ( _mix_settings.sid.volume != -1 )
     *ip |= SOUND_MASK_VOLUME;
    if ( _mix_settings.sid.pcm != -1 )
     *ip |= SOUND_MASK_PCM;
    if ( _mix_settings.sid.line != -1 )
     *ip |= SOUND_MASK_LINE;
    if ( _mix_settings.sid.line1 != -1 )
     *ip |= SOUND_MASK_LINE1;
    if ( _mix_settings.sid.line2 != -1 )
     *ip |= SOUND_MASK_LINE2;
    if ( _mix_settings.sid.line3 != -1 )
     *ip |= SOUND_MASK_LINE3;
    if ( _mix_settings.sid.digital1 != -1 )
     *ip |= SOUND_MASK_DIGITAL1;
    if ( _mix_settings.sid.digital2 != -1 )
     *ip |= SOUND_MASK_DIGITAL2;
    if ( _mix_settings.sid.digital3 != -1 )
     *ip |= SOUND_MASK_DIGITAL3;

    return 0;
   break;
 }

 ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p): unknown mixer CTL", handle, req, ip);
 ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p) = -1 // errno = ENOSYS", handle, req, ip);
 errno = ENOSYS;
 return -1;
}

// -------------------------------------
// emulated functions follow:
// -------------------------------------

int     open(const char *pathname, int flags, ...) {
 int     ret;
 mode_t  mode = 0;
 va_list args;

 _init();

 ret = _open_file(pathname, flags);

 switch (ret) {
  case -2:       // continue as normal, use _op.open()
   break;
  case -1:       // pass error to caller
    return -1;
   break;
  default:       // return successfully opened pointer to caller
    return ret;
   break;
 }

 if (flags & O_CREAT) {
  va_start(args, flags);
  mode = va_arg(args, mode_t);
  va_end(args);
 }

 return _os.open(pathname, flags, mode);
}

int     close(int fd) {
 struct pointer * pointer;
 _init();

 if ( (pointer = _get_pointer_by_fh(fd)) != NULL ) {
  _close_pointer(pointer);
  return 0;
 }

 return _os.close(fd);
}

ssize_t write(int fd, const void *buf, size_t count) {
 struct pointer * pointer;

 _init();

 if ( (pointer = _get_pointer_by_fh(fd)) != NULL ) {
  if ( pointer->handle->type == HT_STREAM ) {
   if ( pointer->handle->stream_opened == 0 ) {
    if ( _open_stream(pointer->handle) == -1 ) {
     errno = EIO;
     return -1;
    }
   }
   return roar_vio_write(&(pointer->handle->stream_vio), (char*)buf, count);
  } else {
   errno = EINVAL;
   return -1;
  }
 }

 return _os.write(fd, buf, count);
}

ssize_t read(int fd, void *buf, size_t count) {
 struct pointer * pointer;

 _init();

 if ( (pointer = _get_pointer_by_fh(fd)) != NULL ) {
  if ( pointer->handle->type == HT_STREAM ) {
   if ( pointer->handle->stream_opened == 0 ) {
    if ( _open_stream(pointer->handle) == -1 ) {
     errno = EIO;
     return -1;
    }
   }
   return roar_vio_read(&(pointer->handle->stream_vio), buf, count);
  } else {
   errno = EINVAL;
   return -1;
  }
 }

 return _os.read(fd, buf, count);
}

extern int ioctl (int __fd, unsigned long int __request, ...) {
 struct pointer * pointer;
 struct handle  * handle;
 va_list args;
 void *argp;
 int * ip = NULL;

 _init();

// ROAR_DBG("ioctl(__fd=%i, __request=%lu) = ?", __fd, (long unsigned int) __request);

 va_start (args, __request);
 argp = va_arg (args, void *);
 va_end (args);

// ROAR_DBG("ioctl(__fd=%i, __request=%lu): argp=%p", __fd, (long unsigned int) __request, argp);

 if ( (pointer = _get_pointer_by_fh(__fd)) != NULL ) {
  ip = argp;
//  ROAR_DBG("ioctl(__fd=%i, __request=%lu): ip=%p", __fd, (long unsigned int) __request, ip);
  switch ((handle = pointer->handle)->type) {
   case HT_STREAM:
     switch (__request) {
      case SNDCTL_DSP_RESET:
      case SNDCTL_DSP_POST:
       break;
      case SNDCTL_DSP_SPEED:
        handle->stream.info.rate = *ip;
        return 0;
       break;
      case SNDCTL_DSP_CHANNELS:
        handle->stream.info.channels = *ip;
        return 0;
       break;
      case SNDCTL_DSP_SETFMT:
        return _ioctl_stream_format(handle, *ip);
       break;
      case SNDCTL_DSP_GETFMTS:
//        ROAR_DBG("ioctl(__fd=%i, __request=%lu): ip=%p", __fd, (long unsigned int) __request, ip);
        *ip = _ioctl_stream_format_list();
        return 0;
       break;
      default:
        ROAR_DBG("ioctl(__fd=%i, __request=%lu) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request);
        errno = ENOSYS;
        return -1;
     }
    break;
   case HT_MIXER:
     return _ioctl_mixer(handle, __request, ip);
    break;
   default:
     ROAR_DBG("ioctl(__fd=%i, __request=%lu): unknown handle type: no ioctl()s supported", __fd, (long unsigned int) __request);
     ROAR_DBG("ioctl(__fd=%i, __request=%lu) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request);
     errno = EINVAL;
     return -1;
    break;
  }
 }

 return _os.ioctl(__fd, __request, argp);
}

#endif

//ll
