//hash.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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

#ifndef _LIBROARHASH_H_
#define _LIBROARHASH_H_

#include "libroar.h"

// the hashtypes:
#define ROAR_HT_NONE        0
#define ROAR_HT_MD5         1
#define ROAR_HT_SHA1        2
#define ROAR_HT_RIPEMD160   3
#define ROAR_HT_MD2         5
#define ROAR_HT_TIGER       6
#define ROAR_HT_HAVAL       7
#define ROAR_HT_SHA256      8
#define ROAR_HT_SHA384      9
#define ROAR_HT_SHA512      10
#define ROAR_HT_SHA224      11
#define ROAR_HT_MD4         301
#define ROAR_HT_CRC32       302
#define ROAR_HT_RFC1510     303
#define ROAR_HT_RFC2440     304
#define ROAR_HT_WHIRLPOOL   305
#define ROAR_HT_UUID        70000
#define ROAR_HT_GTN8        70001
#define ROAR_HT_GTN16       70002
#define ROAR_HT_GTN32       70004
#define ROAR_HT_GTN64       70008
#define ROAR_HT_CLIENTID    71001
#define ROAR_HT_STREAMID    71002
#define ROAR_HT_SOURCEID    71003
#define ROAR_HT_SAMPLEID    71004
#define ROAR_HT_MIXERID     71005
#define ROAR_HT_BRIDGEID    71006
#define ROAR_HT_LISTENID    71007
#define ROAR_HT_ACTIONID    71008
#define ROAR_HT_MSGQUEUEID  71009
#define ROAR_HT_MSGBUSID    71010
#define ROAR_HT_GTIN8       72001
#define ROAR_HT_GTIN13      72002
#define ROAR_HT_ISBN10      72003
#define ROAR_HT_ISBN13      ROAR_HT_GTIN13

const char * roar_ht2str (const int    ht);
int          roar_str2ht (const char * ht);

ssize_t      roar_ht_digestlen (const int    ht);

int          roar_ht_is_supported(const int    ht);

struct roar_hash_state;

int roar_hash_buffer(void * digest, const void * data, size_t datalen, int algo);
int roar_hash_salted_buffer(void * digest, const void * data, size_t datalen, int algo, const void * salt, size_t saltlen);

#endif

//ll
