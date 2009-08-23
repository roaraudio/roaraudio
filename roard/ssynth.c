//ssynth.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH

float ssynth_polys[SSYNTH_POLY_POLYMAX][SSYNTH_POLY_COEFF] = {
       {0.300000,  0.958333, -0.550000,  0.091667},
       {0.700010, -0.083333, -0.150000,  0.033333}
      };

int ssynth_init_config(void) {
 memset(&ssynth_conf, 0, sizeof(ssynth_conf));

 return 0;
}

#define _err() streams_delete(sid); return -1
int ssynth_init (void) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 int sid;

 if ( !ssynth_conf.enable )
  return 0;

 memset(&g_ssynth, 0, sizeof(g_ssynth));
 g_ssynth.stream = -1;

 if ( (sid = streams_new()) == -1 )
  return -1;

 if ( streams_set_client(sid, g_self_client) == -1 ) {
  _err();
 }

 if ( streams_set_dir(sid, ROAR_DIR_BRIDGE, 1) == -1 ) {
  _err();
 }

 if ( streams_set_flag(sid, ROAR_FLAG_PRIMARY) == -1 ) {
  _err();
 }

 if ( streams_set_name(sid, "Simple Synthesizer") == -1 ) {
  _err();
 }

 if ( streams_get(sid, &ss) == -1 ) {
  _err();
 }

 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->info.channels = 1;
 s->info.codec    = ROAR_CODEC_DEFAULT;

 g_ssynth.stream  = sid;

 return 0;
}

int ssynth_free (void) {
 int i;

 if ( !ssynth_conf.enable )
  return 0;

 for (i = 0; i < SSYNTH_NOTES_MAX; i++) {
  if ( g_ssynth.notes[i].buf != NULL ) {
   roar_buffer_free(g_ssynth.notes[i].buf);
   g_ssynth.notes[i].buf = NULL;
  }
 }

 return streams_delete(g_ssynth.stream);
}

int ssynth_update (void) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 struct roar_buffer        * buf;
 struct roar_buffer        * outbuf;
 void                      * outbufdata;
 void                      * indbufs[SSYNTH_NOTES_MAX+1];
 int                         curin = 0;
 size_t buflen;
 size_t needlen;
 int i;

 if ( !ssynth_conf.enable )
  return 0;

 ROAR_DBG("ssynth_update(void) = ?");

 if ( streams_get(g_ssynth.stream, &ss) == -1 ) {
  return -1;
 }

 memset(indbufs, 0, sizeof(indbufs));

 s = ROAR_STREAM(ss);

 needlen = ROAR_OUTPUT_CALC_OUTBUFSIZE(&(s->info));

 ROAR_DBG("ssynth_update(void): needlen=%lu", (unsigned long)needlen);

 for (i = 0; i < SSYNTH_NOTES_MAX; i++) {
  if ( g_ssynth.notes[i].stage != SSYNTH_STAGE_UNUSED ) {
   ROAR_DBG("ssynth_update(void): used note slot: %i", i);
   if ( g_ssynth.notes[i].buf == NULL ) {
    if ( roar_buffer_new(&buf, needlen) == -1 )
     continue;

    g_ssynth.notes[i].buf = buf;
   } else {
    buf = g_ssynth.notes[i].buf;

    if ( roar_buffer_get_len(buf, &buflen) == -1 )
     continue;

    if ( buflen < needlen ) {
     if ( roar_buffer_set_len(buf, needlen) == -1 )
      continue;
    }
   }

   if ( roar_buffer_get_data(buf, &(indbufs[curin])) == -1 )
    continue;

   if ( ssynth_note_render(i, indbufs[curin]) == -1 )
    continue;

   curin++;
  }
 }

 ROAR_DBG("ssynth_update(void): found streams: %i", curin);

 if ( curin > 0 ) {
  if ( roar_buffer_new(&outbuf, needlen) == -1 )
   return -1;

  if ( roar_buffer_get_data(outbuf, &outbufdata) == -1 ) {
   roar_buffer_free(outbuf);
   return -1;
  }

  if ( mix_clients(outbufdata, g_sa->bits, indbufs, ROAR_OUTPUT_BUFFER_SAMPLES) == -1 ) {
   roar_buffer_free(outbuf);
   return -1;
  }

  if ( stream_add_buffer(g_ssynth.stream, outbuf) == -1 ) {
   roar_buffer_free(outbuf);
   return -1;
  }
 }

 ROAR_DBG("ssynth_update(void) = 0");
 return 0;
}


