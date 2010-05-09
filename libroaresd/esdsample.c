//esdsample.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroaresd.h"

// TODO: add support for all of thiese funcs

/* cache a sample in the server returns sample id, < 0 = error */
int esd_sample_cache( int esd, esd_format_t format, const int rate,
                      const int length, const char *name ) {
 return -1;
}
int esd_confirm_sample_cache( int esd ) {
 return -1;
}

/* get the sample id for an already-cached sample */
int esd_sample_getid( int esd, const char *name) {
 return -1;
}

/* uncache a sample in the server */
int esd_sample_free( int esd, int sample ) {
 return -1;
}

/* play a cached sample once */
int esd_sample_play( int esd, int sample ) {
 return -1;
}
/* make a cached sample loop */
int esd_sample_loop( int esd, int sample ) {
 return -1;
}

/* stop the looping sample at end */
int esd_sample_stop( int esd, int sample ) {
 return -1;
}
/* stop a playing sample immed. */
int esd_sample_kill( int esd, int sample ) {
 return -1;
}


//ll
