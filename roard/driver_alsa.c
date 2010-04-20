//driver_alsa.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *      Copyright (C) Hans-Kristian 'maister' Arntzen - 2010
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

#ifdef ROAR_HAVE_LIBASOUND

#define ALSA_PCM_NEW_HW_PARAMS_API

typedef struct roar_alsa {
 snd_pcm_t *handle;
 snd_pcm_hw_params_t* params;
} roar_alsa_t;

int driver_alsa_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 roar_alsa_t     * interface;
 char            * alsa_dev;
 int               rc;
 unsigned          buffer_time = 200000; // 200ms buffer
 snd_pcm_uframes_t frames      = 256; //ROAR_OUTPUT_BUFFER_SAMPLES; //256; // ALSA frames
 snd_pcm_format_t  format      = SND_PCM_FORMAT_UNKNOWN; // If this isn't set further down, we pick 16 bit audio if we're using autoconf.
 uint16_t          channels    = info->channels;
 uint32_t          samplerate  = info->rate;
 int               autoconf;


 if ( fh != -1 )
  return -1;

 if ( (interface = roar_mm_calloc(1, sizeof(roar_alsa_t))) == NULL )
  return -1;

 if ( device == NULL ) {
  alsa_dev = "default";
 } else {
  alsa_dev = device;
 }

 rc = snd_pcm_open(&(interface->handle), alsa_dev, SND_PCM_STREAM_PLAYBACK, 0); 
 if ( rc < 0 ) {
  ROAR_ERR("driver_alsa_open_vio(*): Unable to open PCM device: %s", snd_strerror(rc));
  roar_mm_free(interface);
  return -1;
 }

 // Setting sample format, yay.
 if ( info->codec == ROAR_CODEC_PCM_S_LE ) {
  switch (info->bits) {
   case  8:
     format = SND_PCM_FORMAT_S8;
    break;
   case 16:
     format = SND_PCM_FORMAT_S16_LE;
    break;
   case 32:
     format = SND_PCM_FORMAT_S32_LE;
    break;
  }
 } else if ( info->codec == ROAR_CODEC_PCM_U_LE ) {
  switch (info->bits) {
   case  8:
     format = SND_PCM_FORMAT_U8;
    break;
   case 16:
     format = SND_PCM_FORMAT_U16_LE;
    break;
   case 32:
     format = SND_PCM_FORMAT_U32_LE;
    break;
  }
 } else if ( info->codec == ROAR_CODEC_PCM_S_BE ) {
  switch (info->bits) {
   case  8:
     format = SND_PCM_FORMAT_S8;
    break;
   case 16:
     format = SND_PCM_FORMAT_S16_BE;
    break;
   case 32:
     format = SND_PCM_FORMAT_S32_BE;
    break;
  }
 } else if ( info->codec == ROAR_CODEC_PCM_U_BE ) {
  switch (info->bits) {
   case  8:
     format = SND_PCM_FORMAT_U8;
    break;
   case 16:
     format = SND_PCM_FORMAT_U16_BE;
    break;
   case 32:
     format = SND_PCM_FORMAT_U32_BE;
    break;
  }
 }

 // We did not find a codec
 if ( format == SND_PCM_FORMAT_UNKNOWN ) {
  autoconf = streams_get_flag(ROAR_STREAM(sstream)->id, ROAR_FLAG_AUTOCONF);
  if ( autoconf == -1 ) {
   ROAR_ERR("driver_alsa_open_vio(*): Could not get autoconf flag.");
   goto init_error;
  }

  if ( autoconf ) {
   info->bits  = 16;

#if   ROAR_CODEC_DEFAULT == ROAR_CODEC_PCM_S_LE
   format      = SND_PCM_FORMAT_S16_LE;
   info->codec = ROAR_CODEC_PCM_S_LE;
#elif ROAR_CODEC_DEFAULT == ROAR_CODEC_PCM_S_BE
   format      = SND_PCM_FORMAT_S16_BE;
   info->codec = ROAR_CODEC_PCM_S_BE;
#else
   ROAR_ERR("driver_alsa_open_vio(*): ALSA only support little- or big-endian sample format. Please select -oO codec=pcm_s_le or pcm_s_be");
   goto init_error;
#endif
   ROAR_WARN("driver_alsa_open_vio(*): Supplied codec type not available. Using 16 bit sample format.");
  } else {
   ROAR_ERR("driver_alsa_open_vio(*): can not set requested codec, set codec manually with -oO codec=<codec>.");
   goto init_error;
  }
 }


 if ( snd_pcm_hw_params_malloc(&interface->params) < 0 )
  goto init_error;

 if ( snd_pcm_hw_params_any(interface->handle, interface->params) < 0 )
  goto init_error;
 if ( snd_pcm_hw_params_set_access(interface->handle, interface->params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0 )
  goto init_error;
 if ( snd_pcm_hw_params_set_format(interface->handle, interface->params, format) < 0)
  goto init_error;
 if ( snd_pcm_hw_params_set_channels(interface->handle, interface->params, channels) < 0 )
  goto init_error;
 if ( snd_pcm_hw_params_set_rate_near(interface->handle, interface->params, &samplerate, NULL) < 0 )
  goto init_error;
 if ( snd_pcm_hw_params_set_buffer_time_near(interface->handle, interface->params, &buffer_time, NULL) < 0 )
  goto init_error; 
 if ( snd_pcm_hw_params_set_period_size_near(interface->handle, interface->params, &frames, NULL) < 0 )
  goto init_error;

 rc = snd_pcm_hw_params(interface->handle, interface->params);
 if (rc < 0) {
  ROAR_ERR("driver_alsa_open_vio(*): unable to set hw parameters: %s", snd_strerror(rc));
  snd_pcm_hw_params_free(interface->params);
  goto init_error;
 }

 snd_pcm_hw_params_free(interface->params);

 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->inst  = (void*) interface;
 inst->write = driver_alsa_write;
 inst->close = driver_alsa_close;

 return 0;

init_error:
 snd_pcm_close(interface->handle);
 roar_mm_free(interface);
 return -1;
}

