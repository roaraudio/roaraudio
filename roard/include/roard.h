//roard.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _ROARD_H_
#define _ROARD_H_

// configure ROAR_INFO()
int g_verbose;
#define ROAR_DBG_INFOVAR g_verbose

// configure ROAR_*()
#define ROAR_DBG_PREFIX "roard"

// need to include this first as we need the config
#include <roaraudio.h>
#include <roaraudio/units.h>
#include <libroarlight/libroarlight.h>
#include <libroareio/libroareio.h>

#include <ctype.h>

#ifdef ROAR_HAVE_H_SIGNAL
#include <signal.h>
#endif

#if defined(ROAR_HAVE_SELECT) && defined(ROAR_HAVE_H_SYS_SELECT)
#include <sys/select.h>
#endif

#ifdef ROAR_HAVE_WAIT
#include <sys/wait.h>
#endif

#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
#include <pwd.h>
#endif

#ifdef ROAR_HAVE_H_SYS_TIME
#include <sys/time.h>
#endif
#ifdef ROAR_HAVE_H_TIME
#include <time.h>
#endif

#ifdef ROAR_HAVE_LIBSLP
#include <slp.h>
#endif

/*
#ifdef __linux__
#include <linux/unistd.h>
#include <linux/ioprio.h>
#endif
*/

#define ROAR_SUBSYS_NONE          0x00
#define ROAR_SUBSYS_WAVEFORM      0x01
#define ROAR_SUBSYS_MIDI          0x02
#define ROAR_SUBSYS_CB            0x04
#define ROAR_SUBSYS_LIGHT         0x08
#define ROAR_SUBSYS_RAW           0x10
#define ROAR_SUBSYS_COMPLEX       0x20
#define ROAR_SUBSYS_RDTCS         0x40

// listeing code:
#define ROAR_MAX_LISTEN_SOCKETS  8

union uniinst {
 void * vp;
 int    si;
};

// MFOI = Marked For Optional Include

//#include "buffer.h"
#include "caps.h"
#include "memlock.h"
#include "codecfilter.h" /* MFOI */
#include "container_framework.h" /* MFOI */
#include "client.h"
#include "auth.h"
#include "driver.h" /* MFOI */
#include "output.h"
#include "mixer.h"
//#include "convert.h"
#include "streams.h"
#include "network.h"
#include "commands.h"
#include "req.h"
#include "emul_esd.h" /* MFOI */
#include "emul_simple.h" /* MFOI */
#include "emul_rsound.h" /* MFOI */
#include "emul_rplay.h" /* MFOI */
#include "sources.h"
#include "sample.h"
#include "hwmixer.h"
#include "meta.h"
#include "midi.h"
#include "ssynth.h"
#include "light.h"
#include "raw.h"
#include "rdtcs.h"
#include "waveform.h"
#include "beep.h"
#include "lib.h"
#include "plugins.h"


int alive;
#ifdef ROAR_SUPPORT_LISTEN
int g_no_listen;
#endif

uint32_t g_pos; // current possition in output stream

int g_standby;
int g_autostandby;

#ifdef ROAR_SUPPORT_LISTEN
struct roard_listen;
#endif

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa, int sysclocksync);
void cleanup_listen_socket (int terminate);
void clean_quit (void);
void clean_quit_prep (void);

void on_sig_int  (int signum);
void on_sig_term (int signum);
void on_sig_chld (int signum);
void on_sig_usr1 (int signum);

#ifdef ROAR_SUPPORT_LISTEN
int get_listen(struct roard_listen ** sock, char *** sockname);
#endif

#ifdef ROAR_SUPPORT_LISTEN
struct roard_listen {
 int used;
 struct roar_vio_calls sock;
 int proto;
 union {
  void * vp;
  int    si;
  struct {
   int                    dir;
   struct roar_audio_info info;
  } stpl;
 } inst;
} g_listen[ROAR_MAX_LISTEN_SOCKETS];
#endif

int g_self_client;

int g_terminate;

struct roar_audio_info * g_sa, * g_max_sa;

#define CONF_DEF_STRING "***default***"

struct roard_config {
 uint32_t flags;
 struct {
  uint32_t flags;
  struct roar_mixer_settings mixer;
  int                        mixer_channels;
 } streams[ROAR_DIR_DIRIDS];
 char * location;
 char * description;
 size_t jumbo_mtu;
 int memlock_level;
} * g_config;

struct _counters {
 size_t clients, streams,
        samples,
        sources, outputs,
        mixers, bridges,
        listens;
};

struct {
 struct _counters cur, sum;
} g_counters;


#define counters_init() memset(&g_counters, 0, sizeof(g_counters))
//#define counters_inc(c,i) do { signed long int __i = (i); if ( __i < 0 && __i > (g_counters.cur.c) ) { ROAR_WARN("counters_inc(c=%s, i=%li): Decrement request bigger than current counter value. Counter out of syn!", )  } while (0)
#define counters_inc(c,i) do { signed long int __i = (i); g_counters.cur.c += __i; if ( __i > 0 ) g_counters.sum.c += __i; } while (0)

#define counters_get(g,c) ((g_counters.g.c))

void counters_print(int type, int force);

#endif

//ll
