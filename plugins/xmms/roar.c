//roar.c:

#include <roaraudio.h>

//#include "xmms/i18n.h"
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include "xmms/plugin.h"
#include "xmms/xmmsctrl.h"
#include "xmms/dirbrowser.h"
#include "xmms/configfile.h"
#include "xmms/util.h"

void roar_init(void);
void roar_about(void);
void roar_configure(void);

void roar_get_volume(int *l, int *r);
void roar_fetch_volume(int *l, int *r);
void roar_set_volume(int l, int r);
void roar_mixer_init(void);
void roar_mixer_init_vol(int l, int r);

int roar_playing(void);
int roar_free(void);
void roar_write(void *ptr, int length);
void roar_close(void);
void roar_flush(int time);
void roar_pause(short p);
int roar_open(AFormat fmt, int rate, int nch);
int roar_get_output_time(void);
int roar_get_written_time(void);
void roar_set_audio_params(void);


OutputPlugin roar_op = {
        NULL,
        NULL,
        "RoarAudio XMMS Plugin", /* Description */
        roar_init,
        NULL, //roar_about,
        NULL, //roar_configure,
        NULL, //roar_get_volume,
        NULL, //roar_set_volume,
        roar_open,
        roar_write,
        roar_close,
        NULL, //roar_flush,
        NULL, //roar_pause,
        roar_free,
        roar_playing,
        NULL, //roar_get_output_time,
        NULL, //roar_get_written_time,
};

#define STATE_CONNECTED 1
#define STATE_PLAYING   2

struct xmms_roar_out {
 int state;
 char * server;
 struct roar_connection con;
 struct roar_stream     stream;
 int data_fh;
} g_inst;

OutputPlugin *get_oplugin_info(void) {
 return &roar_op;
}

void roar_init(void) {
 g_inst.state = 0;
 g_inst.server = NULL;
 ROAR_WARN("roar_init(*) = (void)");
}

int roar_playing(void) {
 return g_inst.state & STATE_PLAYING ? TRUE : FALSE;
}

void roar_write(void *ptr, int length) {
 write(g_inst.data_fh, ptr, length);
}

int roar_open(AFormat fmt, int rate, int nch) {
 int codec = ROAR_CODEC_DEFAULT;
 int bits;

 if ( !(g_inst.state & STATE_CONNECTED) ) {
  if ( roar_simple_connect(&(g_inst.con), g_inst.server, "XMMS") == -1 ) {
   return FALSE;
  }
  g_inst.state |= STATE_CONNECTED;
 }

  bits = 16;
  switch (fmt) {
   case FMT_S8:
     bits = 8;
     codec = ROAR_CODEC_DEFAULT;
    break;
   case FMT_U8:
     bits = 8;
     codec = ROAR_CODEC_PCM_S_LE; // _LE, _BE, _PDP,... all the same for 8 bit output
    break;
   case FMT_U16_LE:
     codec = ROAR_CODEC_PCM_U_LE;
    break;
   case FMT_U16_BE:
     codec = ROAR_CODEC_PCM_U_BE;
    break;
   case FMT_U16_NE:
#if BYTE_ORDER == BIG_ENDIAN
     codec = ROAR_CODEC_PCM_U_BE;
#elif BYTE_ORDER == LITTLE_ENDIAN
     codec = ROAR_CODEC_PCM_U_LE;
#else
     codec = ROAR_CODEC_PCM_U_PDP;
#endif
    break;
   case FMT_S16_LE:
     codec = ROAR_CODEC_PCM_S_LE;
    break;
   case FMT_S16_BE:
     codec = ROAR_CODEC_PCM_S_BE;
    break;
   case FMT_S16_NE:
     codec = ROAR_CODEC_DEFAULT;
    break;
 }

 if ( (g_inst.data_fh = roar_simple_new_stream_obj(&(g_inst.con), &(g_inst.stream),
                              rate, nch, bits, codec, ROAR_DIR_PLAY)) == -1) {
  return FALSE;
 }
 g_inst.state |= STATE_PLAYING;

 return TRUE;
}

void roar_close(void) {
 close(g_inst.data_fh);
 g_inst.state |= STATE_PLAYING;
 g_inst.state -= STATE_PLAYING;
}

void roar_pause(short p);
int roar_get_output_time(void);
int roar_get_written_time(void);

int roar_free(void) {
 return 1000000; // ???
}

//ll
