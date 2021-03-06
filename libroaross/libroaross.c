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

#include "roaraudio.h"
#include "libroarlight/libroarlight.h"

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
#include <stdarg.h>

#if defined(RTLD_NEXT)
#define REAL_LIBC RTLD_NEXT
#else
#define REAL_LIBC ((void *) -1L)
#endif

#ifndef ENOTSUP
#define ENOTSUP ENOSYS
#endif

#ifndef O_DIRECTORY
#define O_DIRECTORY 0
#endif

#ifndef O_DIRECT
#define O_DIRECT 0
#endif

#ifndef O_LARGEFILE
#define O_LARGEFILE 0
#endif

#ifndef O_NOATIME
#define O_NOATIME 0
#endif

#define _O_PARA_DIR (O_RDONLY|O_WRONLY|O_RDWR)
#define _O_PARA_IGN (O_DIRECT|O_APPEND|O_LARGEFILE|O_NOATIME|O_NOCTTY|O_TRUNC)

#if defined(ROAR_OS_NETBSD) && defined(ioctl)
#define IOCTL_IS_ALIAS
#endif

#ifdef ROAR_OS_FREEBSD
#define _VA_ARGS_MODE_T int
#else
#define _VA_ARGS_MODE_T mode_t
#endif

#ifdef ROAR_OS_FREEBSD
#define _CREAT_ARG_PATHNAME path
#else
#define _CREAT_ARG_PATHNAME pathname
#endif

#ifdef ROAR_OS_NETBSD
#define IOCTL() int _oss_ioctl __P((int fd, unsigned long com, void *argp))
#define map_args int __fd = fd; unsigned long int __request = com
#elif defined(ROAR_TARGET_CYGWIN)
#define IOCTL() int ioctl (int __fd, int __cmd, ...)
#define map_args unsigned long int __request = __cmd; void * argp
#define va_argp
#define ioctl_lastarg __cmd
#else
#define IOCTL() int ioctl (int __fd, unsigned long int __request, ...)
#define map_args void * argp
#define va_argp
#define ioctl_lastarg __request
#endif

#define OSS_VOLUME_SCALE 100

#define _MAX_POINTER  8

// handle type:
#define HT_NONE       0 /* Unused object */
#define HT_STREAM     1 /* Stream with no specal handling needed */
#define HT_MIXER      2 /* Mixer device */
#define HT_WAVEFORM   3 /* Waveform device */
#define HT_MIDI       4 /* MIDI device */
#define HT_DMX        5 /* DMX512/DMX4Linux device */
#define HT_VIO        6 /* General VIO object */
#define HT_STATIC     7 /* Static file */

struct session {
 int refc;
 struct roar_connection con;
};

static struct session _session = {.refc = 0};

struct handle {
 int refc; // refrence counter
 struct session * session;
 int type;
 int sysio_flags;
 struct roar_stream    stream;
 struct roar_vio_calls stream_vio;
 int                   stream_dir;
 int                   stream_opened;
 size_t                stream_buffersize;
 size_t                readc, writec;
 size_t                pos;
 union {
  struct {
   char * data;
   size_t len;
  } sf;
 } userdata;
};

static struct {
 int     (*open)(const char *pathname, int flags, mode_t mode);
 int     (*close)(int fd);
 ssize_t (*write)(int fd, const void *buf, size_t count);
 ssize_t (*read)(int fd, void *buf, size_t count);
#ifndef IOCTL_IS_ALIAS
 int     (*ioctl)(int d, int request, ...);
#endif
 off_t   (*lseek)(int fildes, off_t offset, int whence);
 FILE   *(*fopen)(const char *path, const char *mode);
 int     (*dup)(int oldfd);
 int     (*dup2)(int oldfd, int newfd);
 int     (*select)(int nfds, fd_set *readfds, fd_set *writefds,
                   fd_set *exceptfds, struct timeval *timeout);
 int     (*fcntl)(int fd, int cmd, ...);
 int     (*access)(const char *pathname, int mode);
 int     (*open64)(const char *__file, int __oflag, ...);
 int     (*creat)(const char *_CREAT_ARG_PATHNAME, mode_t mode);
 int     (*stat)(const char *path, struct stat *buf);
 int     (*fstat)(int filedes, struct stat *buf);
 int     (*lstat)(const char *path, struct stat *buf);
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


static char _sf__dev_sndstat[] =
 "Sound Driver:RoarAudio\n"
 "Config options: 0\n"
 "\n"
 "Installed drivers:\n"
 "Type 10: RoarAudio emulation\n"
 "\n"
 "Card config:\n"
 "\n"
 "Audio devices:\n"
 "0: RoarAudio OSS emulation (DUPLEX)\n"
 "\n"
 "Midi devices:\n"
 "0: RoarAudio OSS emulation MIDI\n"
 "\n"
 "Timers:\n"
 "\n"
 "Mixers:\n"
 "0: RoarAudio OSS emulation Mixer\n"
;

static struct devices {
  char * prefix;
  int type;
  size_t len;
  void * userdata;
  struct handle * (*open)(const char * file, int flags, mode_t mode, struct devices * ptr);
} _device_list[] = {
 {"/dev/dsp",           HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/dsp?",          HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/audio",         HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/audio?",        HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/sound/dsp",     HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/sound/dsp?",    HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/sound/audio",   HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/sound/audio?",  HT_WAVEFORM,  0, NULL, NULL},
 {"/dev/mixer",         HT_MIXER,     0, NULL, NULL},
 {"/dev/mixer?",        HT_MIXER,     0, NULL, NULL},
 {"/dev/sound/mixer",   HT_MIXER,     0, NULL, NULL},
 {"/dev/sound/mixer?",  HT_MIXER,     0, NULL, NULL},
 {"/dev/midi",          HT_MIDI,      0, NULL, NULL},
 {"/dev/midi?",         HT_MIDI,      0, NULL, NULL},
 {"/dev/rmidi",         HT_MIDI,      0, NULL, NULL},
 {"/dev/rmidi?",        HT_MIDI,      0, NULL, NULL},
 {"/dev/sound/midi",    HT_MIDI,      0, NULL, NULL},
 {"/dev/sound/midi?",   HT_MIDI,      0, NULL, NULL},
 {"/dev/sound/rmidi",   HT_MIDI,      0, NULL, NULL},
 {"/dev/sound/rmidi?",  HT_MIDI,      0, NULL, NULL},
 {"/dev/dmx",           HT_DMX,       0, NULL, NULL},
 {"/dev/dmx?",          HT_DMX,       0, NULL, NULL},
 {"/dev/misc/dmx",      HT_DMX,       0, NULL, NULL},
 {"/dev/misc/dmx?",     HT_DMX,       0, NULL, NULL},
 {"/dev/dmxin",         HT_DMX,       0, NULL, NULL},
 {"/dev/dmxin?",        HT_DMX,       0, NULL, NULL},
 {"/dev/misc/dmxin",    HT_DMX,       0, NULL, NULL},
 {"/dev/misc/dmxin?",   HT_DMX,       0, NULL, NULL},
 {"/dev/sndstat",       HT_STATIC,    sizeof(_sf__dev_sndstat)-1, _sf__dev_sndstat, NULL},
#ifdef ROAR_DEFAULT_OSS_DEV
 {ROAR_DEFAULT_OSS_DEV, HT_WAVEFORM,  0, NULL, NULL},
#endif
#ifdef ROAR_DEFAULT_OSS_MIX_DEV
 {ROAR_DEFAULT_OSS_MIX_DEV, HT_MIXER, 0, NULL, NULL},
#endif
 {NULL, HT_NONE, 0, NULL, NULL},
};


static int _update_nonblock (struct handle * handle);

static void _init_os (void) {
 memset(&_os, 0, sizeof(_os));

 // if call roar_dl_getsym() here all applications will segfaul.
 // why?

 _os.open   = dlsym(REAL_LIBC, "open");
 _os.close  = dlsym(REAL_LIBC, "close");
 _os.write  = dlsym(REAL_LIBC, "write");
 _os.read   = dlsym(REAL_LIBC, "read");
#ifndef IOCTL_IS_ALIAS
 _os.ioctl  = dlsym(REAL_LIBC, "ioctl");
#endif
 _os.lseek  = dlsym(REAL_LIBC, "lseek");
 _os.fopen  = dlsym(REAL_LIBC, "fopen");
 _os.dup    = dlsym(REAL_LIBC, "dup");
 _os.dup2   = dlsym(REAL_LIBC, "dup2");
 _os.select = dlsym(REAL_LIBC, "select");
 _os.fcntl  = dlsym(REAL_LIBC, "fcntl");
 _os.access = dlsym(REAL_LIBC, "access");
 _os.open64 = dlsym(REAL_LIBC, "open64");
 _os.creat  = dlsym(REAL_LIBC, "creat");
 _os.stat   = dlsym(REAL_LIBC, "stat");
 _os.fstat  = dlsym(REAL_LIBC, "fstat");
 _os.lstat  = dlsym(REAL_LIBC, "lstat");
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
  roar_vio_select(NULL, 0, NULL, NULL);
  inited++;
 }
}

static void _find_volume_sid (struct session * session) {
 int i;
 int num;
 int id[ROAR_STREAMS_MAX];
 struct roar_stream s;
 char name[1024];

 ROAR_DBG("_find_volume_sid(session=%p) = ?", session);

 if ( (num = roar_list_streams(&(session->con), id, ROAR_STREAMS_MAX)) == -1 ) {
  return;
 }

 for (i = 0; i < num; i++) {
  if ( roar_get_stream(&(session->con), &s, id[i]) == -1 )
   continue;

  if ( s.dir != ROAR_DIR_MIXING )
   continue;

  if ( roar_stream_get_name(&(session->con), &s, name, 1024) == -1 )
   continue;

  if ( !strcasecmp(name, "Waveform Mixer") ) {
   _mix_settings.sid.volume = id[i];
   ROAR_DBG("_find_volume_sid(session=%p): found waveform mixer at sid %i", session, id[i]);
   ROAR_DBG("_find_volume_sid(session=%p) = (void)", session);
   return;
  }
 }
}

static int _open_dummy (void) {
 int p[2];

 ROAR_DBG("_open_dummy(void) = ?");

 if ( pipe(p) == -1 )
  return -1;

 close(p[1]);

 ROAR_DBG("_open_dummy(void) = %i", p[0]);
 return p[0];
}

static struct session * _open_session (char * server, char * name) {
 struct session * ses = &_session;
 int new_session = getenv("ROAR_OSS_NEW_SESSION") == NULL ? 0 : 1;

