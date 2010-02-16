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

#if defined(RTLD_NEXT)
#define REAL_LIBC RTLD_NEXT
#else
#define REAL_LIBC ((void *) -1L)
#endif

#ifndef ENOTSUP
#define ENOTSUP ENOSYS
#endif

#if defined(ROAR_OS_NETBSD) && defined(ioctl)
#define IOCTL_IS_ALIAS
#endif

#ifdef ROAR_OS_FREEBSD
#define mode_t int
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
#define HT_NONE       0
#define HT_STREAM     1
#define HT_MIXER      2
#define HT_WAVEFORM   3
#define HT_MIDI       4
#define HT_DMX        5

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
 size_t                stream_buffersize;
 size_t                readc, writec;
 size_t                pos;
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
#ifndef IOCTL_IS_ALIAS
 _os.ioctl = dlsym(REAL_LIBC, "ioctl");
#endif
 _os.lseek = dlsym(REAL_LIBC, "lseek");
 _os.fopen = dlsym(REAL_LIBC, "fopen");
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

 if ( pipe(p) == -1 )
  return -1;

 close(p[1]);

 return p[0];
}

static struct session * _open_session (char * server, char * name) {
 ROAR_DBG("_open_session(server='%s', name='%s') = ?", server, name);
 ROAR_DBG("_open_session(server='%s', name='%s'): _session.refc=%i", server, name, _session.refc);

 if ( _session.refc == 0 ) {

  if ( name == NULL )
   name = getenv("ROAR_OSS_CLIENT_NAME");

  if ( name == NULL )
   name = "libroaross client";

  if ( roar_simple_connect(&(_session.con), server, name) == -1 )
   return NULL;

  _find_volume_sid(&_session);

  if ( getenv("ROAR_OSS_KEEP_SESSION") != NULL )
   _session.refc++;
 }

 _session.refc++;

 ROAR_DBG("_open_session(server='%s', name='%s') = %p", server, name, &_session);
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

 ROAR_DBG("_open_handle(session=%p) = ?", session);

 if ( (handle = roar_mm_malloc(sizeof(struct handle))) == NULL )
  return NULL;

 memset(handle, 0, sizeof(struct handle));

 handle->refc = 1;
 handle->session = session;
 session->refc++; // TODO: better warp this
 handle->type = HT_NONE;
 handle->stream_dir = ROAR_DIR_PLAY;
 roar_stream_new(&(handle->stream), ROAR_RATE_DEFAULT, ROAR_CHANNELS_DEFAULT, ROAR_BITS_DEFAULT, ROAR_CODEC_DEFAULT);

 ROAR_DBG("_open_handle(session=%p) = %p", session, handle);
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
  {"/dev/dsp",           HT_WAVEFORM},
  {"/dev/audio",         HT_WAVEFORM},
  {"/dev/sound/dsp",     HT_WAVEFORM},
  {"/dev/sound/audio",   HT_WAVEFORM},
  {"/dev/mixer",         HT_MIXER},
  {"/dev/sound/mixer",   HT_MIXER},
  {"/dev/midi",          HT_MIDI},
  {"/dev/rmidi",         HT_MIDI},
  {"/dev/sound/midi",    HT_MIDI},
  {"/dev/sound/rmidi",   HT_MIDI},
  {"/dev/dmx",           HT_DMX},
  {"/dev/misc/dmx",      HT_DMX},
  {"/dev/dmxin",         HT_DMX},
  {"/dev/misc/dmxin",    HT_DMX},
#ifdef ROAR_DEFAULT_OSS_DEV
  {ROAR_DEFAULT_OSS_DEV, HT_WAVEFORM},
#endif
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

 handle->type       = ptr->type;
 handle->stream_dir = -1;

 switch (flags & (O_RDONLY|O_WRONLY|O_RDWR)) {
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
     default:
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
     default:
       return -1;
    }
   break;
  case O_RDWR:
    switch (ptr->type) {
     case HT_WAVEFORM:
       handle->stream_dir = ROAR_DIR_BIDIR;
      break;
     default:
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
  ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p): unspported mixer command %s", handle, req, ip, name);
  ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p) = -1 // errno = ENOSYS", handle, req, ip);
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
   mixer.scale    = 65535;
   mixer.mixer[0] = ( *ip       & 0xFF)*65535/OSS_VOLUME_SCALE;
   mixer.mixer[1] = ((*ip >> 8) & 0xFF)*65535/OSS_VOLUME_SCALE;
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

 ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p): unknown mixer CTL", handle, req, ip);
