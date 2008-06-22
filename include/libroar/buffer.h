//buffer.h:

#ifndef _LIBROAR_BUFFER_H_
#define _LIBROAR_BUFFER_H_

#include <roaraudio.h>

struct roar_buffer {
 size_t               len;
 size_t               user_len;
 void               * data;
 void               * user_data;
 void               * meta;
 struct roar_buffer * next;
};

struct roar_buffer_stats {
 int parts;
 int bytes;
 int memory_usage;
};

#define roar_buffer_next(a) roar_buffer_delete(*(a), (a));

int roar_buffer_new      (struct roar_buffer ** buf, size_t len);
int roar_buffer_free     (struct roar_buffer *  buf);
int roar_buffer_delete   (struct roar_buffer *  buf, struct roar_buffer ** next);
int roar_buffer_add      (struct roar_buffer *  buf, struct roar_buffer *  next);

int roar_buffer_get_next (struct roar_buffer *  buf, struct roar_buffer ** next);

int roar_buffer_get_data (struct roar_buffer *  buf, void   ** data);

int roar_buffer_set_offset (struct roar_buffer *  buf, size_t off);

int roar_buffer_set_meta (struct roar_buffer *  buf, void   *  meta);
int roar_buffer_get_meta (struct roar_buffer *  buf, void   ** meta);

int roar_buffer_set_len  (struct roar_buffer *  buf, size_t    len);
int roar_buffer_get_len  (struct roar_buffer *  buf, size_t *  len);

int roar_buffer_duplicate (struct roar_buffer *  buf, struct roar_buffer ** copy);

int roar_buffer_ring_stats (struct roar_buffer *  buf, struct roar_buffer_stats * stats);

#endif

//ll
