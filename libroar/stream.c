//stream.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroar.h"

int roar_stream_connect (struct roar_connection * con, struct roar_stream * s, int dir) {
 struct roar_message m;

 s->dir = dir;

 m.cmd     = ROAR_CMD_NEW_STREAM;
 m.stream  = -1;
 m.pos     = 0;

 roar_stream_s2m(s, &m);

 if ( roar_req(con, &m, NULL) != 0 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK ) {
  s->id = m.stream;

  ROAR_DBG("roar_stream_connect(*) = 0");
  return 0;
 }

 ROAR_ERR("roar_stream_connect(*): Connecting new stream faild!");
 ROAR_DBG("roar_stream_connect(*) = -1");
 return -1;
}

int roar_stream_new (struct roar_stream * s, unsigned int rate,
                     unsigned int channels, unsigned int bits, unsigned int codec) {

 if ( s == NULL )
  return -1;

 s->fh         = -1;
 s->id         = -1;
 s->pos        =  0;
 s->pos_rel_id = -1;

 s->dir        = ROAR_DIR_DEFAULT;

/*
 s->datalen    = 0;
 s->offset     = 0;

 s->database   = NULL;
 s->dataoff    = NULL;
*/

 s->info.rate     = rate;
 s->info.channels = channels;
 s->info.bits     = bits;
 s->info.codec    = codec;

 if ( bits > ROAR_BITS_MAX )
  return -1;

 return 0;
}

int roar_stream_set_rel_id(struct roar_stream * s, int id) {
 if ( s == NULL )
  return -1;

 s->pos_rel_id = id;

 return 0;
}

int roar_stream_get_rel_id(struct roar_stream * s) {
 if ( s == NULL )
  return -1;

 return s->pos_rel_id;
}

int roar_stream_new_by_id(struct roar_stream * s, int id) {
 if ( s == NULL )
  return -1;

 if ( roar_stream_new_empty(s) == -1 )
  return -1;

 return roar_stream_set_id(s, id);
}

int roar_stream_new_empty(struct roar_stream * s) {
 if ( s == NULL )
  return -1;

 return roar_stream_new(s, 0, 0, 0, 0);
}

int roar_stream_set_id (struct roar_stream * s, int id) {
 if ( s == NULL )
  return -1;

 s->id = id;

 return 0;
}

int roar_stream_get_id (struct roar_stream * s) {
 if ( s == NULL )
  return -1;

 return s->id;
}

int roar_stream_set_fh (struct roar_stream * s, int fh) {
 if ( s == NULL )
  return -1;

 s->fh = fh;

 return 0;
}

int roar_stream_get_fh (struct roar_stream * s) {
 if ( s == NULL )
  return -1;

 return s->fh;
}

int roar_stream_set_dir (struct roar_stream * s, int dir) {
 if ( s == NULL )
  return -1;

 s->dir = dir;

 return 0;
}

int roar_stream_get_dir (struct roar_stream * s) {
 if ( s == NULL )
  return -1;

 return s->dir;
}