 ROAR_DBG("_open_session(server='%s', name='%s') = ?", server, name);
 ROAR_DBG("_open_session(server='%s', name='%s'): _session.refc=%i", server, name, _session.refc);

 if ( new_session ) {
  ses = roar_mm_malloc(sizeof(struct session));
  if ( ses == NULL )
   return NULL;

  memset(ses, 0, sizeof(struct session));
 }

 if ( ses->refc == 0 ) {

  if ( name == NULL )
   name = getenv("ROAR_OSS_CLIENT_NAME");

  if ( name == NULL )
   name = "libroaross client";

  if ( roar_simple_connect(&(ses->con), server, name) == -1 ) {
   if ( new_session )
    roar_mm_free(ses);

   return NULL;
  }

  _find_volume_sid(ses);

  if ( !new_session ) {
   if ( getenv("ROAR_OSS_KEEP_SESSION") != NULL )
    ses->refc++;
  }
 }

 ses->refc++;

 ROAR_DBG("_open_session(server='%s', name='%s') = %p", server, name, ses);
 return ses;
}

static void _close_session(struct session * session) {
 if ( session == NULL )
  return;

 session->refc--;

 ROAR_DBG("_close_session(session=%p): session->refc=%i", session, session->refc);

 if ( session->refc == 0 ) {
  roar_disconnect(&(session->con));
 }

 if ( session != &_session )
  roar_mm_free(session);
}

static struct handle * _open_handle(struct session * session) {
 struct handle * handle;

 ROAR_DBG("_open_handle(session=%p) = ?", session);

 if ( (handle = roar_mm_malloc(sizeof(struct handle))) == NULL )
  return NULL;

 memset(handle, 0, sizeof(struct handle));

 handle->refc = 1;
 handle->session = session;

 if ( session != NULL )
  session->refc++; // TODO: better warp this

 handle->type = HT_NONE;
 handle->stream_dir = ROAR_DIR_PLAY;
 roar_stream_new(&(handle->stream), ROAR_RATE_DEFAULT, ROAR_CHANNELS_DEFAULT, ROAR_BITS_DEFAULT, ROAR_CODEC_DEFAULT);

 ROAR_DBG("_open_handle(session=%p) = %p", session, handle);
 return handle;
}

static void _close_handle(struct handle * handle) {
 int need_close = 0;

 if (handle == NULL)
  return;

 handle->refc--;

 ROAR_DBG("_close_handle(handle=%p): handle->refc=%i", handle, handle->refc);

 if ( handle->refc == 0 ) {
  switch (handle->type) {
   case HT_VIO:
     need_close = 1;
    break;
   case HT_STREAM:
     if ( handle->stream_opened )
      need_close = 1;
    break;
  }

  if ( need_close )
   roar_vio_close(&(handle->stream_vio));

  if ( handle->session != NULL ) {
   handle->session->refc--;

   _close_session(handle->session);
  }

  roar_mm_free(handle);
 }
}

static struct pointer * _get_pointer_by_fh_or_new (int fh) {
 int i;

 for (i = 0; i < _MAX_POINTER; i++) {
  if ( _ptr[i].fh == fh )
   return &(_ptr[i]);
 }

 return NULL;
}

static struct pointer * _get_pointer_by_fh (int fh) {
 if ( fh == -1 )
  return NULL;

 return _get_pointer_by_fh_or_new(fh);
}

static struct pointer * _open_pointer(struct handle * handle) {
 struct pointer * ret = _get_pointer_by_fh_or_new(-1);

 ROAR_DBG("_open_pointer(handle=%p) = ?", handle);

 if ( ret == NULL ) {
  ROAR_DBG("_open_pointer(handle=%p) = NULL", handle);
  return NULL;
 }

 if ( (ret->fh = _open_dummy()) == -1 ) {
  ROAR_DBG("_open_pointer(handle=%p) = NULL", handle);
  return NULL;
 }

 ret->handle = handle;

 ROAR_DBG("_open_pointer(handle=%p) = %p", handle, ret);

 return ret;
}

static struct pointer * _attach_pointer(struct handle * handle, int fh) {
 struct pointer * ret = _get_pointer_by_fh_or_new(-1);

 if ( ret == NULL )
  return NULL;

 if ( (ret->fh = fh) == -1 )
  return NULL;

 ret->handle = handle;

 handle->refc++;

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
// central function to find device:
// -------------------------------------

static struct devices * _get_device (const char * pathname) {
 size_t len, pathlen;
 int i;
 int qm_match;

 ROAR_DBG("_get_device(pathname='%s') = ?", pathname);

