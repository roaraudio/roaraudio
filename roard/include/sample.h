//sample.h:

#ifndef _SAMPLE_H_
#define _SAMPLE_H_

#include <roaraudio.h>

#define ROAR_SAMPLES_MAX  32

struct roar_sample * g_samples[ROAR_SAMPLES_MAX];

int samples_init (void);
int samples_free      (void);
int samples_new       (void);
int samples_delete    (int id);
int samples_set_name  (int id, char * name);

#endif

//ll
