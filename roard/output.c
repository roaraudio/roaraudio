//output.c:

#include "roard.h"

/*
void         * g_output_buffer;
unsigned int   g_output_buffer_len;
*/

int output_buffer_init   (struct roar_audio_info * info) {
 size_t   size; // = ROAR_OUTPUT_BUFFER_SAMPLES;
 void   * buf;

/*
 size *= info->channels;
 size *= info->bits / 8;
*/

 size = ROAR_OUTPUT_CALC_OUTBUFSIZE(info);

 ROAR_DBG("output_buffer_init(*): output buffer size is %i", size);

 if ( (buf = malloc(size)) == NULL )
  return -1;

 g_output_buffer     = buf;
 g_output_buffer_len = size;

 ROAR_DBG("output_buffer_init(*): output buffer is at %p", buf);

#ifdef ROAR_HAVE_MLOCK
 ROAR_MLOCK(buf, size);
#endif

 output_buffer_reinit();

 return 0;
}

int output_buffer_reinit (void) {

 if ( g_output_buffer != NULL )
  memset(g_output_buffer, 0, g_output_buffer_len);

 return 0;
}

int output_buffer_free   (void) {
 ROAR_DBG("output_buffer_init(*): freeing output buffer at %p", g_output_buffer);

 if ( g_output_buffer )
  free(g_output_buffer);

 g_output_buffer     = NULL;
 g_output_buffer_len = 0;

 return 0;
}

int output_buffer_flush  (DRIVER_USERDATA_T inst, int driver) {
 ROAR_DBG("output_buffer_init(*): flushing output buffer");

 return driver_write(inst, driver, g_output_buffer, g_output_buffer_len);
}

//ll
