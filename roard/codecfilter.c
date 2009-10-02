//codecfilter.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#define ROAR_REQUIRE_LIBVORBISFILE

#include "roard.h"

#ifndef ROAR_WITHOUT_CF_CMD
#if !defined(ROAR_HAVE_UNIX) || !defined(ROAR_HAVE_BSDSOCKETS) || !defined(ROAR_HAVE_IO_POSIX)
#define ROAR_WITHOUT_CF_CMD
#endif
#endif

#define _DUMMY_FILTER(codec,name,desc,flags,subsystem) {(codec),(name),(desc),NULL,NULL,(flags),(subsystem), \
                                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL        }

#define _PCM_FILTER(codec) _DUMMY_FILTER(codec, "PCM", "Native PCM Support", \
                                         ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU_NN, \
                                         ROAR_SUBSYS_WAVEFORM)

struct roar_codecfilter g_codecfilter[] = {
 _DUMMY_FILTER(-1, "null", "null codec filter", ROAR_CODECFILTER_NONE, ROAR_SUBSYS_NONE),
 _PCM_FILTER(ROAR_CODEC_PCM_S_LE),
 _PCM_FILTER(ROAR_CODEC_PCM_S_BE),
 _PCM_FILTER(ROAR_CODEC_PCM_S_PDP),
 _PCM_FILTER(ROAR_CODEC_PCM_U_LE),
 _PCM_FILTER(ROAR_CODEC_PCM_U_BE),
 _PCM_FILTER(ROAR_CODEC_PCM_U_PDP),
 _DUMMY_FILTER(ROAR_CODEC_MIDI, "MIDI", "Native MIDI Support",
               ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU_NN, ROAR_SUBSYS_MIDI),
 {ROAR_CODEC_DMX512,      "DMX512", "Native DMX512 Support", NULL, NULL,
                          ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU_NN, ROAR_SUBSYS_LIGHT,
                                          NULL, NULL, NULL, NULL, NULL, NULL, cf_alaw_delay, NULL},

/*
#ifdef ROAR_HAVE_LIBSNDFILE
 {ROAR_CODEC_RIFF_WAVE, "sndfile", "libsndfile codec filter", NULL, ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE,
  cf_sndfile_open, cf_sndfile_close, NULL, cf_sndfile_write, cf_sndfile_read, NULL},
#else
*/
#ifndef ROAR_WITHOUT_CF_WAVE
 {ROAR_CODEC_RIFF_WAVE, "RIFF/WAVE", "RIFF/WAVE", NULL, NULL,
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU, ROAR_SUBSYS_WAVEFORM,
  cf_wave_open, cf_wave_close, NULL, cf_wave_write, cf_wave_read, NULL, NULL, cf_wave_ctl},
#endif
//#endif

#ifdef ROAR_HAVE_LIBRAUM
 {ROAR_CODEC_RAUM, "RAUM", "RAUM Container Format", NULL, cf_raum_setup, ROAR_CODECFILTER_WRITE,
  ROAR_SUBSYS_WAVEFORM|ROAR_SUBSYS_MIDI|ROAR_SUBSYS_LIGHT|ROAR_SUBSYS_RAW|ROAR_SUBSYS_COMPLEX,
  cont_fw_cf_open, cont_fw_cf_close, NULL, cont_fw_cf_write, cont_fw_cf_read, cont_fw_cf_flush, NULL, cont_fw_cf_ctl},
#endif

#ifdef ROAR_HAVE_LIBOGG
 {ROAR_CODEC_OGG, "Ogg", "Ogg Container Format", NULL, NULL, ROAR_CODECFILTER_NONE,
  ROAR_SUBSYS_WAVEFORM|ROAR_SUBSYS_MIDI,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL},
#endif

#ifdef ROAR_SUPPORT_ALAW
 {ROAR_CODEC_ALAW, "alaw", "A-Law", NULL, NULL,
#ifdef ROAR_SUPPORT_ALAW_RW
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU_NN,
#else
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_PRETHRU_NN,
#endif
  ROAR_SUBSYS_WAVEFORM,
  cf_alaw_open, cf_alaw_close, NULL,
#ifdef ROAR_SUPPORT_ALAW_RW
  cf_alaw_write,
#else
  NULL,
#endif
  cf_alaw_read, NULL, cf_alaw_delay, NULL},
#endif

#ifdef ROAR_SUPPORT_MULAW
 {ROAR_CODEC_MULAW, "mulaw", "mu-Law", NULL, NULL,
#ifdef ROAR_SUPPORT_MULAW_RW
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU_NN,
#else
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_PRETHRU_NN,
#endif
  ROAR_SUBSYS_WAVEFORM,
  cf_mulaw_open, cf_mulaw_close, NULL,
#ifdef ROAR_SUPPORT_MULAW_RW
  cf_mulaw_write,
#else
  NULL,
#endif
  cf_mulaw_read, NULL, cf_alaw_delay, NULL},
#endif

#ifndef ROAR_WITHOUT_CF_CMD
#ifdef ROAR_HAVE_BIN_OGG123
 {ROAR_CODEC_OGG_GENERAL, "cmd",  "ogg123",
  ROAR_HAVE_BIN_OGG123 " -q -d raw -f - -", NULL, ROAR_CODECFILTER_READ, ROAR_SUBSYS_WAVEFORM,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL, codecfilter_delay_fulldyn, NULL},
#endif
#endif

#ifdef ROAR_HAVE_LIBVORBISFILE
 {ROAR_CODEC_OGG_VORBIS, "oggvorbis", "Ogg Vorbis decoder", NULL, NULL,
#ifdef ROAR_HAVE_LIBVORBISENC
 ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU,
#else
 ROAR_CODECFILTER_READ,
#endif
 ROAR_SUBSYS_WAVEFORM,
 cf_vorbis_open, cf_vorbis_close, NULL, cf_vorbis_write, cf_vorbis_read, NULL, codecfilter_delay_fulldyn, cf_vorbis_ctl},
#else
#ifndef ROAR_WITHOUT_CF_CMD
#ifdef ROAR_HAVE_BIN_OGG123
 {ROAR_CODEC_OGG_VORBIS, "cmd",  "ogg123",
  ROAR_HAVE_BIN_OGG123 " -q -d raw -f - -", NULL, ROAR_CODECFILTER_READ, ROAR_SUBSYS_WAVEFORM,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL, codecfilter_delay_fulldyn, NULL},
#endif
#endif
#endif

#ifdef ROAR_HAVE_LIBFISHSOUND
 {ROAR_CODEC_OGG_SPEEX, "fishsound",  "libfishsound Xiph Codec library",
  NULL, NULL, ROAR_CODECFILTER_READ, ROAR_SUBSYS_WAVEFORM,
  cf_fishsound_open, cf_fishsound_close, NULL, NULL, cf_fishsound_read, NULL, codecfilter_delay_fulldyn, NULL},

