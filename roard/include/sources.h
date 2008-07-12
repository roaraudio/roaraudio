//sources.h:

#ifndef _SOURCES_H_
#define _SOURCES_H_

#include <roaraudio.h>

int g_source_client;

int sources_init (void);
int sources_free (void);

int sources_set_client (int client);

int sources_add (char * driver, char * device, char * containe, char * options, int primary);

#endif

//ll
