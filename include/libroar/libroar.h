//libroar.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

#ifndef _LIBROAR_H_
#define _LIBROAR_H_

#define ROAR_DBG_PREFIX  "libroar"

#include <roaraudio.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#ifdef ROAR_HAVE_LIBDNET
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif
#ifdef ROAR_HAVE_IPX
#include <netipx/ipx.h>
#endif

#include "vio.h"
#include "basic.h"
#include "stream.h"
#include "simple.h"
#include "auth.h"
#include "socket.h"
#include "ctl.h"
#include "buffer.h"
#include "convert.h"
#include "poly.h"
#include "meta.h"
#include "file.h"
#include "acl.h"

#endif

//ll
