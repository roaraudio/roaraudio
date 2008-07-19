//codecfilter.c:

#include "roard.h"

struct roar_codecfilter g_codecfilter[] = {
 {-1, "null", "null codec filter", NULL,                      NULL, NULL, NULL, NULL, NULL, NULL},
 {-1, "cmd",  "ogg123",            "ogg123 -q -d raw -f - -", NULL, NULL, NULL, NULL, NULL, NULL},
 {-1, NULL,   NULL,                NULL,                      NULL, NULL, NULL, NULL, NULL, NULL} // end of list
};

void print_codecfilterlist (void) {
 int i;

 for (i = 0; g_codecfilter[i].name != NULL; i++) {
  printf("  %.2x %-8s - %s (options: %s)\n",
             g_codecfilter[i].codec, g_codecfilter[i].name, g_codecfilter[i].desc, g_codecfilter[i].options);
 }
}

//ll
