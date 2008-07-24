//esdfile.c:

#include "libroaresd.h"

/*******************************************************************/
/* esdfile.c - audiofile wrappers for sane handling of files */

int esd_send_file( int esd, AFfilehandle au_file, int frame_length ) {
 return -1;
}
int esd_play_file( const char *name_prefix, const char *filename, int fallback ) {
 // TODO: add support for fallback
 return roar_simple_play_file((char*)filename, NULL, (char*)name_prefix) == -1 ? -1 : 0;
}
int esd_file_cache( int esd, const char *name_prefix, const char *filename ) {
 return -1;
}


//ll
