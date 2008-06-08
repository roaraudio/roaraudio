//output.h:

#ifndef _MIXER_H_
#define _MIXER_H_

#include <roaraudio.h>

int mix_clients       (void * output, int bits, void ** input, int samples);
int mix_clients_8bit  (void * output, void ** input, int samples);
int mix_clients_16bit (void * output, void ** input, int samples);
int mix_clients_24bit (void * output, void ** input, int samples);
int mix_clients_32bit (void * output, void ** input, int samples);

#endif

//ll