int roar_stream_exec    (struct roar_connection * con, struct roar_stream * s) {
 struct roar_message m;

 m.cmd     = ROAR_CMD_EXEC_STREAM;
 m.stream  = s->id;
 m.datalen = 0;
 m.pos     = 0;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_connect_to (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port) {
 struct roar_message m;

 if ( roar_stream_connect_to_ask(con, s, type, host, port) == -1 )
  return -1;

 if ( roar_recv_message(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_connect_to_ask (struct roar_connection * con, struct roar_stream * s, int type, char * host, int port) {
 struct roar_message m;
 int len = 0;

 if ( host == NULL )
  return -1;

 ROAR_DBG("roar_stream_connect_to_ask(*): Ask the server to connect to: %s:%i", host, port);

 m.cmd     = ROAR_CMD_CON_STREAM;
 m.stream  = s->id;
 m.pos     = 0;

 m.data[0] = 0;
 m.data[1] = type;
 ((uint16_t*)&(m.data))[1] = ROAR_HOST2NET16(port);

 len = strlen(host);

 if ( len > 76 )
  return -1;

 strncpy(&(m.data[4]), host, len);

 m.datalen = len + 4;

 if ( roar_send_message(con, &m, NULL) == -1 )
  return -1;

 return 0;
}

int roar_stream_passfh  (struct roar_connection * con, struct roar_stream * s, int fh) {
 struct roar_message m;
 int confh;

 m.cmd     = ROAR_CMD_PASSFH;
 m.stream  = s->id;
 m.pos     = 0;
 m.datalen = 0;

 ROAR_DBG("roar_stream_passfh(con=%p{...}, s={.id=%i,...}, fh=%i) = ?", con, s->id, fh);

 if ( (confh = roar_get_connection_fh(con)) == -1 )
  return -1;

 if ( roar_send_message(con, &m, NULL) == -1 ) {
  ROAR_DBG("roar_stream_passfh(con=%p{...}, s={.id=%i,...}, fh=%i) = -1 // can not send message", con, s->id, fh);
  return -1;
 }

 ROAR_DBG("roar_stream_passfh(*): msg send");

 if ( roar_socket_send_fh(confh, fh, NULL, 0) == -1 )
  return -1;

 ROAR_DBG("roar_stream_passfh(*): fh send");

 if ( roar_recv_message(con, &m, NULL) == -1 )
  return -1;

 ROAR_DBG("roar_stream_passfh(*): mes recved");

 if ( m.cmd == ROAR_CMD_OK )
  return 0;

 return -1;
}

int roar_stream_attach_simple (struct roar_connection * con, struct roar_stream * s, int client) {
 struct roar_message m;
 uint16_t * info = (uint16_t *) m.data;
 int i;

 m.cmd     = ROAR_CMD_ATTACH;
 m.stream  = s->id;
 m.pos     = 0;
 m.datalen = 6;

 info[0] = 0;
 info[1] = ROAR_ATTACH_SIMPLE;
 info[2] = client;

 for (i = 0; i < m.datalen/2; i++) {
  info[i] = ROAR_HOST2NET16(info[i]);
 }

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

int roar_stream_add_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len) {
 struct roar_message m;

 m.cmd     = ROAR_CMD_ADD_DATA;
 m.stream  = s->id;
 m.pos     = 0;
 m.datalen = len;

// if ( roar_req(con, &m, (void**)&data) == -1 )
//  return -1;
 if ( roar_send_message(con, &m, data) != 0 )
  return -1;

 if ( roar_recv_message(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_send_data (struct roar_connection * con, struct roar_stream * s, char * data, size_t len) {
 if ( ! s )
  return -1;

 if ( s->fh == -1 ) {
  if ( !con )
   return -1;

  if ( roar_stream_add_data(con, s, data, len) == -1 )
   return -1;

  return len;
 }

#ifdef ROAR_HAVE_IO_POSIX
 return write(s->fh, data, len);
#endif

 return -1;
}

int roar_stream_get_info (struct roar_connection * con, struct roar_stream * s, struct roar_stream_info * info) {
 struct roar_message m;
 uint16_t * data = (uint16_t *) m.data;
 int i;

 m.cmd     = ROAR_CMD_GET_STREAM_PARA;
 m.stream  = s->id;
 m.datalen = 4;
 m.pos     = 0;

 data[0] = 0; // Version and reserved
 data[1] = ROAR_STREAM_PARA_INFO; // stream

 for (i = 0; i < m.datalen/2; i++) {
  data[i] = ROAR_HOST2NET16(data[i]);
 }

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 for (i = 0; i < m.datalen/2; i++) {
  data[i] = ROAR_NET2HOST16(data[i]);
 }

 if ( m.datalen < 7*2 )
  return -1;

 if ( data[0] != 0 || data[1] != 1 )
  return -1;

 memset(info, 0, sizeof(struct roar_stream_info));

 info->block_size     = data[2];
 info->pre_underruns  = data[3];
 info->post_underruns = data[4];
 info->codec          = data[5];
 info->flags          = data[6];
 info->delay          = data[7]*1000;

 if ( m.datalen < 8*2 ) {
  info->state         = ROAR_STREAMSTATE_UNKNOWN;
  return 0;
 } else {
  info->state         = data[8];
 }

 return 0;
}

int roar_stream_get_name (struct roar_connection * con, struct roar_stream * s, char * name, size_t len) {
 struct roar_message m;
 uint16_t * data = (uint16_t *) m.data;

 if ( con == NULL || s == NULL || name == NULL || len == 0 )
  return -1;

 name[0] = 0; // just in case...

 m.cmd     = ROAR_CMD_GET_STREAM_PARA;
 m.stream  = s->id;
 m.datalen = 4;
 m.pos     = 0;

 data[0] = 0; // Version and reserved
 data[1] = ROAR_STREAM_PARA_NAME; // stream

 data[0] = ROAR_HOST2NET16(data[0]);
 data[1] = ROAR_HOST2NET16(data[1]);

 ROAR_DBG("roar_stream_get_name(*) = ?");

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 ROAR_DBG("roar_stream_get_name(*) = ?");

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 ROAR_DBG("roar_stream_get_name(*) = ?");

 if ( m.datalen < 4 )
  return -1;

 data[0] = ROAR_NET2HOST16(data[0]);
 data[1] = ROAR_NET2HOST16(data[1]);

 ROAR_DBG("roar_stream_get_name(*) = ?");

 if ( data[0] != 0 || data[1] != ROAR_STREAM_PARA_NAME )
  return -1;

 m.datalen -= 4;

 len--;

 if ( len > m.datalen )
  len = m.datalen;

 strncpy(name, ((char*)m.data)+4, len);
 name[len] = 0;

 ROAR_DBG("roar_stream_get_name(*) = 0");

 return 0;
}

int roar_stream_set_flags (struct roar_connection * con, struct roar_stream * s, int flags, int reset) {
 struct roar_message m;
 uint16_t * data = (uint16_t *) m.data;
 int i;

 m.cmd     = ROAR_CMD_SET_STREAM_PARA;
 m.stream  = s->id;
 m.datalen = 8;
 m.pos     = 0;

 data[0] = 0; // Version and reserved
 data[1] = 2; // flags
 data[2] = reset == ROAR_RESET_FLAG ? ROAR_RESET_FLAG : ROAR_SET_FLAG;
 data[3] = flags;

 for (i = 0; i < m.datalen/2; i++) {
  data[i] = ROAR_HOST2NET16(data[i]);
 }

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 return 0;
}

#define _ROAR_STREAM_MESSAGE_LEN ((5+1)*4)

int roar_stream_s2m     (struct roar_stream * s, struct roar_message * m) {
 uint32_t * data;
 int i;

 if ( !(s && m) )
  return -1;

 m->datalen = _ROAR_STREAM_MESSAGE_LEN;
 data = (uint32_t*) m->data;

 data[0] = s->dir;
 data[1] = s->pos_rel_id;
 data[2] = s->info.rate;
 data[3] = s->info.bits;
 data[4] = s->info.channels;
 data[5] = s->info.codec;

 for (i = 0; i < _ROAR_STREAM_MESSAGE_LEN/4; i++)
  data[i] = ROAR_HOST2NET32(data[i]);

 ROAR_DBG("roar_stream_s2m(*): s->info:");
 roar_debug_audio_info_print(&s->info);

 m->pos = s->pos;

 return 0;
}
int roar_stream_m2s     (struct roar_stream * s, struct roar_message * m) {
 uint32_t * data;
 int i;

 if ( !(s && m) )
  return -1;

 if ( m->datalen != _ROAR_STREAM_MESSAGE_LEN )
  return -1;

 s->pos = m->pos;

 data = (uint32_t*) m->data;

 for (i = 0; i < _ROAR_STREAM_MESSAGE_LEN/4; i++)
  data[i] = ROAR_NET2HOST32(data[i]);

 s->id            = m->stream;
 s->dir           = data[0];
 s->pos_rel_id    = data[1];
 s->info.rate     = data[2];
 s->info.bits     = data[3];
 s->info.channels = data[4];
 s->info.codec    = data[5];

 ROAR_DBG("roar_stream_m2s(*): s->info:");
 roar_debug_audio_info_print(&s->info);

 return 0;
}

// stream direction funcs:
/*
#define roar_dir2str(x)   ((x) == ROAR_DIR_PLAY   ? "play"   : (x) == ROAR_DIR_MONITOR ? "monitor" : \
                           (x) == ROAR_DIR_FILTER ? "filter" : (x) == ROAR_DIR_RECORD  ? "record"  : \
                           (x) == ROAR_DIR_OUTPUT ? "output" : (x) == ROAR_DIR_BIDIR   ? "bidir"   : \
                           (x) == ROAR_DIR_MIXING ? "mixing" : \
                           "unknown")
*/

struct {
 int    dir;
 char * name;
} _libroar_dir[] = {
 {ROAR_DIR_PLAY,      "play"     },
 {ROAR_DIR_RECORD,    "record"   },
 {ROAR_DIR_MONITOR,   "monitor"  },
 {ROAR_DIR_FILTER,    "filter"   },
 {ROAR_DIR_OUTPUT,    "output"   },
 {ROAR_DIR_MIXING,    "mixing"   },
 {ROAR_DIR_META,      "meta"     },
 {ROAR_DIR_BIDIR,     "bidir"    },
 {ROAR_DIR_THRU,      "thru"     },
 {ROAR_DIR_BRIDGE,    "bridge"   },
 {ROAR_DIR_MIDI_IN,   "midi_in"  },
 {ROAR_DIR_MIDI_OUT,  "midi_out" },
 {ROAR_DIR_LIGHT_IN,  "light_in" },
 {ROAR_DIR_LIGHT_OUT, "light_out"},
 {ROAR_DIR_RAW_IN,    "raw_in"   },
 {ROAR_DIR_RAW_OUT,   "raw_out"  },
 {-1,                 "unknown"  }
};

char * roar_dir2str (int dir) {
 int i;

 for (i = 0; _libroar_dir[i].dir != -1; i++)
  if ( _libroar_dir[i].dir == dir )
   return _libroar_dir[i].name;

 return _libroar_dir[i].name;
}

int roar_str2dir (char * name) {
 int i;

 for (i = 0; _libroar_dir[i].dir != -1; i++)
  if ( !strcmp(_libroar_dir[i].name, name) )
   return _libroar_dir[i].dir;

 return _libroar_dir[i].dir;
}

// codec funcs:

/*
#define roar_codec2str(x) ((x) == ROAR_CODEC_PCM_S_LE  ? "pcm_s_le"  : (x) == ROAR_CODEC_PCM_S_BE  ? "pcm_s_be"  : \
                           (x) == ROAR_CODEC_PCM_S_PDP ? "pcm_s_pdp" : (x) == ROAR_CODEC_MIDI_FILE ? "midi_file" : \
                           "unknown" )
*/

struct {
 int    codec;
 char * name;
} _libroar_codec[] = {
 // PCM:
 {ROAR_CODEC_PCM_S_LE,    "pcm_s_le"   },
 {ROAR_CODEC_PCM_S_BE,    "pcm_s_be"   },
 {ROAR_CODEC_PCM_S_PDP,   "pcm_s_pdp"  },
 {ROAR_CODEC_PCM_U_LE,    "pcm_u_le"   },
 {ROAR_CODEC_PCM_U_BE,    "pcm_u_be"   },
 {ROAR_CODEC_PCM_U_PDP,   "pcm_u_pdp"  },
 {ROAR_CODEC_DEFAULT,     "default"    }, // alias
 {ROAR_CODEC_DEFAULT,     "pcm"        }, // alias
 {ROAR_CODEC_DEFAULT,     "raw"        }, // alias

 // MIDI:
 {ROAR_CODEC_MIDI_FILE,   "midi_file"  },
 {ROAR_CODEC_MIDI,        "midi"       },

 // XIPH:
 {ROAR_CODEC_OGG_VORBIS,  "ogg_vorbis" },
 {ROAR_CODEC_OGG_VORBIS,  "vorbis"     }, // alias
 {ROAR_CODEC_FLAC,        "flac"       },
 {ROAR_CODEC_OGG_SPEEX,   "ogg_speex"  },
 {ROAR_CODEC_OGG_SPEEX,   "speex"      }, // alias
 {ROAR_CODEC_OGG_FLAC,    "ogg_flac"   },
 {ROAR_CODEC_OGG_GENERAL, "ogg_general"},
 {ROAR_CODEC_ROAR_CELT,   "roar_celt"  },
 {ROAR_CODEC_ROAR_SPEEX,  "roar_speex" },

 // RAUM:
 {ROAR_CODEC_RAUM,        "raum"       },
 {ROAR_CODEC_RAUM_VORBIS, "raum_vorbis"},
 {ROAR_CODEC_RAUM_FLAC,   "raum_flac"  },

 // RIFF/WAVE like:
 {ROAR_CODEC_RIFF_WAVE,   "riff_wave"  },
 {ROAR_CODEC_RIFF_WAVE,   "wave"       }, // alias
 {ROAR_CODEC_RIFF_WAVE,   "wav"        }, // alias

 //Log codecs:
 {ROAR_CODEC_ALAW,        "alaw"       },
 {ROAR_CODEC_MULAW,       "mulaw"      },
 {ROAR_CODEC_MULAW,       "ulaw"       }, // alias

 // Meta Codecs:
 {ROAR_CODEC_META_VCLT,     "meta_vclt"    },
 {ROAR_CODEC_META_RALT,     "meta_ralt"    },
 {ROAR_CODEC_META_RALB,     "meta_ralb"    },
 {ROAR_CODEC_META_RALB_LE,  "meta_ralb_le" },
 {ROAR_CODEC_META_RALB_BE,  "meta_ralb_be" },
 {ROAR_CODEC_META_RALB_PDP, "meta_ralb_pdp"},

 // light control:
 {ROAR_CODEC_DMX512,      "dmx512"     },
 {ROAR_CODEC_ROARDMX,     "roardmx"    },

 {-1, NULL}
};

int roar_str2codec(char * codec) {
 int i;
 int guess;

 if ( codec == NULL || *codec == 0 )
  return ROAR_CODEC_DEFAULT;

 if ( (guess = atoi(codec)) > 0 )
  return guess;

 if ( codec == NULL || *codec == 0 )
  return ROAR_CODEC_DEFAULT;

 for (i = 0; _libroar_codec[i].codec != -1; i++)
  if ( strcasecmp(_libroar_codec[i].name, codec) == 0 )
   return _libroar_codec[i].codec;

 return -1;
}


char * roar_codec2str (int codec) {
 int i;

 for (i = 0; _libroar_codec[i].codec != -1; i++)
  if ( _libroar_codec[i].codec == codec )
   return _libroar_codec[i].name;

 return "unknown";
}

char * roar_streamstate2str(int streamstate) {
 switch (streamstate) {
  case ROAR_STREAMSTATE_UNUSED:  return "unused";  break;
  case ROAR_STREAMSTATE_INITING: return "initing"; break;
  case ROAR_STREAMSTATE_NEW:     return "new";     break;
  case ROAR_STREAMSTATE_OLD:     return "old";     break;
  case ROAR_STREAMSTATE_CLOSING: return "closing"; break;
 }

 return "unknown";
}

//ll