// _os.ioctl(-1, req, ip);
 ROAR_DBG("_ioctl_mixer(handle=%p, req=%lu, ip=%p) = -1 // errno = ENOSYS", handle, req, ip);
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
 struct roar_roardmx_message roardmxmsg;
 struct pointer * pointer;
 ssize_t ret;
 int i;

 _init();

 if ( (pointer = _get_pointer_by_fh(fd)) != NULL ) {
  switch (pointer->handle->type) {
   case HT_STREAM:
     if ( pointer->handle->stream_opened == 0 ) {
      if ( _open_stream(pointer->handle) == -1 ) {
       errno = EIO;
       return -1;
      }
     }
     ret = roar_vio_write(&(pointer->handle->stream_vio), (char*)buf, count);
     if ( ret > 0 )
      pointer->handle->writec += ret;
     return ret;
    break;
   case HT_DMX:
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
   default:
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
  if ( pointer->handle->type == HT_STREAM ) {
   if ( pointer->handle->stream_opened == 0 ) {
    if ( _open_stream(pointer->handle) == -1 ) {
     errno = EIO;
     return -1;
    }
   }
   ret = roar_vio_read(&(pointer->handle->stream_vio), buf, count);
   if ( ret > 0 )
    pointer->handle->readc += ret;
   return ret;
  } else {
   errno = EINVAL;
   return -1;
  }
 }

 return _os.read(fd, buf, count);
}

off_t lseek(int fildes, off_t offset, int whence) {
 struct pointer * pointer;

 _init();

 if ( (pointer = _get_pointer_by_fh(fildes)) != NULL ) {
  if ( pointer->handle->type == HT_DMX ) {
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
  } else {
   errno = EINVAL;
   return -1;
  }
 }

 return _os.lseek(fildes, offset, whence);
}

