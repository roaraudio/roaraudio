//output.h:

#ifndef _OUTPUT_H_
#define _OUTPUT_H_

#include <roaraudio.h>

//#define ROAR_OUTPUT_BUFFER_SAMPLES 1024
//#define ROAR_OUTPUT_BUFFER_SAMPLES 1024
#ifdef DEBUG
#define ROAR_OUTPUT_BUFFER_SAMPLES 2048
#else
#define ROAR_OUTPUT_BUFFER_SAMPLES 441
#endif

#define ROAR_OUTPUT_WRITE_SIZE     1024

#define ROAR_OUTPUT_CALC_OUTBUFSIZE(x)   (ROAR_OUTPUT_BUFFER_SAMPLES * (x)->channels * ((x)->bits / 8))
#define ROAR_OUTPUT_CALC_OUTBUFSAMP(x,y) ((y) / ((x)->channels * ((x)->bits / 8)))

void         * g_output_buffer;
unsigned int   g_output_buffer_len;

int output_buffer_init   (struct roar_audio_info * info);
int output_buffer_reinit (void);
int output_buffer_free   (void);

int output_buffer_flush  (DRIVER_USERDATA_T inst, int driver);

#endif

//ll
