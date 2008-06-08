//esdsample.c:

#include "libroaresd.h"

// TODO: add support for all of thiese funcs

/* cache a sample in the server returns sample id, < 0 = error */
int esd_sample_cache( int esd, esd_format_t format, const int rate,
                      const int length, const char *name ) {
 return -1;
}
int esd_confirm_sample_cache( int esd ) {
 return -1;
}

/* get the sample id for an already-cached sample */
int esd_sample_getid( int esd, const char *name) {
 return -1;
}

/* uncache a sample in the server */
int esd_sample_free( int esd, int sample ) {
 return -1;
}

/* play a cached sample once */
int esd_sample_play( int esd, int sample ) {
 return -1;
}
/* make a cached sample loop */
int esd_sample_loop( int esd, int sample ) {
 return -1;
}

/* stop the looping sample at end */
int esd_sample_stop( int esd, int sample ) {
 return -1;
}
/* stop a playing sample immed. */
int esd_sample_kill( int esd, int sample ) {
 return -1;
}


//ll
