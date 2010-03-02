//roar.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of the XMMS RoarAudio output plugin a part of RoarAudio,
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

#include "all.h"

OutputPlugin roar_op = {
        NULL,
        NULL,
        "RoarAudio XMMS Plugin", /* Description */
        roar_init,
        roar_about,
        roar_configure,
        roar_get_volume,
        roar_set_volume,
        roar_open,
        roar_write,
        roar_close,
        roar_flush,
        roar_pause,
        roar_free,
        roar_playing,
        roar_get_output_time,
        roar_get_written_time,
};

OutputPlugin *get_oplugin_info(void) {
 return &roar_op;
}

void roar_init(void) {
 ConfigFile * cfgfile;

 cfgfile = xmms_cfg_open_default_file();

 g_inst.state = 0;
 g_inst.server = NULL;
 g_inst.session = ctrlsocket_get_session_id();
 g_inst.mixer.l = -1;
 g_inst.mixer.r = -1;

 xmms_cfg_read_string(cfgfile, "ROAR", "server", &g_inst.server);

 xmms_cfg_read_string(cfgfile, "ROAR", "player_name", &g_inst.cfg.player_name);

 xmms_cfg_free(cfgfile);

 if ( g_inst.cfg.player_name == NULL )
  g_inst.cfg.player_name = "XMMS";

 ROAR_DBG("roar_init(*) = (void)");
}

int roar_playing(void) {
 return FALSE;
}

void roar_write(void *ptr, int length) {
 ssize_t r;

 if ( g_inst.pause )
  return;

 ROAR_DBG("roar_write(ptr=%p, length=%i) = (void)", ptr, length);

 while (length) {
  if ( (r = roar_vio_write(&(g_inst.vio), ptr, length >= 1764*2 ? 1764*2 : length)) != -1 ) {
   g_inst.written   += r;
   ptr              += r;
   length           -= r;
  } else {
   return;
  }
 }
}

int roar_open(AFormat fmt, int rate, int nch) {
 int codec = ROAR_CODEC_DEFAULT;
 int bits;

 if ( !(g_inst.state & STATE_CONNECTED) ) {
  if ( roar_simple_connect(&(g_inst.con), g_inst.server, g_inst.cfg.player_name) == -1 ) {
   return FALSE;
  }
  g_inst.state |= STATE_CONNECTED;
 }

  bits = 16;
  switch (fmt) {
   case FMT_S8:
     bits = 8;
     codec = ROAR_CODEC_DEFAULT;
    break;
   case FMT_U8:
     bits = 8;
     codec = ROAR_CODEC_PCM_U_LE; // _LE, _BE, _PDP,... all the same for 8 bit output
    break;
   case FMT_U16_LE:
     codec = ROAR_CODEC_PCM_U_LE;
    break;
   case FMT_U16_BE:
     codec = ROAR_CODEC_PCM_U_BE;
    break;
   case FMT_U16_NE:
#if BYTE_ORDER == BIG_ENDIAN
     codec = ROAR_CODEC_PCM_U_BE;
#elif BYTE_ORDER == LITTLE_ENDIAN
     codec = ROAR_CODEC_PCM_U_LE;
#else
     codec = ROAR_CODEC_PCM_U_PDP;
#endif
    break;
   case FMT_S16_LE:
     codec = ROAR_CODEC_PCM_S_LE;
    break;
   case FMT_S16_BE:
     codec = ROAR_CODEC_PCM_S_BE;
    break;
   case FMT_S16_NE:
     codec = ROAR_CODEC_DEFAULT;
    break;
 }

 g_inst.bps       = nch * rate * bits / 8;

 roar_close();

 if ( roar_vio_simple_new_stream_obj(&(g_inst.vio), &(g_inst.con), &(g_inst.stream),
                                     rate, nch, bits, codec, ROAR_DIR_PLAY) == -1 ) {
  roar_disconnect(&(g_inst.con));
  g_inst.state |= STATE_CONNECTED;
  g_inst.state -= STATE_CONNECTED;
  if ( !(g_inst.state & STATE_NORECONNECT) ) {
   g_inst.state |= STATE_NORECONNECT;
   xmms_usleep(100000);
   return roar_open(fmt, rate, nch);
  } else {
   g_inst.state -= STATE_NORECONNECT;
   return FALSE;
  }
 }
 g_inst.state |= STATE_PLAYING;

 g_inst.written = 0;
 g_inst.pause   = 0;

 roar_update_metadata();
 roar_set_volume(g_inst.mixer.l, g_inst.mixer.r);

 return TRUE;
}

void roar_close(void) {
 int was_playing = g_inst.state & STATE_PLAYING;

 g_inst.state |= STATE_PLAYING;
 g_inst.state -= STATE_PLAYING;

 if ( was_playing ) {
  roar_vio_close(&(g_inst.vio));
  xmms_usleep(10000); // give the server 10ms of extra time
 }

 g_inst.written = 0;
 ROAR_DBG("roar_close(void) = (void)");
}

