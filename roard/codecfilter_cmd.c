//codecfilter_cmd.c:

#include "roard.h"

#define CMDMAXLEN 1024

int cf_cmd_open(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter) {
 int socks[2];
 int execed = -1;
 char cmd[CMDMAXLEN+1] = {0};
 char * tmp = NULL;
 char   tb[CMDMAXLEN+1];
 char * o = filter->options;
 int i;

 if ( !o )
  return -1;

 for (i = 0; i < CMDMAXLEN && *o != 0; i++, o++) {
  if ( *o == '%' ) {
//   printf("ol: *o='%c' (escape)\n", *o);
   tmp = NULL;

   o++;
   if ( *o == 0 ) {
    break;
   } else if ( *o == 'R' ) {
    tmp = tb;
    snprintf(tb, CMDMAXLEN, "%i", g_sa->rate);
   } else if ( *o == 'B' ) {
    tmp = tb;
    snprintf(tb, CMDMAXLEN, "%i", g_sa->bits);
   } else if ( *o == 'C' ) {
    tmp = tb;
    snprintf(tb, CMDMAXLEN, "%i", g_sa->channels);
   }

//   printf("*o='%c', tmp=%p\n", *o, tmp);

   if ( tmp ) {
    for (; i < CMDMAXLEN && *tmp != 0; i++, tmp++)
     cmd[i] = *tmp;
    i--;
   }
  } else {
//   printf("ol: *o='%c' (copy to cmd[i=%i])\n", *o, i);
   cmd[i] = *o;
  }
 }

 cmd[i+1] = 0;

 //printf("cmd='%s'\n", cmd);

 if ( socketpair(AF_UNIX, SOCK_STREAM, 0, socks) == -1 ) {
  return -1;
 }

 if ( lib_run_bg(cmd, ((struct roar_stream*)info)->fh, socks[1], ROAR_STDERR, socks, 2) == -1 )
  return -1;

 if ( info->client != -1 ) {
  execed = g_clients[info->client]->execed;

  if ( execed != -1 ) {
   if ( g_streams[execed] == info ) {
    g_clients[info->client]->fh = socks[0];
   } else {
    close(((struct roar_stream*)info)->fh);
   }
  } else {
   close(((struct roar_stream*)info)->fh);
  }
 } else {
  close(((struct roar_stream*)info)->fh);
 }

 ((struct roar_stream*)info)->fh = socks[0];
 ((struct roar_stream*)info)->info.codec = ROAR_CODEC_DEFAULT;
 close(socks[1]);

 info->codecfilter = -1;

 return 0;
}

//ll
