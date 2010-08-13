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

static snd_pcm_format_t driver_alsa_roar_fmt_to_alsa(struct roar_audio_info * info) {
 snd_pcm_format_t format = SND_PCM_FORMAT_UNKNOWN;

 switch (info->codec) {
  case ROAR_CODEC_PCM_S_LE:
    switch (info->bits) {
     case  8: format = SND_PCM_FORMAT_S8;      break;
     case 16: format = SND_PCM_FORMAT_S16_LE;  break;
     case 24: format = SND_PCM_FORMAT_S24_3LE; break;
     case 32: format = SND_PCM_FORMAT_S32_LE;  break;
    }
   break;
  case ROAR_CODEC_PCM_S_BE:
    switch (info->bits) {
     case  8: format = SND_PCM_FORMAT_S8;      break;
     case 16: format = SND_PCM_FORMAT_S16_BE;  break;
     case 24: format = SND_PCM_FORMAT_S24_3BE; break;
     case 32: format = SND_PCM_FORMAT_S32_BE;  break;
    }
   break;
  case ROAR_CODEC_PCM_U_LE:
    switch (info->bits) {
     case  8: format = SND_PCM_FORMAT_U8;      break;
     case 16: format = SND_PCM_FORMAT_U16_LE;  break;
     case 24: format = SND_PCM_FORMAT_U24_3LE; break;
     case 32: format = SND_PCM_FORMAT_U32_LE;  break;
    }
   break;
  case ROAR_CODEC_PCM_U_BE:
    switch (info->bits) {
     case  8: format = SND_PCM_FORMAT_U8;      break;
     case 16: format = SND_PCM_FORMAT_U16_BE;  break;
     case 24: format = SND_PCM_FORMAT_U24_3BE; break;
     case 32: format = SND_PCM_FORMAT_U32_BE;  break;
    }
   break;
  case ROAR_CODEC_ALAW:
    format = SND_PCM_FORMAT_A_LAW;
   break;
  case ROAR_CODEC_MULAW:
    format = SND_PCM_FORMAT_MU_LAW;
   break;
  case ROAR_CODEC_GSM:
    format = SND_PCM_FORMAT_GSM;
   break;
 }

 return format;
}

static int driver_alsa_set_params(roar_alsa_t * device) {
 unsigned int      buffer_time = 32000; // 32ms buffer if possible.
                                        // *_buffer_time_near() will pick something as close as possible.
 snd_pcm_uframes_t frames      = ROAR_OUTPUT_BUFFER_SAMPLES;
 int               rc;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_malloc(&device->params) < 0 )
  return -1;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_any(device->handle, device->params) < 0 )
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_set_access(device->handle, device->params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0 )
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_set_format(device->handle, device->params, device->format) < 0)
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 ROAR_DBG("driver_alsa_set_params(device=%p): device->info.channels=%i", device, device->info.channels);

 if ( snd_pcm_hw_params_set_channels(device->handle, device->params, device->info.channels) < 0 )
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_set_rate(device->handle, device->params, device->info.rate, 0) < 0 )
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_set_buffer_time_near(device->handle, device->params, &buffer_time, NULL) < 0 )
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 if ( snd_pcm_hw_params_set_period_size_near(device->handle, device->params, &frames, NULL) < 0 )
  goto error;

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 rc = snd_pcm_hw_params(device->handle, device->params);
 if (rc < 0) {
  ROAR_ERR("driver_alsa_open_vio(*): unable to set hw parameters: %s", snd_strerror(rc));
  goto error;
 }

 ROAR_DBG("driver_alsa_set_params(device=%p) = ?", device);

 snd_pcm_hw_params_free(device->params);

 ROAR_DBG("driver_alsa_set_params(device=%p) = 0", device);

 return 0;

error:
 snd_pcm_hw_params_free(device->params);
 ROAR_DBG("driver_alsa_set_params(device=%p) = -1", device);
 return -1;
}