 {ROAR_CODEC_OGG_FLAC, "fishsound",  "libfishsound Xiph Codec library",
  NULL, NULL, ROAR_CODECFILTER_READ, ROAR_SUBSYS_WAVEFORM,
  cf_fishsound_open, cf_fishsound_close, NULL, NULL, cf_fishsound_read, NULL, codecfilter_delay_fulldyn, NULL},
#endif

#ifndef ROAR_WITHOUT_CF_CMD
#ifdef ROAR_HAVE_BIN_TIMIDITY
 {ROAR_CODEC_MIDI_FILE, "MIDIFILE", "timidity MIDI synth",
  ROAR_HAVE_BIN_TIMIDITY " -Or1sl -s %R -o - -", NULL, ROAR_CODECFILTER_READ, ROAR_SUBSYS_WAVEFORM,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL, codecfilter_delay_fulldyn, NULL},
#endif
#endif

#ifdef ROAR_HAVE_LIBCELT
 {ROAR_CODEC_ROAR_CELT, "RoarCELT", "RoarAudio CELT", NULL, NULL,
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU,
  ROAR_SUBSYS_WAVEFORM,
  cf_celt_open, cf_celt_close, NULL, cf_celt_write, cf_celt_read, NULL, cf_celt_delay, NULL},
#endif

#ifdef ROAR_HAVE_LIBSPEEX
 {ROAR_CODEC_ROAR_SPEEX, "RoarSpeex", "RoarAudio Speex", NULL, NULL,
  ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU,
  ROAR_SUBSYS_WAVEFORM,
  cf_speex_open, cf_speex_close, NULL, cf_speex_write, cf_speex_read, NULL, NULL, NULL},
#endif

#ifndef ROAR_WITHOUT_CF_CMD
#ifdef ROAR_HAVE_BIN_FLAC
 {ROAR_CODEC_FLAC, "cmd",  "flac",
#if BYTE_ORDER == BIG_ENDIAN
  ROAR_HAVE_BIN_FLAC " --silent --force-raw-format --sign=signed --endian=big -d - -o -",
#elif BYTE_ORDER == LITTLE_ENDIAN
  ROAR_HAVE_BIN_FLAC " --silent --force-raw-format --sign=signed --endian=little -d - -o -",
#else
  "false",
#endif
  NULL, ROAR_CODECFILTER_READ, ROAR_SUBSYS_WAVEFORM,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL, codecfilter_delay_fulldyn, NULL},
#endif
#endif

