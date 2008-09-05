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

#ifndef _LIBROARSOCKET_H_
#define _LIBROARSOCKET_H_

#include "libroar.h"

#define ROAR_SOCKET_MODE_LISTEN  1
#define ROAR_SOCKET_MODE_CONNECT 2

#define ROAR_SOCKET_TYPE_NONE    0
#define ROAR_SOCKET_TYPE_UNKNOWN ROAR_SOCKET_TYPE_NONE
#define ROAR_SOCKET_TYPE_INET    1
#define ROAR_SOCKET_TYPE_TCP     ROAR_SOCKET_TYPE_INET
#define ROAR_SOCKET_TYPE_UNIX    2
#define ROAR_SOCKET_TYPE_FORK    3
#define ROAR_SOCKET_TYPE_PIPE    ROAR_SOCKET_TYPE_FORK
#define ROAR_SOCKET_TYPE_FILE    4
#define ROAR_SOCKET_TYPE_UDP     5
#define ROAR_SOCKET_TYPE_GENSTR  6 /* generic stream: TCP or UNIX */
#define ROAR_SOCKET_TYPE_DECNET  7 /* DECnet */
#define ROAR_SOCKET_TYPE_TCP6    8
#define ROAR_SOCKET_TYPE_UDP6    9
#define ROAR_SOCKET_TYPE_INET6   ROAR_SOCKET_TYPE_TCP6
#define ROAR_SOCKET_TYPE_IPXSPX  10
#define ROAR_SOCKET_TYPE_IPX     11

#define ROAR_SOCKET_TYPE_MAX  11

#define ROAR_SOCKET_QUEUE_LEN 8

#define ROAR_SOCKET_BLOCK     1
#define ROAR_SOCKET_NONBLOCK  2

#define ROAR_SOCKET_MAX_HOSTNAMELEN 64

int roar_socket_listen  (int type, char * host, int port);
int roar_socket_connect (char * host, int port);

int roar_socket_new_tcp    (void);
int roar_socket_new_udp    (void);
int roar_socket_new_tcp6   (void);
int roar_socket_new_udp6   (void);
int roar_socket_new_unix   (void);
int roar_socket_new_decnet_seqpacket (void);
int roar_socket_new_decnet_stream (void);
int roar_socket_new_ipxspx (void);
int roar_socket_new_ipx    (void);

int roar_socket_open       (int mode, int type, char * host, int port);
int roar_socket_open_fork  (int mode, char * host, int port);
int roar_socket_open_file  (int mode, char * host, int port);
int roar_socket_open_proxy (int mode, int type, char * host, int port, char * proxy_type);

int roar_socket_listen_decnet (char * object, int num);

char * roar_socket_get_local_nodename(void);

int roar_socket_nonblock(int fh, int state);

int roar_socket_dup_udp_local_end (int fh);

int roar_socket_send_fh (int sock, int fh, char * mes, size_t   len);
int roar_socket_recv_fh (int sock,         char * mes, size_t * len);

int roar_socket_open_socks4a(int mode, int fh, char * host, int port);

#endif

//ll
