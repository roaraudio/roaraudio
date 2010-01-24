//ff_ssdp.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
 *
 *  This file is part of libroareio a part of RoarAudio,
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

#ifndef _LIBROAREIO_FF_SSDP_H_
#define _LIBROAREIO_FF_SSDP_H_

#include "libroareio.h"

/*
> NOTIFY * HTTP/1.1
> SERVER: Linux/2.6.15.2 UPnP/1.0 Mediaserver/1.0
> CACHE-CONTROL: max-age=1800
> LOCATION: http://192.168.0.10:8080/description.xml
> NTS: ssdp:alive
> NT: urn:schemas-upnp-org:service:ConnectionManager:1
> USN: uuid:550e8400-e29b-11d4-a716-446655440000::urn:schemas-upnp-org:service:ConnectionManager:1
> HOST: 239.255.255.250:1900
*/

#define ROAR_FF_SSDP_M_NOTIFY    1
#define ROAR_FF_SSDP_M_M_SEARCH  2

#define ROAR_FF_SSDP_MS_NOTIFY   "NOTIFY"
#define ROAR_FF_SSDP_MS_M_SEARCH "M-SEARCH"

#define ROAR_FF_SSDP_A_ALIVE     1
#define ROAR_FF_SSDP_A_BYEBYE    2
#define ROAR_FF_SSDP_A_REG       ROAR_FF_SSDP_A_ALIVE
#define ROAR_FF_SSDP_A_UNREG     ROAR_FF_SSDP_A_BYEBYE

#define ROAR_FF_SSDP_AS_ALIVE    "ssdp:alive"
#define ROAR_FF_SSDP_AS_BYEBYE   "ssdp:byebye"

#define ROAR_FF_SSDP_HOST_UPNP   "239.255.255.250:1900"

struct roar_ff_ssdp {
 int    method;
 char * server;
 int    max_age;
 char * location;
 int    nst;
 char * nt;
 char * usn;
 int    usn_nt_suffix;
 char * host;
};

void roar_ff_ssdp_init (struct roar_ff_ssdp * c);
void roar_ff_ssdp_free (struct roar_ff_ssdp * c);

int  roar_ff_ssdp_write(struct roar_vio_calls * vio, struct roar_ff_ssdp * c);
int  roar_ff_ssdp_read (struct roar_vio_calls * vio, struct roar_ff_ssdp * c);

#endif

//ll
