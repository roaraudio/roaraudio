//codecfilter_wave.h:

#ifndef _CODECFILTER_WAVE_H_
#define _CODECFILTER_WAVE_H_

#include <roaraudio.h>

struct codecfilter_wave_inst {
 struct roar_stream_server * stream;
 int opened;
};


int cf_wave_open(CODECFILTER_USERDATA_T * inst, int codec,
                                            struct roar_stream_server * info,
                                            struct roar_codecfilter   * filter);

int cf_wave_close(CODECFILTER_USERDATA_T   inst);

int cf_wave_read(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cf_wave_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);

#endif

//ll
