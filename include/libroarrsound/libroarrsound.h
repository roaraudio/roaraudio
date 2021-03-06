//libroarrsound.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from RSound.
 *  They are copyrighted by Hans-Kristian 'maister' Arntzen.
 *
 *  This file is part of libroarrsound a part of RoarAudio,
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
 */

#ifndef _LIBROARRSOUND_H_
#define _LIBROARRSOUND_H_

#include <roaraudio.h>
#include <rsound.h>

#define LIBROARRSOUND_FLAGS_NONE        0x00
#define LIBROARRSOUND_FLAGS_CONNECTED   0x01
#define LIBROARRSOUND_FLAGS_STREAMING   0x02

struct libroarrsound {
 rsound_t rsound;
 int    flags;
 struct roar_connection con;
 struct roar_stream     stream;
 struct roar_vio_calls  vio;
};

#endif

//ll
