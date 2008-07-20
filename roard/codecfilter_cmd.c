//codecfilter_cmd.c:

#include "roard.h"

int cf_cmd_open(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter) {
 int socks[2];

 if ( socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1 ) {
  return -1;
 }

 if ( lib_run_bg(filter->options, ((struct roar_stream*)info)->fh, socks[1], ROAR_STDERR) == -1 )
  return -1;

 close(((struct roar_stream*)info)->fh);
 ((struct roar_stream*)info)->fh = socks[0];
 ((struct roar_stream*)info)->info.codec = ROAR_CODEC_DEFAULT;
 close(socks[1]);

 info->codecfilter = -1;

 return 0;
}

//ll
