//libroaresd.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from EsounD.
 *  They are mostly copyrighted by Eric B. Mitchell (aka 'Ricdude)
 *  <ericmit@ix.netcom.com>. For more information see AUTHORS.esd.
 *
 *  This file is part of libroaresd a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARESD_H_
#define _LIBROARESD_H_

#include <roaraudio.h>
#include <esd.h>
#include <time.h>
#include <sys/time.h>

#define ROAR_BC2ESD(b,c) (((b) == 8 ? ESD_BITS8 : ESD_BITS16) | ((c) == 1 ? ESD_MONO : ESD_STEREO))
#define ROAR_DIR2ESD(d)  ((d)  == ROAR_DIR_PLAY ? ESD_PLAY : (d) == ROAR_DIR_MONITOR ? ESD_MONITOR : \
                          (d)  == ROAR_DIR_RECORD ? ESD_RECORD : \
                          ESD_MONITOR /* ESD implements FILTER a diffrent way, think MONITOR ist most simular */ )

#define ROAR_S2ESD(s)    (ROAR_BC2ESD((s)->info.bits, (s)->info.channels) | ROAR_DIR2ESD((s)->dir))

#endif

//ll
