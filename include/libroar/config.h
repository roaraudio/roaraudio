//config.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008, 2009
 *
 *  This file is part of libroar a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  libroar is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARCONFIG_H_
#define _LIBROARCONFIG_H_

#include "libroar.h"

// WorkAroundS:
#define ROAR_LIBROAR_CONFIG_WAS_NONE        0x00
#define ROAR_LIBROAR_CONFIG_WAS_USE_EXECED  0x01

#define ROAR_LIBROAR_CONFIG_PSET_Q          0x0001
#define ROAR_LIBROAR_CONFIG_PSET_COMPLEXITY 0x0002
#define ROAR_LIBROAR_CONFIG_PSET_DTX        0x0004
#define ROAR_LIBROAR_CONFIG_PSET_MAX_CC     0x0008
#define ROAR_LIBROAR_CONFIG_PSET_ABR        0x0010 /* need to implement */
#define ROAR_LIBROAR_CONFIG_PSET_VAD        0x0020 /* need to implement */
#define ROAR_LIBROAR_CONFIG_PSET_AGC        0x0040 /* need to implement */
#define ROAR_LIBROAR_CONFIG_PSET_DENOISE    0x0080 /* need to implement */
#define ROAR_LIBROAR_CONFIG_PSET_VBR        0x0100
#define ROAR_LIBROAR_CONFIG_PSET_MODE       0x0200

#define ROAR_LIBROAR_CONFIG_MODE_NB         ROAR_SPEEX_MODE_NB
#define ROAR_LIBROAR_CONFIG_MODE_WB         ROAR_SPEEX_MODE_WB
#define ROAR_LIBROAR_CONFIG_MODE_UWB        ROAR_SPEEX_MODE_UWB

struct roar_libroar_config_codec {
 unsigned int codec; // Codec ID

 // parameters which are set:
 unsigned int para_set;

 // the folloing ints are 256 times there correct value
 // to emulate a .8 bit fixed point float.
 int q;
 int complexity;

 // currectly bools:
 int dtx;
 int vbr;

 // sizes:
 size_t max_cc;

 // enums:
 int mode;
};

struct roar_libroar_config {
 struct {
  int workarounds;
 } workaround;
 char * server;
 struct {
  int sysio;
 } warnings;
 struct {
  size_t num;
  struct roar_libroar_config_codec * codec;
 } codecs;
 struct roar_audio_info info;
 char * authfile;
 struct {
  char * display;
 } x11;
};

struct roar_libroar_config * roar_libroar_get_config_ptr(void);
struct roar_libroar_config * roar_libroar_get_config(void);

int    roar_libroar_config_parse(char * txt, char * delm);

struct roar_libroar_config_codec * roar_libroar_config_codec_get(int codec, int create);

int    roar_libroar_set_server(char * server);
char * roar_libroar_get_server(void);

#endif

//ll
