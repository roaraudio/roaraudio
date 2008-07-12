//roard.h:

#ifndef _ROARD_H_
#define _ROARD_H_

#include <signal.h>
#include <sys/select.h>
#include <roaraudio.h>
//#include "buffer.h"
#include "client.h"
#include "driver.h"
#include "output.h"
#include "mixer.h"
//#include "convert.h"
#include "streams.h"
#include "network.h"
#include "commands.h"
#include "req.h"
#include "sources.h"
#include "sample.h"
#include "meta.h"

int alive;

int g_standby;

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa);
void clean_quit (void);
void clean_quit_prep (void);

void on_sig_int (int signum);

int g_listen_socket;

int g_self_client;

int g_terminate;

struct roar_audio_info * g_sa;

#endif

//ll
