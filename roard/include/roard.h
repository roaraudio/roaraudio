//roard.h:

#ifndef _ROARD_H_
#define _ROARD_H_

#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <roaraudio.h>
#include <pwd.h>

/*
#ifdef __linux__
#include <linux/unistd.h>
#include <linux/ioprio.h>
#endif
*/


//#include "buffer.h"
#include "codecfilter.h"
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
#include "midi.h"
#include "lib.h"


int alive;

uint32_t g_pos; // current possition in output stream

int g_standby;

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa);
void cleanup_listen_socket (int terminate);
void clean_quit (void);
void clean_quit_prep (void);

void on_sig_int (int signum);
void on_sig_chld (int signum);

int g_listen_socket;

int g_self_client;

int g_terminate;

struct roar_audio_info * g_sa;

#endif

//ll
