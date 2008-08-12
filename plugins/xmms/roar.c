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


OutputPlugin esd_op =
{
        NULL,
        NULL,
        "RoarAudio XMMS Plugin", /* Description */
        roar_init,
        roar_about,
        roar_configure,
        roar_get_volume,
        roar_set_volume,
        roar_open,
        roar_write,
        roar_close,
        roar_flush,
        roar_pause,
        roar_free,
        roar_playing,
        roar_get_output_time,
        roar_get_written_time,
};

//ll