int driver_alsa_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 roar_alsa_t     * interface;
 char            * alsa_dev;
 int               rc;
 int               autoconf;

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = ?", inst, device, info, fh, sstream);

 if ( fh != -1 )
  return -1;

 if ( (interface = roar_mm_calloc(1, sizeof(roar_alsa_t))) == NULL )
  return -1;

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = ?", inst, device, info, fh, sstream);

 if ( device == NULL ) {
  alsa_dev = "default";
 } else {
  alsa_dev = device;
 }

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = ?", inst, device, info, fh, sstream);

 rc = snd_pcm_open(&(interface->handle), alsa_dev, SND_PCM_STREAM_PLAYBACK, 0); 
 if ( rc < 0 ) {
  ROAR_ERR("driver_alsa_open_vio(*): Unable to open PCM device: %s", snd_strerror(rc));
  roar_mm_free(interface);
  return -1;
 }

 // Setting sample format, yay.
 interface->format = driver_alsa_roar_fmt_to_alsa(info);

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = ?", inst, device, info, fh, sstream);

 // We did not find a codec
 if ( interface->format == SND_PCM_FORMAT_UNKNOWN ) {
  autoconf = streams_get_flag(ROAR_STREAM(sstream)->id, ROAR_FLAG_AUTOCONF);
  if ( autoconf == -1 ) {
   ROAR_ERR("driver_alsa_open_vio(*): Could not get autoconf flag.");
   goto init_error;
  }

  if ( autoconf ) {
   info->bits  = 16;

#if   ROAR_CODEC_DEFAULT == ROAR_CODEC_PCM_S_LE
   interface->format = SND_PCM_FORMAT_S16_LE;
   info->codec       = ROAR_CODEC_PCM_S_LE;
#elif ROAR_CODEC_DEFAULT == ROAR_CODEC_PCM_S_BE
   interface->format = SND_PCM_FORMAT_S16_BE;
   info->codec       = ROAR_CODEC_PCM_S_BE;
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

 memcpy(&(interface->info), info, sizeof(struct roar_audio_info));

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = ?", inst, device, info, fh, sstream);

 if ( driver_alsa_set_params(interface) < 0 )
  goto init_error;

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = ?", inst, device, info, fh, sstream);

 memset(inst, 0, sizeof(struct roar_vio_calls));

 inst->inst  = (void*) interface;
 inst->write = driver_alsa_write;
 inst->close = driver_alsa_close;
 inst->sync  = driver_alsa_sync;
 inst->ctl   = driver_alsa_ctl;

 ROAR_DBG("driver_alsa_open_vio(inst=%p, device='%s', info=%p, fh=%i, sstream=%p) = 0", inst, device, info, fh, sstream);

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
 char*   mutable_buf = (char*)buf;
 ssize_t have        = 0;
 ssize_t ret;

 while (size) {
  ret = driver_alsa_write_int(vio, mutable_buf, size);
  if ( ret < 0 )
   return -1;

  have        += ret;
  mutable_buf += ret;
  size        -= ret;
 }

 return have;
}

int driver_alsa_sync(struct roar_vio_calls * vio) {
 roar_alsa_t       * device    = vio->inst;
 return snd_pcm_drain(device->handle);
}

/* Stop the driver, set new params, and restart */
/* Not been able to test this function. */
/* Assuming that params can be reset after halting the PCM device */
static int driver_alsa_reopen_device(roar_alsa_t * device) {

 if ( snd_pcm_drop(device->handle) < 0 ) {
  ROAR_ERR("driver_alsa_reopen_device(*): snd_pcm_drop() failed.");
  return -1;
 }

 if ( driver_alsa_set_params(device) < 0 ) {
  ROAR_ERR("driver_alsa_reopen_device(*): driver_alsa_set_params() failed.");
  return -1;
 }

 return 0;
}

/* Not completely tested.
 * Tested:
 * ROAR_VIO_CTL_GET_DELAY
 * ROAR_VIO_CTL_SET_SSTREAMID
 * ROAR_VIO_CTL_SET_SSTREAM
 */

int driver_alsa_ctl(struct roar_vio_calls * vio, int cmd, void * data) {
 roar_alsa_t       * device    = vio->inst;
 snd_pcm_sframes_t alsa_delay;

 ROAR_DBG("driver_alsa_ctl(vio=%p, cmd=0x%.8x, data=%p) = ?", vio, cmd, data);

 switch (cmd) {
  case ROAR_VIO_CTL_GET_DELAY:
   if ( snd_pcm_delay(device->handle, &alsa_delay) < 0 )
    return -1;
   else {
    *(uint_least32_t *)data = snd_pcm_frames_to_bytes(device->handle, alsa_delay);
    ROAR_DBG("driver_alsa_ctl(vio=%p, cmd=ROAR_VIO_CTL_GET_DELAY(0x%.8x), data=%p): %i bytes.", vio, cmd, data, (int)(*(uint_least32_t*)data));
   }
   break;

  case ROAR_VIO_CTL_SET_SSTREAMID:
   device->ssid = *(int *)data;
   ROAR_DBG("driver_alsa_ctl(vio=%p, cmd=ROAR_VIO_CTL_SET_SSTREAMID(0x%.8x), data=%p): ssid=%i", vio, cmd, data, device->ssid);
   break;

  case ROAR_VIO_CTL_SET_SSTREAM:
   device->sstream = data;
   ROAR_DBG("driver_alsa_ctl(vio=%p, cmd=ROAR_VIO_CTL_SET_SSTREAM(0x%.8x), data=%p): sstream=%p", vio, cmd, data, device->sstream);
   break;

  case ROAR_VIO_CTL_GET_AUINFO:
   ROAR_DBG("driver_alsa_ctl(vio=%p, cmd=ROAR_VIO_CTL_GET_AUINFO(0x%.8x), data=%p) = ?", vio, cmd, data);
   memcpy(data, &(device->info), sizeof(struct roar_audio_info));
   break;

  case ROAR_VIO_CTL_SET_AUINFO:
   ROAR_DBG("driver_alsa_ctl(vio=%p, cmd=ROAR_VIO_CTL_SET_AUINFO(0x%.8x), data=%p) = ?", vio, cmd, data);
   memcpy(&(device->info), data, sizeof(struct roar_audio_info));
   return driver_alsa_reopen_device(device);

  default:
   return -1;
 }

 return 0;
}

#endif

