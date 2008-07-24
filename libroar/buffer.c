//buffer.c:

#include "libroar.h"

int roar_buffer_new      (struct roar_buffer ** buf, size_t len) {
 struct roar_buffer * new;

 ROAR_DBG("buffer_new(buf=%p, len=%i) = ?", buf, len);

 if ((new = malloc(sizeof(struct roar_buffer))) == NULL) {
  *buf = NULL;
  return -1;
 }

 if ((new->data = malloc(len)) == NULL) {
  free(new);
  *buf = NULL;
  return -1;
 }

 new->user_data = new->data;

 new->next      = NULL;

 new->len       = len;
 new->user_len  = len;
 *buf           = new;

 ROAR_DBG("buffer_new(buf=%p, len=%i): New buffer at %p", buf, len, new);

 return 0;
}

int roar_buffer_free     (struct roar_buffer * buf) {
 struct roar_buffer * next;

 if ( buf == NULL )
  return -1;

 while ((next = buf->next)) {
  free(buf->data);
  free(buf);
  buf = next;
 }

 free(buf->data);
 free(buf);

 return 0;
}

int roar_buffer_delete   (struct roar_buffer * buf, struct roar_buffer ** next) {
 if ( buf == NULL ) {
  if ( next != NULL )
   *next = NULL;
  return -1;
 }

 ROAR_DBG("buffer_delete(buf=%p, next=%p) = ?", buf, next);

 if ( next != NULL )
  *next = buf->next;

 free(buf->data);
 free(buf);

 ROAR_DBG("buffer_delete(buf=%p, next=%p) = 0", buf, next);
 return 0;
}

int roar_buffer_add      (struct roar_buffer * buf, struct roar_buffer *  next) {
 if ( buf == NULL )
  return -1;

 ROAR_DBG("buffer_add(buf=%p, next=%p) = ?", buf, next);

 while ( buf->next != NULL ) {
  ROAR_DBG("buffer_add(*): buf=%p, next=%p", buf, buf->next);
//  ROAR_DBG("buffer_add(): buf=%p, buf->next=%p", buf, buf->next);
  buf = buf->next;
 }

 buf->next = next;

 return 0;
}

int roar_buffer_get_next (struct roar_buffer *  buf, struct roar_buffer ** next) {
 if ( buf == NULL )
  return -1;

 *next = buf->next;

 return 0;
}

int roar_buffer_get_data (struct roar_buffer *  buf, void   ** data) {
 if ( buf == NULL )
  return -1;

 *data = buf->user_data;

 return 0;
}

int roar_buffer_set_offset (struct roar_buffer *  buf, size_t off) {
 if ( buf == NULL )
  return -1;

 buf->user_len  -= off;
 buf->user_data += off;

 return 0;
}

int roar_buffer_set_meta (struct roar_buffer * buf, void *  meta) {
 if ( buf == NULL )
  return -1;

 buf->meta = meta;

 return 0;
}

int roar_buffer_get_meta (struct roar_buffer * buf, void ** meta) {
 if ( buf == NULL )
  return -1;

 *meta = buf->meta;

 return 0;
}

int roar_buffer_set_len  (struct roar_buffer *  buf, size_t    len) {
 if ( buf == NULL )
  return -1;

 buf->user_len = len;

 return 0;
}

int roar_buffer_get_len  (struct roar_buffer *  buf, size_t *  len) {
 if ( buf == NULL )
  return -1;

 *len = buf->user_len;

 return 0;
}

int roar_buffer_duplicate (struct roar_buffer *  buf, struct roar_buffer ** copy) {
 struct roar_buffer *  cur = buf;
 struct roar_buffer *  new;
 void * od, * nd;

 *copy = NULL;

 while (cur) {
  if ( roar_buffer_new(&new, cur->user_len) == -1 ) {
   roar_buffer_free(*copy);
   return -1;
  }

  if ( *copy == NULL )
   *copy = new;

  roar_buffer_get_data(cur, &od);
  roar_buffer_get_data(new, &nd);
  memcpy(nd, od, cur->user_len);

  roar_buffer_add(*copy, new);

  cur = cur->next;
 }
 return 0;
}

int roar_buffer_ring_stats (struct roar_buffer *  buf, struct roar_buffer_stats * stats) {
 if ( buf == NULL )
  return -1;

 stats->parts        = 0;
 stats->bytes        = 0;
 stats->memory_usage = 0;

 while (buf) {
  stats->parts++;
  stats->bytes        += buf->user_len;
  stats->memory_usage += buf->len + sizeof(struct roar_buffer);
  buf = buf->next;
 }

 return 0;
}

//ll
