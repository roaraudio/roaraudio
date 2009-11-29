//meta.c:

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

/*

grep ^'#define ROAR_META_TYPE_' meta.h | cut -d' ' -f2 | while read line; do printf ' {%-30s     "%-16s},\n' $line, $(echo $line | cut -d_ -f4)\"; done

*/

struct {
 int    id;
 char * name;
} _libroar_meta_typelist[] = {
 {ROAR_META_TYPE_NONE,               "NONE"           },
 {ROAR_META_TYPE_TITLE,              "TITLE"          },
 {ROAR_META_TYPE_ALBUM,              "ALBUM"          },
 {ROAR_META_TYPE_AUTHOR,             "AUTHOR"         },
 {ROAR_META_TYPE_AUTOR,              "AUTOR"          },
 {ROAR_META_TYPE_ARTIST,             "ARTIST"         },
 {ROAR_META_TYPE_VERSION,            "VERSION"        },
 {ROAR_META_TYPE_DATE,               "DATE"           },
 {ROAR_META_TYPE_LICENSE,            "LICENSE"        },
 {ROAR_META_TYPE_TRACKNUMBER,        "TRACKNUMBER"    },
 {ROAR_META_TYPE_ORGANIZATION,       "ORGANIZATION"   },
 {ROAR_META_TYPE_DESCRIPTION,        "DESCRIPTION"    },
 {ROAR_META_TYPE_GENRE,              "GENRE"          },
 {ROAR_META_TYPE_LOCATION,           "LOCATION"       },
 {ROAR_META_TYPE_CONTACT,            "CONTACT"        },
 {ROAR_META_TYPE_STREAMURL,          "STREAMURL"      },
 {ROAR_META_TYPE_HOMEPAGE,           "HOMEPAGE"       },
 {ROAR_META_TYPE_THUMBNAIL,          "THUMBNAIL"      },
 {ROAR_META_TYPE_LENGTH,             "LENGTH"         },
 {ROAR_META_TYPE_COMMENT,            "COMMENT"        },
 {ROAR_META_TYPE_OTHER,              "OTHER"          },
 {ROAR_META_TYPE_FILENAME,           "FILENAME"       },
 {ROAR_META_TYPE_FILEURL,            "FILEURL"        },
 {ROAR_META_TYPE_SERVER,             "SERVER"         },
 {ROAR_META_TYPE_DURATION,           "DURATION"       },
 {ROAR_META_TYPE_WWW,                "WWW"            },
 {ROAR_META_TYPE_WOAF,               "WOAF"           },
 {ROAR_META_TYPE_ENCODER,            "ENCODER"        },
 {ROAR_META_TYPE_ENCODEDBY,          "ENCODEDBY"      },
 {ROAR_META_TYPE_YEAR,               "YEAR"           },
 {ROAR_META_TYPE_DISCID,             "DISCID"         },
 {ROAR_META_TYPE_RPG_TRACK_PEAK,     "REPLAYGAIN_TRACK_PEAK" },
 {ROAR_META_TYPE_RPG_TRACK_GAIN,     "REPLAYGAIN_TRACK_GAIN" },
 {ROAR_META_TYPE_RPG_ALBUM_PEAK,     "REPLAYGAIN_ALBUM_PEAK" },
 {ROAR_META_TYPE_RPG_ALBUM_GAIN,     "REPLAYGAIN_ALBUM_GAIN" },
 {ROAR_META_TYPE_HASH,               "HASH"           },
 {ROAR_META_TYPE_SIGNALINFO,         "SIGNALINFO"     },
 {ROAR_META_TYPE_AUDIOINFO,          "AUDIOINFO"      },
 {ROAR_META_TYPE_OFFSET,             "OFFSET"         },
 
 {-1, "EOL"}
};

char * roar_meta_strtype(int type) {
 int i;
 static char name[ROAR_META_MAX_NAMELEN];

 for (i = 0; _libroar_meta_typelist[i].id != -1; i++)
  if ( _libroar_meta_typelist[i].id == type ) {
   strncpy(name, _libroar_meta_typelist[i].name, ROAR_META_MAX_NAMELEN);
   return name;
  }

 return NULL;
}

int    roar_meta_inttype(char * type) {
 int i;

 for (i = 0; _libroar_meta_typelist[i].id != -1; i++)
  if ( strcasecmp(_libroar_meta_typelist[i].name, type) == 0 ) {
   return _libroar_meta_typelist[i].id;
  }

 return -1;
}

int roar_stream_meta_set (struct roar_connection * con, struct roar_stream * s, int mode, struct roar_meta * meta) {
 struct roar_message m;
 int len;

 memset(&m, 0, sizeof(struct roar_message)); // make valgrind happy!

 m.cmd     = ROAR_CMD_SET_META;
 m.stream  = s->id;
// m.datalen = len;

 if ( (mode == ROAR_META_MODE_FINALIZE || mode == ROAR_META_MODE_CLEAR) && meta->value == NULL )
  meta->value = "";

 if ( meta->value == NULL )
   return -1;

 m.data[0] = 0;
 m.data[1] = mode;
 m.data[2] = meta->type;

 m.data[3] = strlen(meta->key);
 m.data[4] = len = strlen(meta->value);

 if ( len > 255 )
  return -1;

 m.datalen = (int) 5 + (int) m.data[3] + len;
 if ( m.datalen > LIBROAR_BUFFER_MSGDATA )
  return -1;

 strncpy(&(m.data[5]), meta->key, ROAR_META_MAX_NAMELEN);
 strncpy(&(m.data[5+m.data[3]]), meta->value, len);

 ROAR_DBG("roar_stream_meta_set(*): meta value length is %i byte", len);
 ROAR_DBG("roar_stream_meta_set(*): message data length is %i byte", m.datalen);

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd == ROAR_CMD_OK )
  return 0;
 return -1;
}

int roar_stream_meta_get (struct roar_connection * con, struct roar_stream * s, struct roar_meta * meta) {
 struct roar_message m;
 char * c;

 m.cmd     = ROAR_CMD_GET_META;
 m.stream  = s->id;
// m.datalen = len;

 m.data[0] = 0;
 m.data[1] = meta->type;
 m.datalen = 2;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 if ( m.datalen < 2 )
  return -1;

 if ( m.data[0] != 0 )
  return -1;

 if ( (c = malloc(((unsigned) m.data[1]) + 1)) == NULL )
  return -1;

 strncpy(c, &(m.data[2]), (unsigned) m.data[1]);
 c[(unsigned) m.data[1]] = 0;

 meta->value  = c;
 meta->key[0] = 0;

 return 0;
}

int roar_stream_meta_list (struct roar_connection * con, struct roar_stream * s, int * types, size_t len) {
 int i;
 struct roar_message m;

 m.cmd     = ROAR_CMD_LIST_META;
 m.stream  = s->id;
 m.pos     = 0;

 m.data[0] = 0;
 m.datalen = 1;

 if ( roar_req(con, &m, NULL) == -1 )
  return -1;

 if ( m.cmd != ROAR_CMD_OK )
  return -1;

 if ( m.datalen < 1 )
  return -1;

 if ( m.data[0] != 0 )
  return -1;

 if ( len < (m.datalen - 1 ) )
  return -1;

 for (i = 1; i < m.datalen; i++)
  types[i-1] = (unsigned) m.data[i];

 return m.datalen - 1;
}

int roar_meta_free (struct roar_meta * meta) {
 if ( meta->value )
  free(meta->value);

 return 0;
}

//ll
