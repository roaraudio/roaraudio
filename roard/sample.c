//sample.c:

#include "roard.h"

int samples_init      (void) {
 int i;

 for (i = 0; i < ROAR_SAMPLES_MAX; i++)
  g_samples[i] = NULL;

 return 0;
}


int samples_free      (void);
int samples_new       (void);
int samples_delete    (int id);
int samples_set_name  (int id, char * name);

//ll
