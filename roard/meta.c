//meta.c:

#include "roard.h"

int stream_meta_set   (int id, int type, char * name, char * val) {
 int i;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++)
  if ( s->meta[i].type == type ) {
   s->meta[i].type = ROAR_META_TYPE_NONE;
   if ( s->meta[i].value )
    free(s->meta[i].value);
   s->meta[i].value = NULL;
  }

 return stream_meta_add(id, type, name, val);
}

int stream_meta_add   (int id, int type, char * name, char * val) {
 int i;
 char * c;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  if ( s->meta[i].type == ROAR_META_TYPE_NONE ) {
   s->meta[i].type = type;

   if ( name == NULL ) {
    s->meta[i].key[0] = 0;
   } else {
    strncpy(s->meta[i].key, name, ROAR_META_MAX_NAMELEN);
   }

   if ( (c = malloc(strlen(val)+1)) == NULL ) {
    s->meta[i].type = ROAR_META_TYPE_NONE;
    s->meta[i].key[0] = 0;
    return -1;
   }

   strcpy(c, val);
   s->meta[i].value = c;

   ROAR_DBG("stream_meta_add(id=%i, type=%i, name='%s', val='%s') = 0", id, type, name, val);

   return 0;
  }
 }

 return -1;
}

int stream_meta_get   (int id, int type, char * name, char * val, size_t len) {
 int i, vallen;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  if ( s->meta[i].type == type ) {
   if ( name != NULL )
    if ( strncmp(s->meta[i].key, name, ROAR_META_MAX_NAMELEN) != 0 )
     continue;

   if ( s->meta[i].value == NULL )
    return -1;

   if ( (vallen = strlen(s->meta[i].value)) > (len - 1) ) {
    ROAR_DBG("stream_meta_get(*): val too small: need %i have %i", vallen, len);
    return -1;
   }

   strncpy(val, s->meta[i].value, vallen);
   val[vallen] = 0;

   return 0;
  }
 }

 return -1;
}

int stream_meta_list  (int id, int * types, size_t len) {
 int i, j;
 int have = 0;
 int found;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 if ( len < ROAR_META_MAX_PER_STREAM ) // TODO: find a way we do not need this
  return -1;

 types[0] = -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  found = 0;
  for (j = 0; j < have; j++)
   if ( types[j] == s->meta[i].type ) {
    found = 1;
    break;
   }

  if ( !found )
   types[have++] = s->meta[i].type;
 }

 return have;
}

int stream_meta_clear (int id) {
 int i;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  s->meta[i].type   = ROAR_META_TYPE_NONE;
  if ( s->meta[i].value )
   free(s->meta[i].value);
  s->meta[i].value  = NULL;
  s->meta[i].key[0] = 0;
 }

 return 0;
}

//ll