 pathlen = strlen(pathname);

 for (i = 0; _device_list[i].prefix != NULL; i++) {
  len = strlen(_device_list[i].prefix);

  qm_match = 0;

  if ( _device_list[i].prefix[len-1] == '*' ) {
   len--;
  } else if ( _device_list[i].prefix[len-1] == '?' ) {
   qm_match = 1;
   len--;
  } else {
   len++;
  }

  if ( !strncmp(pathname, _device_list[i].prefix, len) ) {
   if ( !qm_match || pathlen == (len + 1) )
    return &(_device_list[i]);
  }
 }

 ROAR_DBG("_get_device(pathname='%s') = NULL", pathname);
 return NULL;
}

// -------------------------------------
// central open function:
// -------------------------------------

static int _open_file (const char *pathname, int flags) {
 struct session * session;
 struct handle  * handle;
 struct pointer * pointer;
 struct devices * ptr = NULL;

 ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = ?", pathname, flags);

/*
 * Flags we ignore:
 * O_DIRECT, O_APPEND, O_LARGEFILE, O_NOATIME, O_NOCTTY, O_TRUNC
 */

 if ( (ptr = _get_device(pathname)) == NULL ) {
  ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -2", pathname, flags);
  return -2;
 }

 ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = ?", pathname, flags);


#ifdef O_ASYNC
 if ( flags & O_ASYNC ) {
  ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1 // not supported O_ASYNC", pathname, flags);
  errno = ENOSYS;
  return -1;
 }
#endif

 if ( flags & O_DIRECTORY ) {
  ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1 // invalid flags (O_DIRECTORY)", pathname, flags);
  errno = EINVAL;
  return -1;
 }

 if ( flags & O_EXCL ) {
  ROAR_WARN("_open_file(pathname='%s', flags=0x%x): This application is asked us for exclusive device access.", pathname, flags);
  ROAR_WARN("_open_file(pathname='%s', flags=0x%x): This is maybe not what you want.", pathname, flags);
  ROAR_WARN("_open_file(pathname='%s', flags=0x%x): We reject this according to OSS specs.", pathname, flags);
  ROAR_WARN("_open_file(pathname='%s', flags=0x%x): There should be a option in the application to switch this off.", pathname, flags);
  ROAR_WARN("_open_file(pathname='%s', flags=0x%x) = -1 // invalid flags (O_EXCL)", pathname, flags);
  errno = EINVAL;
  return -1;
 }

 ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = ?", pathname, flags);

 if ( ptr->type == HT_STATIC || ptr->type == HT_VIO ) { // non-session handles
  session = NULL;
 } else {
  if ( (session = _open_session(NULL, NULL)) == NULL ) {
   ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
   return -1;
  }
 }

 if ( ptr->open != NULL ) {
  // TODO: Add support to pass mode (perms) to open.
  if ( (handle = ptr->open(pathname, flags, 0000, ptr)) == NULL ) {
   _close_session(session);
   ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
   return -1;
  }
 } else {
  if ( (handle = _open_handle(session)) == NULL ) {
   _close_session(session);
   ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
   return -1;
  }

  handle->type        = ptr->type;
  handle->sysio_flags = flags;
  handle->stream_dir  = -1;
 }

 switch (flags & _O_PARA_DIR) {
  case O_RDONLY:
    switch (ptr->type) {
     case HT_WAVEFORM:
       handle->stream_dir = ROAR_DIR_MONITOR;
      break;
     case HT_MIDI:
       handle->stream_dir = ROAR_DIR_MIDI_OUT;
      break;
     case HT_DMX:
       handle->stream_dir = ROAR_DIR_LIGHT_OUT;
      break;
     case HT_MIXER:
     case HT_STATIC:
      break;
     default:
       ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
       return -1;
    }
   break;
  case O_WRONLY:
    switch (ptr->type) {
     case HT_WAVEFORM:
       handle->stream_dir = ROAR_DIR_PLAY;
      break;
     case HT_MIDI:
       handle->stream_dir = ROAR_DIR_MIDI_IN;
      break;
     case HT_DMX:
       handle->stream_dir = ROAR_DIR_LIGHT_IN;
      break;
     case HT_MIXER:
     case HT_STATIC:
      break;
     default:
       ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
       return -1;
    }
   break;
  case O_RDWR:
    switch (ptr->type) {
     case HT_WAVEFORM:
       handle->stream_dir = ROAR_DIR_BIDIR;
      break;
     case HT_MIXER:
     case HT_STATIC:
      break;
     default:
       ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
       return -1;
    }
   break;
 }

 switch (handle->type) {
  case HT_WAVEFORM:
    handle->type = HT_STREAM;
   break;
  case HT_MIDI:
    handle->type = HT_STREAM;
    handle->stream.info.rate     = 0;
    handle->stream.info.bits     = ROAR_MIDI_BITS;
    handle->stream.info.channels = ROAR_MIDI_CHANNELS_DEFAULT;
    handle->stream.info.codec    = ROAR_CODEC_MIDI;
   break;
  case HT_DMX:
    handle->stream.info.rate     = 0;
    handle->stream.info.bits     = ROAR_LIGHT_BITS;
    handle->stream.info.channels = 512;
    handle->stream.info.codec    = ROAR_CODEC_ROARDMX;
   break;
  case HT_STATIC:
    handle->userdata.sf.len      = ptr->len;
    handle->userdata.sf.data     = ptr->userdata;
   break;
 }

 ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = ?", pathname, flags);

 if ( (pointer = _open_pointer(handle)) == NULL ) {
  _close_handle(handle);
  ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = -1", pathname, flags);
  return -1;
 }

 ROAR_DBG("_open_file(pathname='%s', flags=0x%x) = %i", pathname, flags, pointer->fh);

 return pointer->fh;
}

// -------------------------------------
// VIO open function:
// -------------------------------------

int libroaross_open_vio(struct handle ** handleret, struct roar_vio_calls ** vio, int flags) {
 struct handle  * handle;
 struct pointer * pointer;

 _init();

 if ( vio == NULL )
  return -1;

 if ( (handle = _open_handle(NULL)) == NULL ) {
  return -1;
 }

 handle->type        = HT_VIO;
 handle->sysio_flags = flags;

 if ( roar_vio_init_calls(&(handle->stream_vio)) == -1 ) {
  _close_handle(handle);
  return -1;
 }

 *vio = &(handle->stream_vio);

 if ( handleret != NULL )
  *handleret = handle;

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

 _update_nonblock(handle);

 return 0;
}

// -------------------------------------
// function to update O_NONBLOCK:
// -------------------------------------

static int _update_nonblock (struct handle * handle) {
 int opened = 0;
 int state  = handle->sysio_flags & O_NONBLOCK ? ROAR_SOCKET_NONBLOCK : ROAR_SOCKET_BLOCK;

 switch (handle->type) {
  case HT_NONE:
  case HT_STATIC:
  case HT_MIXER:
    // we can ignore setting of nonblock flag here.
    return 0;
   break;
  case HT_VIO:
    opened = 1;
   break;
  case HT_STREAM:
  case HT_WAVEFORM:
  case HT_MIDI:
  case HT_DMX:
    opened = handle->stream_opened;
   break;
 }

 if ( opened ) {
  return roar_vio_nonblock(&(handle->stream_vio), state);
 }

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
    ROAR_DBG("_ioctl_stream_format(*): unsupported format");
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

static int _ioctl_mixer (struct handle * handle, long unsigned int req, void * vp) {
 mixer_info * info;
 int channels;
 struct roar_mixer_settings mixer;
 int o_w    =  0;
 int o_sid  = -1;
 int * ip   = vp;
#if defined(DEBUG) && defined(DEBUG_IOCTL_NAMES)
 char * name = NULL;
#endif

#if defined(DEBUG) && defined(DEBUG_IOCTL_NAMES)
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
//  case SOUND_MIXER_INFO:             name = "SOUND_MIXER_INFO";             break;
  case SOUND_OLD_MIXER_INFO:         name = "SOUND_OLD_MIXER_INFO";         break;
  case SOUND_MIXER_ACCESS:           name = "SOUND_MIXER_ACCESS";           break;
  case SOUND_MIXER_AGC:              name = "SOUND_MIXER_AGC";              break;
  case SOUND_MIXER_3DSE:             name = "SOUND_MIXER_3DSE";             break;
  case SOUND_MIXER_GETLEVELS:        name = "SOUND_MIXER_GETLEVELS";        break;
  case SOUND_MIXER_SETLEVELS:        name = "SOUND_MIXER_SETLEVELS";        break;
  case SOUND_MIXER_PRIVATE1:         name = "SOUND_MIXER_PRIVATE1";         break;
  case SOUND_MIXER_PRIVATE2:         name = "SOUND_MIXER_PRIVATE2";         break;
  case SOUND_MIXER_PRIVATE3:         name = "SOUND_MIXER_PRIVATE3";         break;
  case SOUND_MIXER_PRIVATE4:         name = "SOUND_MIXER_PRIVATE4";         break;
  case SOUND_MIXER_PRIVATE5:         name = "SOUND_MIXER_PRIVATE5";         break;
  case OSS_GETVERSION:               name = "OSS_GETVERSION";               break;
//  case SOUND_MIXER_READ_CAPS:        name = "SOUND_MIXER_READ_CAPS";        break;
  case SOUND_MIXER_READ_MUTE:        name = "SOUND_MIXER_READ_MUTE";        break;
/*
  case :     name = "";     break;
  case :     name = "";     break;
*/
 }
 if ( name != NULL ) {
  ROAR_DBG("_ioctl_mixer(handle=%p, req=0x%lX, ip=%p): unspported mixer command %s", handle, req, ip, name);
  ROAR_DBG("_ioctl_mixer(handle=%p, req=0x%lX, ip=%p) = -1 // errno = ENOSYS", handle, req, ip);
  errno = ENOSYS;
  return -1;
 }
#endif

 switch (req) {
  case SOUND_MIXER_READ_VOLUME:    o_w = 0; o_sid = _mix_settings.sid.volume;   break;
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
  case SOUND_MIXER_WRITE_LINE:     o_w = 1; o_sid = _mix_settings.sid.line;     break;
  case SOUND_MIXER_WRITE_LINE1:    o_w = 1; o_sid = _mix_settings.sid.line1;    break;
  case SOUND_MIXER_WRITE_LINE2:    o_w = 1; o_sid = _mix_settings.sid.line2;    break;
  case SOUND_MIXER_WRITE_LINE3:    o_w = 1; o_sid = _mix_settings.sid.line3;    break;
#if 0
  case SOUND_MIXER_WRITE_DIGITAL1: o_w = 1; o_sid = _mix_settings.sid.digital1; break;
  case SOUND_MIXER_WRITE_DIGITAL2: o_w = 1; o_sid = _mix_settings.sid.digital2; break;
  case SOUND_MIXER_WRITE_DIGITAL3: o_w = 1; o_sid = _mix_settings.sid.digital3; break;
#endif
  // we handle PCM seperatly as we want to be abled to abled to handle it on a stream (not mixer), too:
  case SOUND_MIXER_READ_PCM:
    o_w = 0;
    if ( handle->type == HT_STREAM ) {
     o_sid = roar_stream_get_id(&(handle->stream));
    } else {
     o_sid = _mix_settings.sid.pcm;
    }
   break;
  case SOUND_MIXER_WRITE_PCM:
    o_w = 1;
    if ( handle->type == HT_STREAM ) {
     o_sid = roar_stream_get_id(&(handle->stream));
    } else {
     o_sid = _mix_settings.sid.pcm;
    }
   break;
 }
 if ( o_sid != -1 ) {
  // set/get volume
  if ( o_w ) {
   mixer.scale    = OSS_VOLUME_SCALE;
   mixer.mixer[0] = ( *ip       & 0xFF);
   mixer.mixer[1] = ((*ip >> 8) & 0xFF);
   if ( roar_set_vol(&(handle->session->con), o_sid, &mixer, 2) == -1 ) {
    errno = EIO;
    return -1;
   }
   return 0;
  } else {
   if ( roar_get_vol(&(handle->session->con), o_sid, &mixer, &channels) == -1 ) {
    errno = EIO;
    return -1;
   }
   *ip = ((OSS_VOLUME_SCALE*mixer.mixer[0])/mixer.scale) | (((OSS_VOLUME_SCALE*mixer.mixer[0])/mixer.scale)<<8);
   return 0;
  }
 }

 switch (req) {
  case SOUND_MIXER_READ_STEREODEVS: /* FIXME: check the streams for channel config */
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
#if 0
     *ip |= SOUND_MASK_DIGITAL1;
    if ( _mix_settings.sid.digital2 != -1 )
     *ip |= SOUND_MASK_DIGITAL2;
    if ( _mix_settings.sid.digital3 != -1 )
     *ip |= SOUND_MASK_DIGITAL3;
#endif

    return 0;
   break;
  case SOUND_MIXER_READ_RECMASK:
  case SOUND_MIXER_READ_RECSRC:
    *ip = SOUND_MASK_VOLUME; // we can currently only read from mixer
    return 0;
   break;
  case SOUND_MIXER_WRITE_RECSRC:
    if ( *ip == SOUND_MASK_VOLUME ) {
     return  0;
    } else {
     errno = ENOTSUP;
     return -1;
    }
   break;
  case SOUND_MIXER_READ_CAPS:
    *ip = 0;
    return 0;
   break;
  case SOUND_MIXER_INFO:
    info = vp;
    memset(info, 0, sizeof(*info));
    strcpy(info->id, "RoarAudio");
    strcpy(info->name, "RoarAudio");
    return 0;
   break;
 }

 ROAR_DBG("_ioctl_mixer(handle=%p, req=0x%lX, ip=%p): unknown mixer CTL", handle, req, ip);
// _os.ioctl(-1, req, ip);
 ROAR_DBG("_ioctl_mixer(handle=%p, req=0x%lX, ip=%p) = -1 // errno = ENOSYS", handle, req, ip);
 errno = ENOSYS;
 return -1;
}

// -------------------------------------
// buffer size calculation:
// -------------------------------------

static size_t _get_stream_buffersize (struct handle * handle) {
 if ( handle->stream_buffersize )
  return handle->stream_buffersize;

 return handle->stream_buffersize = handle->stream.info.rate     *
                                    handle->stream.info.channels *
                                    handle->stream.info.bits     / 800;
}

// -------------------------------------
// emulated functions follow:
// -------------------------------------

int     open(const char *pathname, int flags, ...) {
 int     ret;
 mode_t  mode = 0;
 va_list args;

 _init();

 if ( pathname == NULL ) {
  errno = EFAULT;
  return -1;
 }

 ROAR_DBG("open(pathname='%s', flags=%x, ...) = ?\n", pathname, flags);
 ret = _open_file(pathname, flags);

 switch (ret) {
  case -2:       // continue as normal, use _op.open()
    ROAR_DBG("open(pathname='%s', flags=%x, ...): is not handled by us, pass to kernel\n", pathname, flags);
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
  mode = va_arg(args, _VA_ARGS_MODE_T);
  va_end(args);
 }

 return _os.open(pathname, flags, mode);
}

int    open64(const char *__file, int __oflag, ...) {
 int     ret;
 mode_t  mode = 0;
 va_list args;

 _init();

 if ( __file == NULL ) {
  errno = EFAULT;
  return -1;
 }

 ROAR_DBG("open64(__file='%s', __oflags=%x, ...) = ?", __file, __oflag);
 ret = _open_file(__file, __oflag);

 switch (ret) {
  case -2:       // continue as normal, use _op.open()
    ROAR_DBG("open64(__file='%s', __oflags=%x, ...): not for us, passing to kernel", __file, __oflag);
   break;
  case -1:       // pass error to caller
    return -1;
   break;
  default:       // return successfully opened pointer to caller
    ROAR_DBG("open64(__file='%s', __oflags=%x, ...) = %i", __file, __oflag, ret);
    return ret;
   break;
 }

 if (__oflag & O_CREAT) {
  va_start(args, __oflag);
  mode = va_arg(args, _VA_ARGS_MODE_T);
  va_end(args);
 }

 if ( _os.open64 != NULL ) {
  return _os.open64(__file, __oflag, mode);
 } else {
#ifdef O_LARGEFILE
  return _os.open(__file, __oflag | O_LARGEFILE, mode);
#else
  return _os.open(__file, __oflag, mode);
#endif
 }
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
 struct roar_roardmx_message roardmxmsg;
 struct pointer * pointer;
 ssize_t ret;
 size_t i;

 _init();

 if ( (pointer = _get_pointer_by_fh(fd)) != NULL ) {
  ROAR_DBG("write(fd=%i, buf=%p, count=%lu) = ? // pointer write", fd, buf, (long unsigned int) count);
  switch (pointer->handle->type) {
   case HT_STREAM: // handle stream specific stuff
     if ( pointer->handle->stream_opened == 0 ) {
      if ( _open_stream(pointer->handle) == -1 ) {
       errno = EIO;
       return -1;
      }
     }
   case HT_VIO: // from here we only look at the VIO object of streams, or handle simple VIOs
     ret = roar_vio_write(&(pointer->handle->stream_vio), (char*)buf, count);
     if ( ret > 0 )
      pointer->handle->writec += ret;
     return ret;
    break;
   case HT_DMX: // DMX need specal handling as we need to convert the protocol
     if ( pointer->handle->stream_opened == 0 ) {
      if ( _open_stream(pointer->handle) == -1 ) {
       errno = EIO;
       return -1;
      }
     }
     if ( count > 0 ) {
      if ( roar_roardmx_message_new_sset(&roardmxmsg) == -1 ) {
       errno = EIO;
       return -1;
      }
      for (i = 0; i < count; i++) {
       if ( roar_roardmx_message_add_chanval(&roardmxmsg, pointer->handle->pos + i, ((unsigned char*)buf)[i]) == -1 ) {
#ifdef EMSGSIZE
        errno = EMSGSIZE;
#else
        errno = EIO;
#endif
        return -1;
       }
      }
      if ( roar_roardmx_message_send(&roardmxmsg, &(pointer->handle->stream_vio)) == -1 ) {
       errno = EIO;
       return -1;
      }
     }
     pointer->handle->pos += count;
     return count;
    break;
   default: // we don't know what to do with other types
     errno = EINVAL;
     return -1;
    break;
  }
 }

 return _os.write(fd, buf, count);
}

ssize_t read(int fd, void *buf, size_t count) {
 struct pointer * pointer;
 ssize_t ret;

 _init();

 if ( (pointer = _get_pointer_by_fh(fd)) != NULL ) {
  ROAR_DBG("read(fd=%i, buf=%p, count=%lu) = ? // pointer read", fd, buf, (long unsigned int)count);

  switch (pointer->handle->type) {
   case HT_STREAM:
     if ( pointer->handle->stream_opened == 0 ) {
      if ( _open_stream(pointer->handle) == -1 ) {
       errno = EIO;
       return -1;
      }
     }
   case HT_VIO:
     ret = roar_vio_read(&(pointer->handle->stream_vio), buf, count);
     if ( ret > 0 )
      pointer->handle->readc += ret;
     return ret;
    break;
   case HT_STATIC:
     ROAR_DBG("read(fd=%i, buf=%p, count=%lu) = ? // type=HT_STATIC", fd, buf, (long unsigned int)count);
     ret = pointer->handle->pos + count; // calc the end of the read

     if ( ret > (ssize_t)pointer->handle->userdata.sf.len ) {
      count = pointer->handle->userdata.sf.len - pointer->handle->pos;
     }

     memcpy(buf, pointer->handle->userdata.sf.data + pointer->handle->pos, count);
     pointer->handle->pos += count;
     return count;
    break;
   default:
     errno = EINVAL;
     return -1;
    break;
  }
 }

 return _os.read(fd, buf, count);
}

off_t lseek(int fildes, off_t offset, int whence) {
 struct pointer * pointer;
 ssize_t tmp;

 _init();

 if ( (pointer = _get_pointer_by_fh(fildes)) != NULL ) {
  switch (pointer->handle->type) {
   case HT_DMX:
     switch (whence) {
      case SEEK_SET:
        pointer->handle->pos  = offset;
       break;
      case SEEK_CUR:
        pointer->handle->pos += offset;
       break;
      case SEEK_END:
      default:
        errno = EINVAL;
        return -1;
       break;
     }
     return pointer->handle->pos;
    break;
   case HT_VIO:
     return roar_vio_lseek(&(pointer->handle->stream_vio), offset, whence);
    break;
   case HT_STATIC:
     switch (whence) {
      case SEEK_SET:
        if ( offset < 0 || offset > (ssize_t)pointer->handle->userdata.sf.len ) {
         errno = EINVAL;
         return -1;
        }
        pointer->handle->pos  = offset;
       break;
      case SEEK_CUR:
        tmp = pointer->handle->pos + offset;
        if ( tmp < 0 || tmp > (ssize_t)pointer->handle->userdata.sf.len ) {
         errno = EINVAL;
         return -1;
        }
        pointer->handle->pos = tmp;
       break;
      case SEEK_END:
        tmp = pointer->handle->userdata.sf.len + offset;
        if ( tmp < 0 || tmp > (ssize_t)pointer->handle->userdata.sf.len ) {
         errno = EINVAL;
         return -1;
        }
        pointer->handle->pos = tmp;
       break;
      default:
        errno = EINVAL;
        return -1;
       break;
     }
    break;
   default:
     errno = EINVAL;
     return -1;
    break;
  }
 }

 return _os.lseek(fildes, offset, whence);
}

IOCTL() {
 map_args;
 struct pointer * pointer;
 struct handle  * handle;
 int * ip = NULL;
 size_t tmp;
 audio_buf_info * bi;
 count_info     * ci;
#ifdef __FIXME__
 char * nosys_reqname = NULL;
#endif
#ifdef va_argp
 va_list args;
#endif

 _init();

 ROAR_DBG("ioctl(__fd=%i, __request=0x%lX) = ?", __fd, (long unsigned int) __request);

#ifdef va_argp
 va_start (args, ioctl_lastarg);
 argp = va_arg (args, void *);
 va_end (args);
#endif

// ROAR_DBG("ioctl(fh=%i, request=%i, ...) = ?", __fd, __request);

 ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): argp=%p", __fd, (long unsigned int) __request, argp);

