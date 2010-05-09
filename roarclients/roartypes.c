//roartypes.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of roarclients a part of RoarAudio,
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

#include <roaraudio.h>
#include "../roard/include/roard.h"

int main (void) {
 struct { char * name; int len; } types[] = {
  { "roar_buffer",         sizeof(struct roar_buffer)         },
  { "roar_stack",          sizeof(struct roar_stack)          },
  { "roar_message",        sizeof(struct roar_message)        },
  { "roar_stream",         sizeof(struct roar_stream)         },
  { "roar_stream_server",  sizeof(struct roar_stream_server)  },
  { "roar_mixer_settings", sizeof(struct roar_mixer_settings) },
  { "roar_sample",         sizeof(struct roar_sample)         },
  { "roar_client",         sizeof(struct roar_client)         },
  { "roard_config",        sizeof(struct roard_config)        },
  { "roar_connection",     sizeof(struct roar_connection)     },
  { "roar_audio_info",     sizeof(struct roar_audio_info)     },
  { "roar_meta",           sizeof(struct roar_meta)           },
  { "roar_vio_calls",      sizeof(struct roar_vio_calls)      },
  { "roar_stack",          sizeof(struct roar_stack)          },
  { "roar_vio_defaults",   sizeof(struct roar_vio_defaults)   },
  { "roar_vio_dstr_chain", sizeof(struct roar_vio_dstr_chain) },
#ifndef ROAR_WITHOUT_DCOMP_MIDI
  { "midi_message",        sizeof(struct midi_message)        },
#endif
  { "roar_note_octave",    sizeof(struct roar_note_octave)    },
  { NULL, 0 }
 }, * c = types - 1;

 while ((++c)->name != NULL)
  printf("%-20s = %5i Bytes = %6i Bits\n", c->name, c->len, c->len * 8);

 return 0;
}

//ll
