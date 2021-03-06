//midi.h:

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

#ifndef _MIDI_H_
#define _MIDI_H_

#include <roaraudio.h>

// CB has MIDI as dep
#if defined(ROAR_WITHOUT_DCOMP_MIDI) && !defined(ROAR_WITHOUT_DCOMP_CB)
#define ROAR_WITHOUT_DCOMP_CB
#endif

#ifndef ROAR_WITHOUT_DCOMP_MIDI

#ifndef ROAR_WITHOUT_DCOMP_CB
#ifdef __linux__
#include <sys/ioctl.h>
#include <linux/kd.h>
#endif

#define MIDI_CB_NOOVERRIDE 0
#define MIDI_CB_OVERRIDE   1
#endif

#define MIDI_RATE    31250

// standard MIDI commands:
#define MIDI_TYPE_NOTE_OFF      0x80
#define MIDI_TYPE_NOTE_ON       0x90
#define MIDI_TYPE_PA            0xA0
#define MIDI_TYPE_CONTROLER     0xB0
#define MIDI_TYPE_PROGRAM       0xC0
#define MIDI_TYPE_MA            0xD0
#define MIDI_TYPE_PB            0xE0
#define MIDI_TYPE_SYSEX         0xF0
#define MIDI_TYPE_CLOCK_TICK    0xF8
#define MIDI_TYPE_CLOCK_START   0xFA
#define MIDI_TYPE_CLOCK_STOP    0xFC

// RoarAudio MIDI Commands:
#define MIDI_TYPE_NONE          0x00
#define MIDI_TYPE_RAW_PASS      0x10
#define MIDI_TYPE_RAW_NOPASS    0x20

// controller events:
#define MIDI_CCE_MAIN_VOL        7
#define MIDI_CCE_BALANCE         8
#define MIDI_CCE_PANORAMA       10
#define MIDI_CCE_LOCAL_CONTR   122
#define MIDI_CCE_ALL_NOTE_OFF  123


#define MIDI_MES_BUFSIZE        4
#define MIDI_READ_SIZE          256 /* this should be big enoth in both cfreq=20..100Hz mode */

#define MIDI_MFLAG_FREE_DP     (1<<0)

#define MIDI_INITED_MAIN        0x01
#define MIDI_INITED_CB          0x02
#define MIDI_INITED_CLOCK       0x04

struct {
 int init;
 int inited;

#ifndef ROAR_WITHOUT_DCOMP_CB
 int init_cb;
 char * console_dev;
#endif
} midi_config;

struct midi_message {
 unsigned char   type;
 unsigned char   channel;
 unsigned char   flags;
 unsigned char   kk;
 unsigned char   vv;
 size_t          datalen;
 unsigned char * dataptr;
 union {
  unsigned char           ldata[MIDI_MES_BUFSIZE];
  struct roar_note_octave note;
 }               d;
};

struct {
 struct roar_buffer * buf;
} g_midi_mess;

#ifndef ROAR_WITHOUT_DCOMP_CB
struct {
 int      console;
 int      stream;
 uint32_t stoptime;
 int      playing;
} g_midi_cb;
#endif

struct {
 int stream;
 uint_least32_t bph; // beats per hour
 uint_least32_t spt; // samples per tick
 uint32_t       nt;  // time of next tick
} g_midi_clock;

struct {
 int stream;
} g_midi_mixer;


// general midi interface:
int midi_init_config(void);

int midi_init (void);
int midi_free (void);

int midi_update(void);
int midi_reinit(void);

// streams:
int midi_check_stream  (int id);
int midi_send_stream   (int id);

int midi_conv_midi2mes (int id);
int midi_conv_mes2midi (int id);

int midi_conv_mes2ssynth(void);

int midi_new_bufmes    (struct roar_buffer ** buf, struct midi_message ** mes);
int midi_add_buf       (int id, struct roar_buffer * buf);

// bridges:
int midi_check_bridge  (int id);

// clock:
int midi_clock_init (void);
int midi_clock_set_bph (uint_least32_t bph);
int midi_clock_tick (void);

// cb = console beep
#ifndef ROAR_WITHOUT_DCOMP_CB
int midi_cb_init(void);
int midi_cb_free(void);
int midi_cb_play(float t, float freq, int override);
int midi_cb_update (void);
int midi_cb_start(float freq);
int midi_cb_stop (void);
int midi_cb_readbuf(void);
#endif

// dummys:
int     midi_vio_set_dummy(int stream);
int     midi_vio_ok(struct roar_vio_calls * vio, ...);

#endif

#endif

//ll