 if ( (pointer = _get_pointer_by_fh(__fd)) != NULL ) {
  ip = argp;
//  ROAR_DBG("ioctl(__fd=%i, __request=0x%lx): ip=%p", __fd, (long unsigned int) __request, ip);
#ifdef __FIXME__
  switch (__request) {
   case SOUND_PCM_READ_RATE: nosys_reqname     = "SOUND_PCM_READ_RATE";     break;
   case SOUND_PCM_READ_CHANNELS: nosys_reqname = "SOUND_PCM_READ_CHANNELS"; break;
   case SOUND_PCM_READ_BITS: nosys_reqname     = "SOUND_PCM_READ_BITS";     break;
   case SOUND_PCM_READ_FILTER: nosys_reqname   = "SOUND_PCM_READ_FILTER";   break;
   case SNDCTL_COPR_RESET: nosys_reqname       = "SNDCTL_COPR_RESET";       break;
   case SNDCTL_COPR_LOAD: nosys_reqname        = "SNDCTL_COPR_LOAD";        break;
   case SNDCTL_COPR_HALT: nosys_reqname        = "SNDCTL_COPR_HALT";        break;
   case SNDCTL_COPR_RDATA: nosys_reqname       = "SNDCTL_COPR_RDATA";       break;
   case SNDCTL_COPR_RCODE: nosys_reqname       = "SNDCTL_COPR_RCODE";       break;
   case SNDCTL_COPR_WDATA: nosys_reqname       = "SNDCTL_COPR_WDATA";       break;
   case SNDCTL_COPR_WCODE: nosys_reqname       = "SNDCTL_COPR_WCODE";       break;
   case SNDCTL_COPR_RUN: nosys_reqname         = "SNDCTL_COPR_RUN";         break;
   case SNDCTL_COPR_SENDMSG: nosys_reqname     = "SNDCTL_COPR_SENDMSG";     break;
   case SNDCTL_COPR_RCVMSG: nosys_reqname      = "SNDCTL_COPR_RCVMSG";      break;
   case SNDCTL_DSP_GETCAPS: nosys_reqname      = "SNDCTL_DSP_GETCAPS";      break;
   default: nosys_reqname = "<<<UNKNOWN>>>"; break;
/*
   case : nosys_reqname = ""; break;
   case : nosys_reqname = ""; break;
   case : nosys_reqname = ""; break;
*/
  }
#endif
  switch ((handle = pointer->handle)->type) {
   case HT_STREAM:
     switch (__request) {
      case SNDCTL_DSP_RESET:
      case SNDCTL_DSP_POST:
      case SNDCTL_DSP_SYNC: // ignore for the moment.
      case SNDCTL_DSP_SETFRAGMENT: // any fragments should be ok for us...
      case SNDCTL_DSP_SETTRIGGER: // we should implement this using PAUSE flag.
        return 0;
       break;
      case SNDCTL_DSP_SPEED:
        handle->stream.info.rate = *ip;
        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): rate=%i", __fd, (long unsigned int) __request, *ip);
        return 0;
       break;
      case SNDCTL_DSP_CHANNELS:
        handle->stream.info.channels = *ip;
        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): channels=%i", __fd, (long unsigned int) __request, *ip);
        return 0;
       break;
      case SNDCTL_DSP_STEREO:
        handle->stream.info.channels = *ip ? 2 : 1;
        return 0;
       break;
      case SNDCTL_DSP_GETBLKSIZE:
        *ip = _get_stream_buffersize(handle);
        return 0;
       break;
      case SNDCTL_DSP_SETFMT:
        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): fmt=0x%x", __fd, (long unsigned int) __request, *ip);
        return _ioctl_stream_format(handle, *ip);
       break;
      case SNDCTL_DSP_GETFMTS:
//        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): ip=%p", __fd, (long unsigned int) __request, ip);
        *ip = _ioctl_stream_format_list();
        return 0;
       break;
      case SNDCTL_DSP_GETOSPACE:
      case SNDCTL_DSP_GETISPACE:
        bi = argp;
        memset(bi, 0, sizeof(*bi));
        bi->bytes      = _get_stream_buffersize(handle);
        bi->fragments  = 1;
        bi->fragsize   = bi->bytes;
        bi->fragstotal = 1;
        return 0;
       break;
      case SNDCTL_DSP_GETOPTR:
        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): writec=%lu", __fd, (long unsigned int) __request, (long unsigned int) handle->writec);
        ci = argp;
        memset(ci, 0, sizeof(*ci));
        ci->bytes  = handle->writec;
        ci->blocks = ci->bytes / (tmp = _get_stream_buffersize(handle));
        ci->ptr    = ci->bytes % tmp;
        return 0;
       break;
      case SNDCTL_DSP_GETIPTR:
        ci = argp;
        memset(ci, 0, sizeof(*ci));
        ci->bytes  = handle->readc;
        ci->blocks = ci->bytes / (tmp = _get_stream_buffersize(handle));
        ci->ptr    = ci->bytes % tmp;
        return 0;
       break;
#ifdef SNDCTL_DSP_GETPLAYVOL
      case SNDCTL_DSP_GETPLAYVOL:
        return _ioctl_mixer(handle, SOUND_MIXER_READ_PCM, argp);
       break;
#endif
#ifdef SNDCTL_DSP_SETPLAYVOL
      case SNDCTL_DSP_SETPLAYVOL:
        return _ioctl_mixer(handle, SOUND_MIXER_WRITE_PCM, argp);
       break;
#endif
#ifdef SNDCTL_DSP_NONBLOCK
      case SNDCTL_DSP_NONBLOCK:
        return fcntl(__fd, F_SETFL, handle->sysio_flags|O_NONBLOCK);
       break;
#endif
      default:
#ifdef __FIXME__
        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX (%s)) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request, nosys_reqname);
