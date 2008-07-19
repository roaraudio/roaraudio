//codecfilter.c:

#include "roard.h"

struct roar_codecfilter g_codecfilter[] = {
 {-1,                     "null", "null codec filter", NULL,                      NULL, NULL, NULL, NULL, NULL, NULL},
 {ROAR_CODEC_OGG_GENERAL, "cmd",  "ogg123",            "ogg123 -q -d raw -f - -", NULL, NULL, NULL, NULL, NULL, NULL},

 {-1, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL} // end of list
};

void print_codecfilterlist (void) {
 int i;

 for (i = 0; g_codecfilter[i].name != NULL; i++) {
  printf("  %-8s %-8s - %s (options: %s)\n",
             roar_codec2str(g_codecfilter[i].codec),
             g_codecfilter[i].name,
             g_codecfilter[i].desc,
             g_codecfilter[i].options);
 }
}

int codecfilter_close(CODECFILTER_USERDATA_T   inst, int codecfilter) {
 ROAR_DBG("codecfilter_close(inst=%p, codecfilter=%i) = ?", inst, codecfilter);

 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].close )
  return g_codecfilter[codecfilter].close(inst);

 return 0;
}

int codecfilter_pause(CODECFILTER_USERDATA_T   inst, int codecfilter, int newstate) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].pause )
  return g_codecfilter[codecfilter].pause(inst, newstate);

 return 0;
}

int codecfilter_write(CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].write )
  return g_codecfilter[codecfilter].write(inst, buf, len);

 return 0;
}

int codecfilter_read (CODECFILTER_USERDATA_T   inst, int codecfilter, char * buf, int len) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].read )
  return g_codecfilter[codecfilter].read(inst, buf, len);

 return 0;
}

int codecfilter_flush(CODECFILTER_USERDATA_T   inst, int codecfilter) {
 if ( codecfilter == -1 )
  return -1;

 if ( g_codecfilter[codecfilter].flush )
  return g_codecfilter[codecfilter].flush(inst);

 return 0;
}

//ll
