//hwmixer_oss.c:

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

#define OSS_VOLUME_SCALE 100

struct subdev {
 int bit;
 int channels;
 long long int cmd_read, cmd_write;
};

static struct subdev_map {
 const char * name;
 struct subdev subdev;
} g_subdevs[] = {
 {"Volume", {.bit = SOUND_MASK_VOLUME, .cmd_read = SOUND_MIXER_READ_VOLUME, .cmd_write = SOUND_MIXER_WRITE_VOLUME}},
 {NULL}
};

static int hwmixer_oss_open_stream(struct hwmixer_stream * stream, int devmask, int sdevmask, char * basename, struct roar_keyval * subname) {
 struct subdev * subdev;
 struct subdev * source_subdev = NULL;
 struct roar_stream_server * ss;
 int i;

 for (i = 0; g_subdevs[i].name != NULL; i++) {
  if ( !strcasecmp(subname->key, g_subdevs[i].name) ) {
   source_subdev = &(g_subdevs[i].subdev);
   break;
  }
 }

 if ( source_subdev == NULL )
  return -1;

 if ( !(devmask & source_subdev->bit) )
  return -1;

 subdev = roar_mm_malloc(sizeof(struct subdev));

 if ( subdev == NULL )
  return -1;

 memcpy(subdev, source_subdev, sizeof(struct subdev));

 subdev->channels = sdevmask & subdev->bit ? 2 : 1;

 stream->ud     = subdev;

 if (streams_get(stream->stream, &ss) != -1) {
  ROAR_STREAM(ss)->info.channels = subdev->channels;
 } else {
  ROAR_WARN("hwmixer_dstr_open(*): can not get object for stream %i", stream->stream);
 }

 return 0;
}

int hwmixer_oss_open(struct hwmixer_stream * stream, char * drv, char * dev, int fh, char * basename, struct roar_keyval * subnames, size_t subnamelen) {
 struct roar_vio_calls * vio = roar_mm_malloc(sizeof(struct roar_vio_calls));
 struct roar_vio_defaults def;
 struct roar_vio_sysio_ioctl ctl;
 struct roar_keyval kv;
 int devmask, sdevmask;

 if ( vio == NULL ) {
  return -1;
 }

 if ( fh == -1 ) {
#ifdef ROAR_DEFAULT_OSS_MIX_DEV
  if ( dev == NULL ) {
   dev = ROAR_DEFAULT_OSS_MIX_DEV;
  }
#endif

  if ( dev == NULL ) {
   roar_mm_free(vio);
   return -1;
  }

  if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_FILE, O_RDWR, 0644) == -1 ) {
   roar_mm_free(vio);
   return -1;
  }

  if ( roar_vio_open_dstr(vio, dev, &def, 1) == -1 ) {
   roar_mm_free(vio);
   return -1;
  }
 } else {
  if ( roar_vio_open_fh(vio, fh) == -1 ) {
   roar_mm_free(vio);
   return -1;
  }
 }

 stream->baseud = vio;

 ctl.cmd  = SOUND_MIXER_READ_DEVMASK;
 ctl.argp = &devmask;

 if ( roar_vio_ctl(vio, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 ) {
  roar_vio_close(vio);
  roar_mm_free(vio);
  return -1;
 }

 ctl.cmd  = SOUND_MIXER_READ_STEREODEVS;
 ctl.argp = &sdevmask;

 if ( roar_vio_ctl(vio, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 ) {
  sdevmask = 0;
 }

 kv.key   = "Volume";
 kv.value = NULL;

 if ( hwmixer_oss_open_stream(stream, devmask, sdevmask, basename, &kv) == -1 ) {
  roar_vio_close(vio);
  roar_mm_free(vio);
  return -1;
 }


#ifdef TEST_HWMIXER_SUBSTREAMS
 stream = hwmixer_substream_new(stream);
 if ( stream == NULL ) {
  ROAR_WARN("hwmixer_dstr_open(*): can not create substream");
 } else {
  if (streams_get(stream->stream, &ss) != -1) {
   ROAR_STREAM(ss)->info.channels = 2;
  } else {
   ROAR_WARN("hwmixer_dstr_open(*): can not get object for stream %i", stream->stream);
  }
 }
#endif

 return 0;
}

int hwmixer_oss_close(struct hwmixer_stream * stream) {
 // are we a substream? if yes we do not clean up anything.
 // streams_delete() will do all our work.

 roar_mm_free(stream->ud);

 if ( stream->stream != stream->basestream )
  return 0;

 roar_vio_close(stream->baseud);
 roar_mm_free(stream->baseud);
 return 0;
}

int hwmixer_oss_set_vol(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings) {
 struct roar_vio_calls * vio = stream->baseud;
 struct roar_vio_sysio_ioctl ctl;
 struct subdev         * subdev = stream->ud;
 int i;
 int l, r;

 if ( channels == 0 )
  return 0;

 if ( channels == 1 ) {
  l = r = settings->mixer[0];
 } else {
  l = settings->mixer[0];
  r = settings->mixer[1];
 }

 if ( subdev->channels == 1 ) {
  l = r = (l + r) / 2;
 }

 l = (l * OSS_VOLUME_SCALE) / settings->scale;
 r = (r * OSS_VOLUME_SCALE) / settings->scale;

 i = (l & 0xFF) | ((r & 0xFF) << 8); 

 ctl.cmd = subdev->cmd_write;
 ctl.argp = &i;

 return roar_vio_ctl(vio, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl);
}

int hwmixer_oss_get_vol(struct hwmixer_stream * stream, int channels, int mode, struct roar_mixer_settings * settings) {
 struct roar_vio_calls * vio = stream->baseud;
 struct roar_vio_sysio_ioctl ctl;
 struct subdev         * subdev = stream->ud;
 int i;
 int l, r;

 ctl.cmd = subdev->cmd_read;
 ctl.argp = &i;

 if ( roar_vio_ctl(vio, ROAR_VIO_CTL_SYSIO_IOCTL, &ctl) == -1 )
  return -1;

 l =  i       & 0xFF;
 r = (i >> 8) & 0xFF;

 if ( subdev->channels == 1 )
  r = l;

 settings->scale    = OSS_VOLUME_SCALE;

 if ( channels == 1 ) {
  settings->mixer[0] = (l + r) / 2;
 } else if ( channels == 2 ) {
  settings->mixer[0] = l;
  settings->mixer[1] = r;
 } else {
  return -1;
 }

 return 0;
}

//ll
