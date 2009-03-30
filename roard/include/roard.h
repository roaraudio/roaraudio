//roard.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _ROARD_H_
#define _ROARD_H_

// need to include this first as we need the config
#include <roaraudio.h>


#ifndef ROAR_TARGET_MICROCONTROLLER
#include <signal.h>
#endif

#ifdef ROAR_HAVE_SELECT
#include <sys/select.h>
#endif

#ifdef ROAR_HAVE_WAIT
#include <sys/wait.h>
#endif

#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
#include <pwd.h>
#endif

#ifndef ROAR_TARGET_MICROCONTROLLER
#include <sys/time.h>
#include <time.h>
#endif

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
int g_no_listen;

uint32_t g_pos; // current possition in output stream

int g_standby;
int g_autostandby;

int main_loop (int driver, DRIVER_USERDATA_T driver_inst, struct roar_audio_info * sa, int sysclocksync);
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