void roar_pause(short p) {
 g_inst.pause = p;
}

int roar_free(void) {
 if ( g_inst.pause )
  return 0;
 else
  return 1000000; // ???
}

void roar_flush(int time) {
 gint64 r = time;

 r *= g_inst.bps;
 r /= 1000;

 g_inst.written = r;
}

int roar_get_output_time(void) {
 return roar_get_written_time();
}

int roar_get_written_time(void) {
 gint64 r;

 if ( !g_inst.bps ) {
  ROAR_DBG("roar_get_written_time(void) = 0");
  return 0;
 }

 r  = g_inst.written;
 r *= 1000; // sec -> msec
 r /= g_inst.bps;
 ROAR_DBG("roar_get_written_time(void) = %lu", r);

 return r;
}


// META DATA:

int roar_update_metadata(void) {
 struct roar_meta   meta;
 char empty = 0;
 char * info;
 char * delm;
 int pos;

 pos     = xmms_remote_get_playlist_pos(g_inst.session);

 meta.value = &empty;
 meta.key[0] = 0;
 meta.type = ROAR_META_TYPE_NONE;

 roar_stream_meta_set(&(g_inst.con), &(g_inst.stream), ROAR_META_MODE_CLEAR, &meta);

 info = xmms_remote_get_playlist_file(g_inst.session, pos);

 if ( info ) {
  if ( strncmp(info, "http:", 5) == 0 )
   meta.type = ROAR_META_TYPE_FILEURL;
  else
   meta.type = ROAR_META_TYPE_FILENAME;

  meta.value = info;
  ROAR_DBG("roar_update_metadata(*): setting meta data: type=%i, strlen(value)=%i", meta.type, strlen(info));
  roar_stream_meta_set(&(g_inst.con), &(g_inst.stream), ROAR_META_MODE_SET, &meta);

  free(info);
 }

 info = xmms_remote_get_playlist_title(g_inst.session, pos);
 if ( info ) {

  if ( (delm = strstr(info, " - ")) != NULL ) {
   *delm = 0;
   meta.value  = info;
   meta.type   = ROAR_META_TYPE_ARTIST;
   roar_stream_meta_set(&(g_inst.con), &(g_inst.stream), ROAR_META_MODE_SET, &meta);

   meta.value = delm + 3;
  } else {
   meta.value = info;
  }

  meta.type = ROAR_META_TYPE_TITLE;

  roar_stream_meta_set(&(g_inst.con), &(g_inst.stream), ROAR_META_MODE_SET, &meta);

  free(info);
 }

 meta.value = &empty;
 meta.type = ROAR_META_TYPE_NONE;
 roar_stream_meta_set(&(g_inst.con), &(g_inst.stream), ROAR_META_MODE_FINALIZE, &meta);

 return 0;
}

int roar_chk_metadata(void) {
 static int    old_pos = -1;
 static char * old_title = "NEW TITLE";
 int pos;
 char * title;
 int need_update = 0;

 pos     = xmms_remote_get_playlist_pos(g_inst.session);

 if ( pos != old_pos ) {
  old_pos = pos;
  need_update = 1;
 } else {
  title = xmms_remote_get_playlist_title(g_inst.session, pos);

  if ( strcmp(title, old_title) ) {
   free(old_title);
   old_title = title;
   need_update = 1;
  } else {
   free(title);
  }
 }

 if ( need_update ) {
  roar_update_metadata();
 }

 return 0;
}

// MIXER:

void roar_get_volume(int *l, int *r) {
 int channels;
 struct roar_mixer_settings mixer;
 float fs;

 if ( !(g_inst.state & STATE_CONNECTED) || !(g_inst.state & STATE_PLAYING) ) {
  *l = g_inst.mixer.l;
  *r = g_inst.mixer.r;
  return;
 }

 if ( roar_get_vol(&(g_inst.con), g_inst.stream.id, &mixer, &channels) == -1 ) {
  *l = *r = 100;
  return;
 }

 fs = (float)mixer.scale/100.;

 if ( channels == 1 ) {
  *l = *r = mixer.mixer[0]/fs;
 } else {
  *l = mixer.mixer[0]/fs;
  *r = mixer.mixer[1]/fs;
 }
}

void roar_set_volume(int l, int r) {
 struct roar_mixer_settings mixer;

 if ( !(g_inst.state & STATE_CONNECTED) )
  return;

 if ( l == -1 ) l = 100;
 if ( r == -1 ) r = 100;

 g_inst.mixer.l = l;
 g_inst.mixer.r = r;

 if ( !(g_inst.state & STATE_PLAYING) )
  return;

 mixer.mixer[0] = l;
 mixer.mixer[1] = r;
 mixer.scale    = 100;

 roar_set_vol(&(g_inst.con), g_inst.stream.id, &mixer, 2);
}

//ll
