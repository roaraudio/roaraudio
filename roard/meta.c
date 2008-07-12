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

   if ( (c = malloc(strlen(val))) == NULL ) {
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
 int i;
 struct roar_stream_server * s = g_streams[id];

 if ( s == NULL )
  return -1;

 for (i = 0; i < ROAR_META_MAX_PER_STREAM; i++) {
  if ( s->meta[i].type == type ) {
   if ( name != NULL )
    if ( strncmp(s->meta[i].key, name, ROAR_META_MAX_NAMELEN) != 0 )
     continue;

   if ( strlen(s->meta[i].value) > (len - 1) )
    return -1;

   strcpy(val, s->meta[i].value);

   return 0;
  }
 }

 return -1;
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