 _DUMMY_FILTER(ROAR_CODEC_ROARDMX, "RoarDMX", "Native RoarDMX Support",
               ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE|ROAR_CODECFILTER_PRETHRU_NN, ROAR_SUBSYS_LIGHT),

 {-1, NULL, NULL, NULL, NULL, ROAR_CODECFILTER_NONE, ROAR_SUBSYS_NONE,
  NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL} // end of list
};

void print_codecfilterlist (void) {
 int i;
 int flags;
 char mode[5];
 char delay[6];
 char subsys[7] = "      ";
 uint_least32_t d;

 printf("  Codec        Filtername   Mode Subsys Delay - Description\n");
 printf("-------------------------------------------------------------\n");

 for (i = 0; g_codecfilter[i].name != NULL; i++) {
  strncpy(subsys, "      ", 6);

  if ( g_codecfilter[i].subsystems & ROAR_SUBSYS_WAVEFORM )
   subsys[0] = 'W';
  if ( g_codecfilter[i].subsystems & ROAR_SUBSYS_MIDI )
   subsys[1] = 'M';
  if ( g_codecfilter[i].subsystems & ROAR_SUBSYS_CB )
   subsys[2] = 'C';
  if ( g_codecfilter[i].subsystems & ROAR_SUBSYS_LIGHT )
   subsys[3] = 'L';
  if ( g_codecfilter[i].subsystems & ROAR_SUBSYS_RAW )
   subsys[4] = 'R';
  if ( g_codecfilter[i].subsystems & ROAR_SUBSYS_COMPLEX )
   subsys[5] = 'X';

  flags = g_codecfilter[i].flags;

  if ( flags == ROAR_CODECFILTER_NONE ) {
   strcpy(mode, "none");
  } else {
   strcpy(mode, "    ");
   if ( flags & ROAR_CODECFILTER_READ )
    mode[0] = 'r';
   if ( flags & ROAR_CODECFILTER_WRITE )
    mode[1] = 'w';
   if ( flags & ROAR_CODECFILTER_PRETHRU )
    mode[2] = 'P';
   if ( flags & ROAR_CODECFILTER_PRETHRU_NN )
    mode[2] = 'p';
  }

  *delay = 0;
  if ( g_codecfilter[i].codec == -1 ) { // null codec filter
   strcpy(delay, "0ms");
  } else if ( g_codecfilter[i].delay == NULL ) {
   strcpy(delay, "?");
  } else {
   if ( codecfilter_delay(NULL, i, &d) == -1 ) {
    strcpy(delay, "dyn");
   } else {
    snprintf(delay, 5, "%ims", d/1000);
   }
  }
 
  printf("  %-12s %-12s %-4s %6s %-5s - %s\n",
             roar_codec2str(g_codecfilter[i].codec),
             g_codecfilter[i].name,
             mode,
             subsys,
             delay,
             g_codecfilter[i].desc
             );
 }
}

