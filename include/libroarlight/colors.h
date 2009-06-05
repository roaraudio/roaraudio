//colors.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
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

#ifndef _LIBROARLIGHT_COLORS_H_
#define _LIBROARLIGHT_COLORS_H_

#include "libroarlight.h"

#define ROAR_COLOR_TYPE_NONE           0x00000000
#define ROAR_COLOR_TYPE_GRAY           0x00000001 /* ???         */
#define ROAR_COLOR_TYPE_ALPHA          0x00000002 /* Alpha       */
#define ROAR_COLOR_TYPE_DOUBLE         0x00000004 /* use 16 bit per channel */
#define ROAR_COLOR_TYPE_A              ROAR_COLOR_TYPE_ALPHA
#define ROAR_COLOR_TYPE_K              0x00000010 /* black (Key) */
#define ROAR_COLOR_TYPE_R              0x00000020 /* red         */
#define ROAR_COLOR_TYPE_G              0x00000040 /* green       */
#define ROAR_COLOR_TYPE_B              0x00000080 /* blue        */
#define ROAR_COLOR_TYPE_H              0x00000100
#define ROAR_COLOR_TYPE_S              0x00000200
#define ROAR_COLOR_TYPE_V              0x00000400
#define ROAR_COLOR_TYPE_B              0x00000800
#define ROAR_COLOR_TYPE_L              0x00001000
#define ROAR_COLOR_TYPE_I              0x00002000
#define ROAR_COLOR_TYPE_C              0x00004000 /* Cyan          */
#define ROAR_COLOR_TYPE_M              0x00008000 /* Magenta       */
#define ROAR_COLOR_TYPE_Y              0x00010000 /* Yellow or ??? */
#define ROAR_COLOR_TYPE_U              0x00020000
//#define ROAR_COLOR_TYPE_              0x0100

#define ROAR_COLORSYSTEM_GRAY          (ROAR_COLOR_TYPE_K)
#define ROAR_COLORSYSTEM_RGB           (ROAR_COLOR_TYPE_R|ROAR_COLOR_TYPE_G|ROAR_COLOR_TYPE_B)
#define ROAR_COLORSYSTEM_RGBA          (ROAR_COLOR_TYPE_R|ROAR_COLOR_TYPE_G|ROAR_COLOR_TYPE_B|ROAR_COLOR_TYPE_ALPHA)
#define ROAR_COLORSYSTEM_YUV           (ROAR_COLOR_TYPE_Y|ROAR_COLOR_TYPE_U|ROAR_COLOR_TYPE_V)
#define ROAR_COLORSYSTEM_HSV           (ROAR_COLOR_TYPE_H|ROAR_COLOR_TYPE_S|ROAR_COLOR_TYPE_V)
#define ROAR_COLORSYSTEM_HSL           (ROAR_COLOR_TYPE_H|ROAR_COLOR_TYPE_S|ROAR_COLOR_TYPE_L)
#define ROAR_COLORSYSTEM_HSB           (ROAR_COLOR_TYPE_H|ROAR_COLOR_TYPE_S|ROAR_COLOR_TYPE_B)
#define ROAR_COLORSYSTEM_HSI           (ROAR_COLOR_TYPE_H|ROAR_COLOR_TYPE_S|ROAR_COLOR_TYPE_I)
#define ROAR_COLORSYSTEM_CMYK          (ROAR_COLOR_TYPE_C|ROAR_COLOR_TYPE_M|ROAR_COLOR_TYPE_Y|ROAR_COLOR_TYPE_K)

#endif

//ll
