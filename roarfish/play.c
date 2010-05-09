//play.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of roarfish a part of RoarAudio,
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

#include "roarfish.h"

struct roarfish_play_inst {
 int roarfh;
 int begun;
 FishSoundInfo fsinfo;
 FishSound * fsound;
};

int decoded_float (FishSound * fsound, float ** pcm, long frames, void * user_data) {
 struct roarfish_play_inst * inst = (struct roarfish_play_inst*) user_data;
 int16_t * data;
 int i;
 double s;

 if (!inst->begun) {
   fish_sound_command (fsound, FISH_SOUND_GET_INFO, &(inst->fsinfo),
                       sizeof (FishSoundInfo));
   if ( (inst->roarfh = roar_simple_play(inst->fsinfo.samplerate, inst->fsinfo.channels,
                                         16, ROAR_CODEC_NATIVE, NULL, "roarfish")) == -1 ) {
    return -1;
   }
   inst->begun = 1;
 }

 if ( (data = malloc(frames*2*inst->fsinfo.channels)) == NULL ) {
  return -1;
 }

 frames *= inst->fsinfo.channels;

 for (i = 0; i < frames; i++) {
  s  = ((float*)pcm)[i];
  s *= 32767;
  data[i] = s;
 }


 write(inst->roarfh, (char*)data, frames * 2);


 free(data);

 return 0;
}

int read_packet (OGGZ * oggz, ogg_packet * op, long serialno, void * user_data) {
 FishSound * fsound = (FishSound *)user_data;

 fish_sound_prepare_truncation (fsound, op->granulepos, op->e_o_s);
 fish_sound_decode (fsound, op->packet, op->bytes);

 return 0;
}

int play (char * file, char ** opts) {
 OGGZ * oggz;
 struct roarfish_play_inst inst;

 inst.roarfh = -1;
 inst.begun  = 0;

 inst.fsound = fish_sound_new(FISH_SOUND_DECODE, &inst.fsinfo);
 fish_sound_set_interleave(inst.fsound, 1);

 fish_sound_set_decoded_float_ilv(inst.fsound, decoded_float, (void*)&inst);

 if ((oggz = oggz_open(file, OGGZ_READ)) == NULL) {
  ROAR_ERR("Can not open input file: %s", file);
  return -1;
 }

 oggz_set_read_callback(oggz, -1, read_packet, inst.fsound);

 // TODO: add some status display here?
 while (oggz_read(oggz, 1024));

 oggz_close(oggz);

 fish_sound_delete(inst.fsound);

 return -1;
}

//ll