int driver_alsa_close(struct roar_vio_calls * vio) {
 roar_alsa_t * device = vio->inst;

 if ( device->handle != NULL ) {
  snd_pcm_drain(device->handle);
  snd_pcm_close(device->handle);
 }

 roar_mm_free(device);

 return 0;
}

static ssize_t driver_alsa_write_int(struct roar_vio_calls * vio, void *buf, size_t size) {
 roar_alsa_t       * device    = vio->inst;
 snd_pcm_sframes_t  write_size = snd_pcm_bytes_to_frames(device->handle, size);
 snd_pcm_sframes_t  rc;

 ROAR_DBG("driver_alsa_write(vio=%p, buf=%p, size=%llu) = ?", vio, buf, (long long unsigned int)size);

 rc = snd_pcm_writei(device->handle, buf, write_size);

 if ( rc == -EPIPE || rc == -EINTR || rc == -ESTRPIPE ) {
  if ( snd_pcm_recover(device->handle, rc, 1) < 0 )
   return -1;
  return size;
 } else if (rc < 0) {
  ROAR_ERR("driver_alsa_write(*): Error from snd_pcm_writei(*): %s", snd_strerror(rc));
  return -1;
 }  

 return snd_pcm_frames_to_bytes(device->handle, rc);
}

ssize_t driver_alsa_write(struct roar_vio_calls * vio, void *buf, size_t size) {
 ssize_t have = 0;
 ssize_t ret;

 while (size) {
  ret = driver_alsa_write_int(vio, buf, size);
  if ( ret < 0 )
   return -1;

  have += ret;
  buf  += ret;
  size -= ret;
 }

 return have;
}

#endif