#else
        ROAR_DBG("ioctl(__fd=%i, __request=0x%lX) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request);
#endif
        errno = ENOSYS;
        return -1;
     }
    break;
   case HT_MIXER:
     return _ioctl_mixer(handle, __request, argp);
    break;
   default:
     ROAR_DBG("ioctl(__fd=%i, __request=0x%lX): unknown handle type: no ioctl()s supported", __fd, (long unsigned int) __request);
     ROAR_DBG("ioctl(__fd=%i, __request=0x%lX) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request);
     errno = EINVAL;
     return -1;
    break;
  }
 }

#ifdef IOCTL_IS_ALIAS
 errno = ENOSYS;
 return -1;
#else
 ROAR_DBG("ioctl(__fd=%i, __request=0x%lX, argp=%p): not for us, passing to kernel", __fd, (long unsigned int) __request, argp);
 return _os.ioctl(__fd, __request, argp);
#endif
}

int dup(int oldfd) {
 struct pointer * pointer;
 int ret;

 _init();

 ret = _os.dup(oldfd);

 if (ret == -1)
  return -1;

 if ( (pointer = _get_pointer_by_fh(oldfd)) != NULL ) {
  if ( _attach_pointer(pointer->handle, ret) == NULL ) {
   _os.close(ret);
   return -1;
  }
 }

 return ret;
}

