//output.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of roard a part of RoarAudio,
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

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
