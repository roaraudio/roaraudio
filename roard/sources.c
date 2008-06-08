//sources.c:

#include "roard.h"

int sources_init (void) {
 g_source_client = -1;
 return 0;
}


int sources_free (void) {
 return 0;
}

int sources_add (char * driver, char * device, char * containe, char * options, int primary) {
 return -1;
}

//ll