int codecfilter_open (CODECFILTER_USERDATA_T * inst,
                 int * codecfilter_id, char * codecfilter /* NOTE: this is not part of struct roar_codecfilter's def! */,
                 int codec, struct roar_stream_server * info) {
 int i;
 struct roar_codecfilter   * filter = NULL;

 *codecfilter_id = -1;

 ROAR_DBG("codecfilter_open(*): codecfilter='%s', info->id=%i", codecfilter, ROAR_STREAM(info)->id);

 for (i = 0; g_codecfilter[i].name != NULL; i++) {
  ROAR_DBG("codecfilter_open(*): g_codecfilter[i].codec <-> codec => %i <-> %i", g_codecfilter[i].codec, codec);
  if ( g_codecfilter[i].codec == codec ) {
   if ( !codecfilter || strcmp(codecfilter, g_codecfilter[i].name) == 0 ) {
    *codecfilter_id = i;
    filter = &g_codecfilter[i];
    break;
   }
  }
 }

 info->codecfilter = *codecfilter_id;

 if (*codecfilter_id != -1) {
  if ( filter->open != NULL ) {
   if ( (i = filter->open(inst, codec, info, filter)) == -1 ) {
    info->codecfilter = *codecfilter_id = -1;
   }
   return i;
  } else {
   // in case we can not open the filter we will not use it
   *codecfilter_id   = -1;
   info->codecfilter = -1;
  }
  return 0;
 }

 return -1; // we found no filter -> ok
}

int codecfilter_close(CODECFILTER_USERDATA_T   inst, int codecfilter) {
 ROAR_DBG("codecfilter_close(inst=%p, codecfilter=%i) = ?", inst, codecfilter);

 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].close )
  return g_codecfilter[codecfilter].close(inst);

 return 0;
}

int codecfilter_pause(CODECFILTER_USERDATA_T   inst, int codecfilter, int newstate) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].pause )
  return g_codecfilter[codecfilter].pause(inst, newstate);

 return 0;
}

int codecfilter_write(CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].write )
  return g_codecfilter[codecfilter].write(inst, buf, len);

 return 0;
}

int codecfilter_read (CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len) {
 if ( codecfilter == -1 )
  return -1;

 errno = 0;

 if ( g_codecfilter[codecfilter].read )
  return g_codecfilter[codecfilter].read(inst, buf, len);

 return 0;
}

int codecfilter_flush(CODECFILTER_USERDATA_T   inst, int codecfilter) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].flush )
  return g_codecfilter[codecfilter].flush(inst);

 return 0;
}

int codecfilter_delay(CODECFILTER_USERDATA_T   inst, int codecfilter, uint_least32_t * delay) {
 ROAR_DBG("codecfilter_delay(inst=%p, codecfilter=%i, *delay=?) = ?", inst, codecfilter);

 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].delay )
  return g_codecfilter[codecfilter].delay(inst, delay);

 return -1;
}

int codecfilter_ctl  (CODECFILTER_USERDATA_T   inst, int codecfilter, int_least32_t cmd, void * data) {
 ROAR_DBG("codecfilter_ctl(inst=%p, codecfilter=%i, cmd=0x%.8x, data=%p) = ?", inst, codecfilter, cmd, data);
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].ctl )
  return g_codecfilter[codecfilter].ctl(inst, cmd, data);

 return -1;
}


int codecfilter_delay_fulldyn(CODECFILTER_USERDATA_T   inst, uint_least32_t * delay) {
 *delay = 0; // just to be sure
 return -1;
}

//ll