int ssynth_note_new(struct roar_note_octave * note, char vv) {
 int i;

 ROAR_DBG("ssynth_note_new(note=%p, vv=%i) = ?", note, vv);

 for (i = 0; i < SSYNTH_NOTES_MAX; i++) {
  if ( g_ssynth.notes[i].stage == SSYNTH_STAGE_UNUSED ) {
   // TODO: do some error checking here
   g_ssynth.notes[i].vv_down = vv;
   memcpy(&(g_ssynth.notes[i].note), note, sizeof(struct roar_note_octave));
   roar_synth_init(&(g_ssynth.notes[i].synth), &(g_ssynth.notes[i].note), g_sa->rate);
   roar_synth_set_volume(&(g_ssynth.notes[i].synth), 0.25);
   roar_synth_set_func(&(g_ssynth.notes[i].synth), ROAR_SYNTH_SYNF_TRAP);
   ssynth_note_set_stage(i, SSYNTH_STAGE_KEYSTROKE);
   ROAR_DBG("ssynth_note_new(note=%p, vv=%i) = %i", note, vv, i);
   return i;
  }
 }

 ROAR_DBG("ssynth_note_new(note=%p, vv=%i) = -1", note, vv);
 return -1;
}

int ssynth_note_free(int id) {
 g_ssynth.notes[id].stage = SSYNTH_STAGE_UNUSED;
 return 0;
}

int ssynth_note_find(struct roar_note_octave * note) {
 struct roar_note_octave * cn;
 int i;

 for (i = 0; i < SSYNTH_NOTES_MAX; i++) {
  if ( g_ssynth.notes[i].stage == SSYNTH_STAGE_UNUSED )
   continue;

  cn = &(g_ssynth.notes[i].note);

  if ( !(note->note == cn->note && note->octave == cn->octave) )
   continue;

  return i;
 }

 return -1;
}

int ssynth_note_set_stage(int id, int stage) {
 int r = -1;

 switch (stage) {
  case SSYNTH_STAGE_UNUSED:
    r = ssynth_note_free(id);
   break;
  case SSYNTH_STAGE_KEYSTROKE:
    r = roar_fader_init(&(g_ssynth.notes[id].fader), ssynth_polys[SSYNTH_POLY_KEYDOWN], SSYNTH_POLY_COEFF);
   break;
  case SSYNTH_STAGE_MIDSECTION:
    r = 0;
   break;
  case SSYNTH_STAGE_KEYRELEASE:
    r = roar_fader_init(&(g_ssynth.notes[id].fader), ssynth_polys[SSYNTH_POLY_KEYUP], SSYNTH_POLY_COEFF);
   break;
 }

 if ( r == 0 )
  g_ssynth.notes[id].stage = stage;

 return r;
}

int ssynth_note_render   (int id, void * data) {
 if ( g_sa->bits != 16 )
  return -1;

 return roar_synth_pcmout_i161(&(g_ssynth.notes[id].synth), data, ROAR_OUTPUT_BUFFER_SAMPLES);
}

int ssynth_note_on       (struct roar_note_octave * note, char vv) {
 ROAR_DBG("ssynth_note_on(note=%p, vv=%i) = ?", note, vv);
 return ssynth_note_new(note, vv);
}

int ssynth_note_off      (struct roar_note_octave * note, char vv) {
 int id;

 ROAR_DBG("ssynth_note_off(note=%p, vv=%i) = ?", note, vv);

 if ( (id = ssynth_note_find(note)) == -1 )
  return -1;

 // add support to for keyups...

 return ssynth_note_free(id);
}

int ssynth_eval_message (struct midi_message * mes) {
 if ( !ssynth_conf.enable )
  return -1;

 ROAR_DBG("ssynth_eval_message(mes=%p) = ?", mes);

 switch (mes->type) {
  case MIDI_TYPE_NOTE_OFF:
    return ssynth_note_off(&(mes->d.note), mes->vv);
   break;
  case MIDI_TYPE_NOTE_ON:
    return ssynth_note_on(&(mes->d.note), mes->vv);
   break;
  default:
    /* ignore all other events at the moment */
    return 0;
 }

 ROAR_DBG("ssynth_eval_message(mes=%p) = -1", mes);
 return -1;
}

#endif

//ll
