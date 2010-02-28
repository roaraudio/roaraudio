//driver_sout.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
#ifdef ROAR_HAVE_LIBSHOUT

int _driver_shout_usage_counter = 0;

int     driver_shout_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream) {
 char * s_server = NULL;
 char * s_mount  = NULL;
 char * s_user   = NULL;
 char * s_pw     = NULL;
 int    s_port   = -1;
 char * s_desc   = NULL;
 char * s_genre  = NULL;
 char * s_name   = NULL;
 char * s_url    = NULL;
 int    s_public = 0;
 char * a;
 shout_t * shout;

 switch (info->codec) {
  case ROAR_CODEC_DEFAULT:
    info->codec = ROAR_CODEC_OGG_VORBIS;
   break;
  case ROAR_CODEC_OGG_VORBIS:
  case ROAR_CODEC_OGG_SPEEX:
  case ROAR_CODEC_OGG_FLAC:
  case ROAR_CODEC_OGG_GENERAL:
    // ok, no errors here
   break;
  default:
    ROAR_ERR("This driver only supports Ogg/* (most common is Ogg/Vorbis), current codec is %s", roar_codec2str(info->codec));
    return -1;
   break;
 }

 if ( device != NULL ) {
  // device sould be an URL in this form:
  // [http[s]://][user[:pw]@]host[:port][/mp.ogg]

  if ( (a = strstr(device, "://")) != NULL ) {
   *a = 0;
   if ( strcmp(device, "http") ) {
    return -1;
   }
   device = a + 3;
  }

  // [user[:pw]@]host[:port][/mp.ogg]

  if ( (a = strstr(device, "@")) != NULL ) {
   *a = 0;
   s_user = device;
   device = a + 1;
  }

  if ( s_user != NULL ) {
   if ( (a = strstr(s_user, ":")) != NULL ) {
    *a = 0;
    s_pw = a+1;
   }
  }

  if ( s_user != NULL && ! *s_user )
   s_user = NULL;

  if ( s_pw != NULL && ! *s_pw )
   s_pw = NULL;

  // host[:port][/mp.ogg]

  if ( (a = strstr(device, "/")) != NULL ) {
   *a = 0;
   s_server = device;
   device = a + 1;
  } else {
   s_server  = device;
   device   += strlen(device);
  }

  if ( (a = strstr(s_server, ":")) != NULL ) {
   *a = 0;
   s_port = atoi(a+1);
  }

  if ( ! *s_server )
   s_server = NULL;

  // [/mp.ogg]

  if ( *device ) {
   s_mount = device;
  }
 }

 ROAR_DBG("driver_shout_open_vio(*): user='%s', pw='%s', server='%s', port=%i, mount='%s'",
                   s_user, s_pw, s_server, s_port, s_mount);

 if ( s_server == NULL )
  s_server = "localhost";

 if ( s_mount == NULL )
  s_mount  = "/roar.ogg";

 if ( s_pw == NULL )
  s_pw     = "hackme";

 if ( s_user == NULL )
  s_user     = "source";

 if ( s_port == -1 )
  s_port   = 8000;

 if ( _driver_shout_usage_counter++ == 0 )
  shout_init();

 if (!(shout = shout_new())) {
  ROAR_ERR("Can not clreate shout object");
  return 1;
 }

 if (shout_set_host(shout, s_server) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting hostname: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_protocol(shout, SHOUT_PROTOCOL_HTTP) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting protocol: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_port(shout, s_port) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting port: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_password(shout, s_pw) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting password: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_mount(shout, s_mount) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting mount: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_user(shout, s_user) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting user: %s", shout_get_error(shout));
  return 1;
 }

 if (shout_set_format(shout, SHOUT_FORMAT_OGG) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Error setting format: %s", shout_get_error(shout));
  return 1;
 }

 shout_set_public(shout, s_public);

 if (s_desc  != NULL)
  shout_set_description(shout, s_desc);

 if (s_genre != NULL)
  shout_set_genre(shout, s_genre);

 if (s_name  != NULL)
  shout_set_name(shout, s_name);

 if (s_url   != NULL)
  shout_set_url(shout, s_url);

 if (shout_open(shout) != SHOUTERR_SUCCESS) {
  ROAR_ERR("Can not open connection via libshout!");
  return -1;
 }

 memset(inst, 0, sizeof(struct roar_vio_calls));
 inst->inst  = (void*)shout;
 inst->write = driver_shout_write;
 inst->close = driver_shout_close;

 return 0;
}

int     driver_shout_close(struct roar_vio_calls * vio) {

 shout_close((shout_t *)vio->inst);

 if ( _driver_shout_usage_counter-- == 1 )
  shout_shutdown();

 return 0;
}

ssize_t driver_shout_write(struct roar_vio_calls * vio, void *buf, size_t count) {
 if (shout_send((shout_t *)vio->inst, (unsigned char*)buf, count) != SHOUTERR_SUCCESS)
  return -1;

 return count;
}

#endif
//ll
