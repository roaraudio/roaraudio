//codecfilter_cmd.h:

#ifndef _CODECFILTER_CMD_H_
#define _CODECFILTER_CMD_H_

#include <roaraudio.h>

int cf_cmd_open(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter);

#endif

//ll