int dup2(int oldfd, int newfd) {
 struct pointer * pointer;
 int ret;

 _init();

 ret = _os.dup2(oldfd, newfd);

 if (ret == -1)
  return -1;

 if ( (pointer = _get_pointer_by_fh(oldfd)) != NULL ) {
  if ( _attach_pointer(pointer->handle, ret) == NULL ) {
   _os.close(ret);
   return -1;
  }
 }

 return ret;
}

int select(int nfds, fd_set *readfds, fd_set *writefds,
           fd_set *exceptfds, struct timeval *timeout) {
 struct roar_vio_selecttv rtv;
 struct roar_vio_select * sv  = NULL;
 struct pointer * pointer;
 struct handle  * handle;
 ssize_t ret;
 size_t num = 0;
 size_t idx;
 int i;
 int i_r, i_w, i_e;
 int max_index = -1;
 static volatile int is_critical = 0;

 _init();

 if ( is_critical )
  return _os.select(nfds, readfds, writefds, exceptfds, timeout);

 ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = ?", nfds, readfds, writefds, exceptfds, timeout);

 if ( nfds == 0 ) {
  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = 0", nfds, readfds, writefds, exceptfds, timeout);
  return 0;
 }

 if ( readfds == NULL && writefds == NULL && exceptfds == NULL ) {
  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = 0", nfds, readfds, writefds, exceptfds, timeout);
  return 0;
 }

 if ( timeout != NULL ) {
  rtv.sec = timeout->tv_sec;
  rtv.nsec = timeout->tv_usec*1000;
 }

 // count number of handles:
 for (i = 0; i < nfds; i++) {
  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p): i=%i, EXISTS", nfds, readfds, writefds, exceptfds, timeout, i);
  if ( (readfds   != NULL && FD_ISSET(i, readfds  )) ||
       (writefds  != NULL && FD_ISSET(i, writefds )) ||
       (exceptfds != NULL && FD_ISSET(i, exceptfds))
     ) {
   ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p): i=%i, EXISTS", nfds, readfds, writefds, exceptfds, timeout, i);
   num++;
   max_index = i;
  }
 }

 if ( num == 0 ) {
  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = 0", nfds, readfds, writefds, exceptfds, timeout);
  return 0;
 }

 nfds = max_index + 1;

 // create sv;
 sv = roar_mm_malloc(sizeof(struct roar_vio_select)*num);
 if ( sv == NULL ) {
  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = -1", nfds, readfds, writefds, exceptfds, timeout);
  return -1;
 }

 memset(sv, 0, sizeof(struct roar_vio_select)*num);

 for (i = 0, idx = 0; i < nfds; i++) {
  if ( idx >= num ) {
   roar_mm_free(sv);
   errno = EFAULT;
   ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = -1 // i=%i, idx=%i, num=%i", nfds, readfds, writefds, exceptfds, timeout, i, (int)idx, (int)num);
   return -1;
  }
  i_r = readfds   != NULL && FD_ISSET(i, readfds);
  i_w = writefds  != NULL && FD_ISSET(i, writefds);
  i_e = exceptfds != NULL && FD_ISSET(i, exceptfds);

  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p): i=%i, i_r=%i, i_w=%i, i_e=%i", nfds, readfds, writefds, exceptfds, timeout, i, i_r, i_w, i_e);

  if ( i_r || i_w || i_e ) {
   // TODO: use VIO for pointers...
   if ( (pointer = _get_pointer_by_fh(i)) != NULL ) {
    handle = pointer->handle;
    sv[idx].vio     = NULL;
    sv[idx].fh      = -1;
    switch (handle->type) {
     case HT_DMX:
     case HT_STREAM:
       if ( ! handle->stream_opened ) {
        // implement this as statichly return OK
        errno = ENOSYS;
        return -1;
       }
     case HT_VIO:
       sv[idx].vio = &(handle->stream_vio);
      break;
     default: /* non supported type */
       errno = EINVAL;
       return -1;
      break;
    }
   } else {
    sv[idx].vio     = NULL;
    sv[idx].fh      = i;
   }

   sv[idx].ud.si   = i;
   sv[idx].eventsq = (i_r ? ROAR_VIO_SELECT_READ   : 0) |
                     (i_w ? ROAR_VIO_SELECT_WRITE  : 0) |
                     (i_e ? ROAR_VIO_SELECT_EXCEPT : 0);
   idx++;
  }
 }

 is_critical++;
 ret = roar_vio_select(sv, num, timeout == NULL ? NULL : &rtv, NULL);
 is_critical--;

 if ( ret < 1 ) {
  roar_mm_free(sv);
  ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = %i", nfds, readfds, writefds, exceptfds, timeout, (int)ret);

  if ( ret == 0 ) {
   if ( readfds != NULL ) {
    FD_ZERO(readfds);
   }
   if ( writefds != NULL ) {
    FD_ZERO(writefds);
   }
   if ( exceptfds != NULL ) {
    FD_ZERO(exceptfds);
   }
  }

  return ret;
 }

 // update readfds, writefds, exceptfds:
 if ( readfds != NULL )
  FD_ZERO(readfds);

 if ( writefds != NULL )
  FD_ZERO(writefds);

 if ( exceptfds != NULL )
  FD_ZERO(exceptfds);

 for (idx = 0; idx < num; idx++) {
  if ( sv[idx].eventsa == 0 )
   continue;

  if ( sv[idx].eventsa & ROAR_VIO_SELECT_READ )
   if ( readfds != NULL )
    FD_SET(sv[idx].ud.si, readfds);

  if ( sv[idx].eventsa & ROAR_VIO_SELECT_WRITE )
   if ( writefds != NULL )
    FD_SET(sv[idx].ud.si, writefds);

  if ( sv[idx].eventsa & ROAR_VIO_SELECT_EXCEPT )
   if ( exceptfds != NULL )
    FD_SET(sv[idx].ud.si, exceptfds);
 }

 roar_mm_free(sv);

 ROAR_DBG("select(nfds=%i, readfds=%p, writefds=%p, exceptfds=%p, timeout=%p) = %i", nfds, readfds, writefds, exceptfds, timeout, (int)ret);
 return ret;
}

