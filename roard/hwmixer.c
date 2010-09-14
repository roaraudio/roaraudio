//hwmixer.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#define FLAG_NONE     0x0000
#define FLAG_FHSEC    0x0001

struct hwmixer {
 const char * name;
 const char * desc;
 const char * devs;
 int flags;
 int (*open)(struct hwmixer_stream * stream, char * drv, char * dev, int fh, char * basename, struct roar_keyval * subnames, size_t subnamelen);
 int (*close)(struct hwmixer_stream * stream);
 int (*set_vol)(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings);
 int (*get_vol)(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings);
};

static int __true (void) { return 0; }

struct hwmixer g_hwmixers[] = {
#if defined(ROAR_HAVE_OSS_BSD) || defined(ROAR_HAVE_OSS)
 {"oss",  "OSS Mixer", "/dev/mixer*", FLAG_FHSEC, hwmixer_oss_open, hwmixer_oss_close, hwmixer_oss_set_vol, hwmixer_oss_get_vol},
#endif
 {"dstr", "Write to DSTR",       "/some/file", FLAG_NONE,  hwmixer_dstr_open, hwmixer_dstr_close, hwmixer_dstr_set_vol, NULL},
 {"null", "Null Mixer",          NULL, FLAG_NONE,  (int (*)(struct hwmixer_stream * stream, char * drv, char * dev, int fh, char * basename, struct roar_keyval * subnames, size_t subnamelen))__true, (int (*)(struct hwmixer_stream * stream))__true, (int (*)(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings))__true, (int (*)(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings))__true},
 {NULL,   NULL, NULL, FLAG_NONE, NULL, NULL, NULL, NULL}
};

void print_hwmixerlist (void) {
 struct hwmixer * mixer;
 int i;

 printf("  Source   Flag Subsys - Description (devices)\n");
 printf("------------------------------------------------------\n");

 for (i = 0; (mixer = &(g_hwmixers[i]))->name != NULL; i++) {
  printf("  %-9s %c   Mixer  - %s (devices: %s)\n",
         mixer->name,
         mixer->flags & FLAG_FHSEC ? 's' : ' ',
         mixer->desc,
         mixer->devs == NULL ? "(none)" : mixer->devs
        );
 }
}

void hwmixer_setup_info(struct hwmixer_stream * mstream) {
 struct roar_stream_server * ss;

 streams_get(mstream->stream, &ss);

 memset(&(ROAR_STREAM(ss)->info), 0, sizeof(ROAR_STREAM(ss)->info));

 streams_set_dir(mstream->stream, ROAR_DIR_MIXING, 1);

 streams_set_flag(mstream->stream, ROAR_FLAG_HWMIXER);
}

int hwmixer_open(int basestream, char * drv, char * dev, int fh, char * basename, char * subnames) {
 struct roar_stream_server * ss;
 struct roar_keyval * subnamekv = NULL;
 struct hwmixer * mixer = NULL;
 struct hwmixer_stream * stream;
 ssize_t subnamekvlen = 0;
 int i;
 int ret;

 for (i = 0; g_hwmixers[i].name != NULL; i++) {
  if ( !strcmp(g_hwmixers[i].name, drv) ) {
   mixer = &(g_hwmixers[i]);
   break;
  }
 }

 if ( mixer == NULL ) {
  ROAR_WARN("hwmixer_open(basestream=%i, drv='%s', dev='%s', fh=%i, basename='%s', subnames='%s'): Driver not found.", basestream, drv, dev, fh, basename, subnames);
  return -1;
 }

 if ( mixer->open == NULL ) {
  return -1;
 }

 if ( fh != -1 && !(mixer->flags & FLAG_FHSEC) ) {
  return -1;
 }

 stream = roar_mm_malloc(sizeof(struct hwmixer_stream));
 if ( stream == NULL )
  return -1;

 memset(stream, 0, sizeof(struct hwmixer_stream));

 stream->hwmixer    = mixer;
 stream->basestream = basestream;
 stream->stream     = basestream;
 stream->baseud     = NULL;
 stream->ud         = NULL;

 if ( basename == NULL ) {
  streams_set_name(basestream, "Hardware Mixer");
 } else {
  streams_set_name(basestream, basename);
 }

 hwmixer_setup_info(stream);

 if ( subnames != NULL ) {
  subnamekvlen = roar_keyval_split(&subnamekv, subnames, ",;", "=:", 0);
 }

 ret = mixer->open(stream, drv, dev, fh, basename, subnamekv, subnamekvlen);

 if ( subnamekv != NULL )
  roar_mm_free(subnamekv);

 if ( ret == -1 ) {
  roar_mm_free(stream);
  return -1;
 }

 streams_set_mixerstream(basestream, stream);

 // try to get in sync with HW mixer.
 // if possible read hw mixer state.
 // if not possible write to force a sync value.
 if ( streams_get(basestream, &ss) == 0 ) {
  if ( mixer->get_vol != NULL ) {
   hwmixer_get_volume(basestream, ss, stream, &(ss->mixer));
  } else {
   hwmixer_set_volume(basestream, ss, stream, &(ss->mixer));
  }
 }

 return 0;
}

int hwmixer_close(int stream) {
 struct hwmixer_stream * mstream = streams_get_mixerstream(stream);

 if ( mstream == NULL )
  return 0;

 if ( mstream->hwmixer->close != NULL )
  mstream->hwmixer->close(mstream);

 roar_mm_free(mstream);

 return 0;
}

int hwmixer_set_volume(int id, struct roar_stream_server * ss, struct hwmixer_stream * mstream, struct roar_mixer_settings *
settings) {
 if ( mstream->hwmixer->set_vol != NULL )
  return mstream->hwmixer->set_vol(mstream, ROAR_STREAM(ss)->info.channels, HWMIXER_MODE_SET, settings);

 return 0;
}

int hwmixer_get_volume(int id, struct roar_stream_server * ss, struct hwmixer_stream * mstream, struct roar_mixer_settings *
settings) {
 if ( mstream->hwmixer->get_vol != NULL )
  return mstream->hwmixer->get_vol(mstream, ROAR_STREAM(ss)->info.channels, HWMIXER_MODE_ASK, settings);

 return 0;
}

struct hwmixer_stream * hwmixer_substream_new(struct hwmixer_stream * parent) {
 struct roar_stream_server * ss;
 struct hwmixer_stream * stream;
 int id;

 ROAR_DBG("hwmixer_substream_new(parent=%p) = ?", parent);

 if ( parent == NULL )
  return NULL;

 ROAR_DBG("hwmixer_substream_new(parent=%p) = ?", parent);

 stream = roar_mm_malloc(sizeof(struct hwmixer_stream));
 if ( stream == NULL )
  return NULL;

 ROAR_DBG("hwmixer_substream_new(parent=%p) = ?", parent);

 if ( (id = streams_new_virtual(parent->basestream, &ss)) == -1 ) {
  roar_mm_free(stream);
  return NULL;
 }

 memset(stream, 0, sizeof(struct hwmixer_stream));

 stream->hwmixer    = parent->hwmixer;
 stream->basestream = parent->basestream;
 stream->stream     = id;
 stream->baseud     = parent->baseud;
 stream->ud         = NULL;

 hwmixer_setup_info(stream);

 streams_set_mixerstream(id, stream);

 ROAR_DBG("hwmixer_substream_new(parent=%p) = %p", parent, stream);

 return stream;
}

//ll