IOCTL() {
 map_args;
 struct pointer * pointer;
 struct handle  * handle;
 int * ip = NULL;
 audio_buf_info * bi;
 count_info     * ci;
#ifdef __FIXME__
 char * nosys_reqname = NULL;
#endif
#ifdef va_argp
 va_list args;
#endif

 _init();

// ROAR_DBG("ioctl(__fd=%i, __request=%lu) = ?", __fd, (long unsigned int) __request);

#ifdef va_argp
 va_start (args, ioctl_lastarg);
 argp = va_arg (args, void *);
 va_end (args);
#endif

// ROAR_DBG("ioctl(__fd=%i, __request=%lu): argp=%p", __fd, (long unsigned int) __request, argp);

 if ( (pointer = _get_pointer_by_fh(__fd)) != NULL ) {
  ip = argp;
//  ROAR_DBG("ioctl(__fd=%i, __request=%lu): ip=%p", __fd, (long unsigned int) __request, ip);
#ifdef __FIXME__
  switch ((handle = pointer->handle)->type) {
   case SOUND_PCM_READ_RATE: nosys_reqname = "SOUND_PCM_READ_RATE"; break;
   case SOUND_PCM_READ_CHANNELS: nosys_reqname = "SOUND_PCM_READ_CHANNELS"; break;
   case SOUND_PCM_READ_BITS: nosys_reqname = "SOUND_PCM_READ_BITS"; break;
   case SOUND_PCM_READ_FILTER: nosys_reqname = "SOUND_PCM_READ_FILTER"; break;
   case SNDCTL_COPR_RESET: nosys_reqname = "SNDCTL_COPR_RESET"; break;
   case SNDCTL_COPR_LOAD: nosys_reqname = "SNDCTL_COPR_LOAD"; break;
   case SNDCTL_COPR_HALT: nosys_reqname = "SNDCTL_COPR_HALT"; break;
   case SNDCTL_COPR_RDATA: nosys_reqname = "SNDCTL_COPR_RDATA"; break;
   case SNDCTL_COPR_RCODE: nosys_reqname = "SNDCTL_COPR_RCODE"; break;
   case SNDCTL_COPR_WDATA: nosys_reqname = "SNDCTL_COPR_WDATA"; break;
   case SNDCTL_COPR_WCODE: nosys_reqname = "SNDCTL_COPR_WCODE"; break;
   case SNDCTL_COPR_RUN: nosys_reqname = "SNDCTL_COPR_RUN"; break;
   case SNDCTL_COPR_SENDMSG: nosys_reqname = "SNDCTL_COPR_SENDMSG"; break;
   case SNDCTL_COPR_RCVMSG: nosys_reqname = "SNDCTL_COPR_RCVMSG"; break;
   case SNDCTL_DSP_SPEED: nosys_reqname = "SNDCTL_DSP_SPEED"; break;
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
      case SNDCTL_DSP_SETFRAGMENT: // any fragments should be ok for us...
        return 0;
       break;
      case SNDCTL_DSP_SPEED:
        handle->stream.info.rate = *ip;
        ROAR_DBG("ioctl(__fd=%i, __request=%lu): rate=%i", __fd, (long unsigned int) __request, *ip);
        return 0;
       break;
      case SNDCTL_DSP_CHANNELS:
        handle->stream.info.channels = *ip;
        ROAR_DBG("ioctl(__fd=%i, __request=%lu): channels=%i", __fd, (long unsigned int) __request, *ip);
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
        ROAR_DBG("ioctl(__fd=%i, __request=%lu): fmt=0x%x", __fd, (long unsigned int) __request, *ip);
        return _ioctl_stream_format(handle, *ip);
       break;
      case SNDCTL_DSP_GETFMTS:
//        ROAR_DBG("ioctl(__fd=%i, __request=%lu): ip=%p", __fd, (long unsigned int) __request, ip);
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
        ci = argp;
        memset(ci, 0, sizeof(*ci));
        ci->bytes  = handle->writec;
        ci->blocks = ci->bytes / _get_stream_buffersize(handle);
        ci->ptr    = 0;
        return 0;
       break;
      case SNDCTL_DSP_GETIPTR:
        ci = argp;
        memset(ci, 0, sizeof(*ci));
        ci->bytes  = handle->readc;
        ci->blocks = ci->bytes / _get_stream_buffersize(handle);
        ci->ptr    = 0;
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
      default:
#ifdef __FIXME__
        ROAR_DBG("ioctl(__fd=%i, __request=%lu (%s)) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request, nosys_reqname);
#else
        ROAR_DBG("ioctl(__fd=%i, __request=%lu) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request);
#endif
        errno = ENOSYS;
        return -1;
     }
    break;
   case HT_MIXER:
     return _ioctl_mixer(handle, __request, argp);
    break;
   default:
     ROAR_DBG("ioctl(__fd=%i, __request=%lu): unknown handle type: no ioctl()s supported", __fd, (long unsigned int) __request);
     ROAR_DBG("ioctl(__fd=%i, __request=%lu) = -1 // errno = ENOSYS", __fd, (long unsigned int) __request);
     errno = EINVAL;
     return -1;
    break;
  }
 }

#ifdef IOCTL_IS_ALIAS
 errno = ENOSYS;
 return -1;
#else
 return _os.ioctl(__fd, __request, argp);
#endif
}


// -------------------------------------
// emulated stdio functions follow:
// -------------------------------------

//roar_vio_to_stdio

FILE *fopen(const char *path, const char *mode) {
 struct pointer * pointer;
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
   break;
  case -1:       // pass error to caller
    return NULL;
   break;
  default:       // return successfully opened pointer to caller
    if ( (pointer = _get_pointer_by_fh(ret)) != NULL ) {
     if ( (fr = roar_vio_to_stdio(&(pointer->handle->stream_vio), flags)) == NULL ) {
      errno = EIO;
      return NULL;
     } else {
      return fr;
     }
    } else {
     errno = EIO;
     return NULL;
    }
   break;
 }

 return _os.fopen(path, mode);
}

#endif

//ll