int fcntl(int fd, int cmd, ...) {
 enum { NONE, UNKNOWN, LONG, POINTER } type = NONE;
 struct pointer * pointer;
 va_list ap;
 long argl = -1;
 void * vp = NULL;
 int ret   = -1;
 int diff;

 _init();

 ROAR_DBG("fcntl(fd=%i, cmd=%i, ...) = ?", fd, cmd);

 switch (cmd) {
  case F_DUPFD:
  case F_SETFD:
  case F_SETFL:
  case F_SETOWN:
#ifdef F_SETSIG
  case F_SETSIG:
#endif
#ifdef F_SETLEASE
  case F_SETLEASE:
#endif
#ifdef F_NOTIFY
  case F_NOTIFY:
#endif
    type = LONG;
   break;
  case F_GETFD:
  case F_GETFL:
  case F_GETOWN:
#ifdef F_GETSIG
  case F_GETSIG:
#endif
#ifdef F_GETLEASE
  case F_GETLEASE:
#endif
    type = NONE;
   break;
  case F_GETLK:
  case F_SETLK:
  case F_SETLKW:
#ifdef F_GETLK64
#if F_GETLK64 != F_GETLK
  case F_GETLK64:
#endif
#endif
#ifdef F_SETLK64
#if F_SETLK64 != F_SETLK
  case F_SETLK64:
#endif
#endif
#ifdef F_SETLKW64
#if F_SETLKW64 != F_SETLKW
  case F_SETLKW64:
#endif
#endif
    type = POINTER;
   break;
/*
  case F_EXLCK:
  case F_GETLK64:
  case F_SETLK64:
  case F_SETLKW64:
  case F_SHLCK:
  case F_LINUX_SPECIFIC_BASE:
  case F_INPROGRESS:
*/
  default:
    type = UNKNOWN;
 }

 if ( type == UNKNOWN ) {
  ROAR_DBG("fcntl(fd=%i, cmd=%i, ...): unknown data type!", fd, cmd);
  ROAR_DBG("fcntl(fd=%i, cmd=%i, ...) = -1 // errno = EINVAL", fd, cmd);
  errno = EINVAL;
  return -1;
 }

 if ( type != NONE ) {
  va_start(ap, cmd);
  switch (type) {
   case LONG:
     argl = va_arg(ap, long);
    break;
   case POINTER:
     vp = va_arg(ap, void*);
    break;
   default: /* make compiler happy */
    break;
  }
  va_end(ap);
 }

 if ( (pointer = _get_pointer_by_fh(fd)) == NULL ) {
  switch (type) {
   case NONE:
     ROAR_DBG("fcntl(fd=%i, cmd=%i): fd is true sysio, pass call to kernel", fd, cmd);
     return _os.fcntl(fd, cmd);
    break;
   case LONG:
     ROAR_DBG("fcntl(fd=%i, cmd=%i, arg=%li): fd is true sysio, pass call to kernel", fd, cmd, argl);
     return _os.fcntl(fd, cmd, argl);
    break;
   case POINTER:
     ROAR_DBG("fcntl(fd=%i, cmd=%i, lock=%p): fd is true sysio, pass call to kernel", fd, cmd, vp);
     return _os.fcntl(fd, cmd, vp);
    break;
   default: /* make compiler happy */
    break;
  }
 }

 ROAR_DBG("fcntl(fd=%i, cmd=%i, ...): fd is true pointer, handle internaly", fd, cmd);

 switch (cmd) {
  case F_DUPFD:
    ret = _os.fcntl(fd, F_DUPFD, argl);

    if ( ret != -1 ) {
     if ( _attach_pointer(pointer->handle, ret) == NULL ) {
      _os.close(ret);
      ret = -1;
     }
    }
   break;
  case F_SETFD:
    if ( argl == 0 ) {
     ret = 0;
    } else {
     errno = ENOSYS;
     ret = -1;
    }
   break;
  case F_GETFD:
    ret = 0;
   break;
  case F_GETFL:
    ret = pointer->handle->sysio_flags;
   break;
  case F_SETFL:
    diff  = (int)argl ^ pointer->handle->sysio_flags;
    diff &= (int)~(int)_O_PARA_DIR;
    diff &= (int)~(int)_O_PARA_IGN;

    if ( diff & O_NONBLOCK ) {
     diff -= O_NONBLOCK;
     pointer->handle->sysio_flags ^= O_NONBLOCK;
     if ( _update_nonblock(pointer->handle) == -1 ) {
      pointer->handle->sysio_flags ^= O_NONBLOCK;
      return -1;
     }
    }

    if ( diff == 0 ) { // only flags changed we ignore anyway.
     pointer->handle->sysio_flags  = (int)argl;
     ret = 0;
    } else {
     errno = EINVAL;
     ret = -1;
    }
   break;
/* TODO: add support for those types:
  case F_SETFD:
  case F_SETOWN:
  case F_SETSIG:
  case F_SETLEASE:
  case F_NOTIFY:
  case F_GETOWN:
  case F_GETSIG:
  case F_GETLEASE:
  case F_GETLK:
  case F_SETLK:
  case F_SETLKW:
*/
  default:
    errno = ENOSYS;
    ret = -1;
   break;
 }

 return ret;
}

