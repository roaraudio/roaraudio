//meta.c:

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
 
 {-1, "EOL"}
};

char * roar_meta_strtype(int type) {
 int i;
 static char name[24];

 for (i = 0; _libroar_meta_typelist[i].id != -1; i++)
  if ( _libroar_meta_typelist[i].id == type ) {
   strcpy(name, _libroar_meta_typelist[i].name);
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

 m.cmd     = ROAR_CMD_SET_META;
 m.stream  = s->id;
// m.datalen = len;

 m.data[0] = 0;
 m.data[1] = mode;
 m.data[2] = meta->type;

 m.data[3] = strlen(meta->key);
 m.data[4] = len = strlen(meta->value);

 if ( len > 255 )
  return -1;

 m.datalen = 5 + m.data[3] + m.data[4];
 if ( m.datalen > LIBROAR_BUFFER_MSGDATA )
  return -1;

 strncpy(&(m.data[5]), meta->key, ROAR_META_MAX_NAMELEN);
 strncpy(&(m.data[5+m.data[3]]), meta->value, len);

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
