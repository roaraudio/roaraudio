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

#include "roard.h"

struct roar_codecfilter g_codecfilter[] = {
 {-1,                     "null", "null codec filter", NULL, ROAR_CODECFILTER_NONE, NULL, NULL, NULL, NULL, NULL, NULL},

#ifdef ROAR_HAVE_LIBSNDFILE
 {ROAR_CODEC_RIFF_WAVE, "sndfile", "libsndfile codec filter", NULL, ROAR_CODECFILTER_READ,
  cf_sndfile_open, cf_sndfile_close, NULL, NULL, cf_sndfile_read, NULL},
#else
 {ROAR_CODEC_RIFF_WAVE, "RIFF/WAVE", "RIFF/WAVE", NULL, ROAR_CODECFILTER_READ,
  cf_wave_open, cf_wave_close, NULL, NULL, cf_wave_read, NULL},
#endif

 {ROAR_CODEC_ALAW, "alaw", "A-Law", NULL, ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE,
  cf_alaw_open, cf_alaw_close, NULL, cf_alaw_write, cf_alaw_read, NULL},

 {ROAR_CODEC_MULAW, "mulaw", "mu-Law", NULL, ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE,
  cf_mulaw_open, cf_mulaw_close, NULL, cf_mulaw_write, cf_mulaw_read, NULL},

#ifdef ROAR_HAVE_BIN_OGG123
 {ROAR_CODEC_OGG_GENERAL, "cmd",  "ogg123",
  ROAR_HAVE_BIN_OGG123 " -q -d raw -f - -", ROAR_CODECFILTER_READ,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL},
#endif

#ifdef ROAR_HAVE_LIBVORBISFILE
 {ROAR_CODEC_OGG_VORBIS, "oggvorbis", "Ogg Vorbis decoder", NULL,
#ifdef ROAR_HAVE_LIBVORBISENC
 ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE,
#else
 ROAR_CODECFILTER_READ,
#endif
 cf_vorbis_open, cf_vorbis_close, NULL, cf_vorbis_write, cf_vorbis_read, NULL},
#endif

#ifdef ROAR_HAVE_BIN_TIMIDITY
 {ROAR_CODEC_MIDI_FILE, "MIDIFILE", "timidity MIDI synth",
  ROAR_HAVE_BIN_TIMIDITY " -Or1sl -s %R -o - -", ROAR_CODECFILTER_READ,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL},
#endif

#ifdef ROAR_HAVE_LIBCELT
 {ROAR_CODEC_ROAR_CELT, "RoarCELT", "RoarAudio CELT", NULL, ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE,
  cf_celt_open, cf_celt_close, NULL, cf_celt_write, cf_celt_read, NULL},
#endif

#ifdef ROAR_HAVE_LIBSPEEX
 {ROAR_CODEC_ROAR_SPEEX, "RoarSpeex", "RoarAudio Speex", NULL, ROAR_CODECFILTER_READ|ROAR_CODECFILTER_WRITE,
  cf_speex_open, cf_speex_close, NULL, cf_speex_write, cf_speex_read, NULL},
#endif

#ifdef ROAR_HAVE_BIN_FLAC
 {ROAR_CODEC_FLAC, "cmd",  "flac",
#if BYTE_ORDER == BIG_ENDIAN
  ROAR_HAVE_BIN_FLAC " --silent --force-raw-format --sign=signed --endian=big -d - -o -",
#elif BYTE_ORDER == LITTLE_ENDIAN
  ROAR_HAVE_BIN_FLAC " --silent --force-raw-format --sign=signed --endian=little -d - -o -",
#else
  "false",
#endif
  ROAR_CODECFILTER_READ,
  cf_cmd_open, NULL, NULL, NULL, NULL, NULL},
#endif

 {-1, NULL, NULL, NULL, ROAR_CODECFILTER_NONE, NULL, NULL, NULL, NULL, NULL, NULL} // end of list
};

void print_codecfilterlist (void) {
 int i;
 int flags;
 char mode[5];

 printf("  Codec        Filtername   Mode - Description\n");
 printf("------------------------------------------------------\n");

 for (i = 0; g_codecfilter[i].name != NULL; i++) {
  flags = g_codecfilter[i].flags;

  if ( flags == ROAR_CODECFILTER_NONE ) {
   strcpy(mode, "none");
  } else {
   strcpy(mode, "    ");
   if ( flags & ROAR_CODECFILTER_READ )
    mode[0] = 'r';
   if ( flags & ROAR_CODECFILTER_WRITE )
    mode[1] = 'w';
  }
 
  printf("  %-12s %-12s %-4s - %s\n",
             roar_codec2str(g_codecfilter[i].codec),
             g_codecfilter[i].name,
             mode,
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
  if ( filter->open ) {
   if ( (i = filter->open(inst, codec, info, filter)) == -1 ) {
    info->codecfilter = *codecfilter_id = -1;
   }
   return i;
  }
  return 0;
 }

 return 0; // we found no filter -> ok
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

//ll