int access(const char *pathname, int mode) {
 struct devices * ptr = NULL;

 _init();

 if ( (ptr = _get_device(pathname)) != NULL ) {
  // the only flag we do not support is +x, which means
  // we need to reject all requets with X_OK.
  if ( mode & X_OK ) {
   errno = EACCES;
   return -1;
  }

  // in addition HT_STATIC files do not support write (+w)
  // so we need to reject W_OK.
  if ( ptr->type == HT_STATIC && (mode & W_OK) ) {
   errno = EACCES;
   return -1;
  }

  // Else the access is granted:
  return 0;
 }

 return _os.access(pathname, mode);
}

int creat(const char *_CREAT_ARG_PATHNAME, mode_t mode) {
 _init();

 if ( _get_device(_CREAT_ARG_PATHNAME) != NULL ) {
  errno = EEXIST;
  return -1;
 }

 return _os.creat(_CREAT_ARG_PATHNAME, mode);
}

// -------------------------------------
// emulated *stat*() functions follow:
// -------------------------------------

int stat(const char *path, struct stat *buf) {
 struct devices * ptr;

 _init();

 if ( (ptr = _get_device(path)) != NULL ) {
  errno = ENOSYS;
  return -1;
 }

 return _os.stat(path, buf);
}

int fstat(int filedes, struct stat *buf) {
 struct pointer * pointer;

 _init();

 if ( (pointer = _get_pointer_by_fh(filedes)) == NULL ) {
  return _os.fstat(filedes, buf);
 }

 errno = ENOSYS;
 return -1;
}

int lstat(const char *path, struct stat *buf) {
 _init();

 if ( _get_device(path) != NULL ) {
  return stat(path, buf);
 }

 return _os.lstat(path, buf);
}

// -------------------------------------
// emulated stdio functions follow:
// -------------------------------------

//roar_vio_to_stdio

static int _vio_close    (struct roar_vio_calls * vio) {
 int ret = 0;

 if ( roar_vio_get_fh(vio) != -1 )
  ret = close(roar_vio_get_fh(vio));

 roar_mm_free(vio);

 return ret;
}

FILE *fopen(const char *path, const char *mode) {
 struct roar_vio_calls * vio;
 FILE  * fr;
 int     ret;
 int     r = 0, w = 0;
 int     flags = 0;
 int     i;
 register char c;

 _init();

 if ( path == NULL || mode == NULL ) {
  errno = EFAULT;
  return NULL;
 }

 ROAR_DBG("open(path='%s', mode='%s') = ?\n", path, mode);

 for (i = 0; (c = mode[i]) != 0; i++) {
  switch (c) {
   case 'r': r = 1; break;
   case 'w': w = 1; break;
   case 'a': w = 1; break;
   case '+':
     r = 1;
     w = 1;
    break;
  }
 }

 if ( r && w ) {
  flags = O_RDWR;
 } else if ( r ) {
  flags = O_RDONLY;
 } else if ( w ) {
  flags = O_WRONLY;
 } else {
  errno = EINVAL;
  return NULL;
 }

 ret = _open_file(path, flags);

 switch (ret) {
  case -2:       // continue as normal, use _op.open()
    ROAR_DBG("fopen(path='%s', mode='%s'): not for us, passing to libc", path, mode);
   break;
  case -1:       // pass error to caller
    return NULL;
   break;
  default:       // return successfully opened pointer to caller
#ifdef __USE_FDOPEN__
    ROAR_DBG("fopen(path='%s', mode='%s') = fdopen(%i, '%s')", path, mode, ret, mode);
    return fdopen(ret, r ? (w ? "rw" : "r") : "w");
#else
    if ( (vio = roar_mm_malloc(sizeof(struct roar_vio_calls))) == NULL ) {
     return NULL; // errno should be set correctly by roar_mm_malloc().
    }

    roar_vio_init_calls(vio);  // TODO: add error handling.
    roar_vio_set_fh(vio, ret); // TODO: add error handling.
    vio->close = _vio_close;
    if ( (fr = roar_vio_to_stdio(vio, flags)) == NULL ) {
     _vio_close(vio);
     errno = EIO;
     return NULL;
    } else {
     return fr;
    }
#endif
   break;
 }

 return _os.fopen(path, mode);
}

// -------------------------------------
// RoarAudio plugin functions follow:
// -------------------------------------

ROAR_DL_PLUGIN_START(libroaross) {
 (void)para;
 _init();
} ROAR_DL_PLUGIN_END

#endif

//ll
