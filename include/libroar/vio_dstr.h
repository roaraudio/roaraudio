//vio_dstr.h:

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

#ifndef _LIBROARVIO_DSTR_H_
#define _LIBROARVIO_DSTR_H_

#include "libroar.h"

#define ROAR_VIO_DEF_TYPE_EOL            -1 
#define ROAR_VIO_DEF_TYPE_NONE            0
#define ROAR_VIO_DEF_TYPE_FILE            1
#define ROAR_VIO_DEF_TYPE_SOCKET          2
#define ROAR_VIO_DEF_TYPE_FH              3
#define ROAR_VIO_DEF_TYPE_SOCKETFH        4

#define ROAR_VIO_DSTR_OBJGT_INTERNAL      0x0000
#define ROAR_VIO_DSTR_OBJGT_GEN           0x0100
#define ROAR_VIO_DSTR_OBJGT_SOCKET        0x0200
#define ROAR_VIO_DSTR_OBJGT_PROXY         0x0300
#define ROAR_VIO_DSTR_OBJGT_COMP          0x0400 /* compression */
#define ROAR_VIO_DSTR_OBJGT_CRYPT         0x0500
#define ROAR_VIO_DSTR_OBJGT_PROTO         0x0600 /* protocolls like HTTP and Gopher */
#define ROAR_VIO_DSTR_OBJGT_SPECAL        0xff00

#define ROAR_VIO_DSTR_OBJT_EOL           -1
#define ROAR_VIO_DSTR_OBJT_NONE           0
#define ROAR_VIO_DSTR_OBJT_INTERNAL       1

#define ROAR_VIO_DSTR_OBJT_FILE           (0x01|ROAR_VIO_DSTR_OBJGT_GEN)
#define ROAR_VIO_DSTR_OBJT_FH             (0x02|ROAR_VIO_DSTR_OBJGT_GEN)
#define ROAR_VIO_DSTR_OBJT_FD             ROAR_VIO_DSTR_OBJT_FH
#define ROAR_VIO_DSTR_OBJT_SOCKETFH       (0x03|ROAR_VIO_DSTR_OBJGT_GEN)
/* some space to add memory FHs and the like */
#define ROAR_VIO_DSTR_OBJT_PASS           (0x10|ROAR_VIO_DSTR_OBJGT_GEN)
#define ROAR_VIO_DSTR_OBJT_RE             (0x11|ROAR_VIO_DSTR_OBJGT_GEN)
#define ROAR_VIO_DSTR_OBJT_EXEC           (0x20|ROAR_VIO_DSTR_OBJGT_GEN)

#define ROAR_VIO_DSTR_OBJT_SOCKET         (0x01|ROAR_VIO_DSTR_OBJGT_SOCKET)
#define ROAR_VIO_DSTR_OBJT_UNIX           (0x02|ROAR_VIO_DSTR_OBJGT_SOCKET)
#define ROAR_VIO_DSTR_OBJT_DECNET         (0x10|ROAR_VIO_DSTR_OBJGT_SOCKET)
#define ROAR_VIO_DSTR_OBJT_TCP            (0x21|ROAR_VIO_DSTR_OBJGT_SOCKET)
#define ROAR_VIO_DSTR_OBJT_UDP            (0x22|ROAR_VIO_DSTR_OBJGT_SOCKET)
#define ROAR_VIO_DSTR_OBJT_TCP6           (0x31|ROAR_VIO_DSTR_OBJGT_SOCKET)
#define ROAR_VIO_DSTR_OBJT_UDP6           (0x32|ROAR_VIO_DSTR_OBJGT_SOCKET)

#define ROAR_VIO_DSTR_OBJT_SOCKS          (0x10|ROAR_VIO_DSTR_OBJGT_PROXY)
#define ROAR_VIO_DSTR_OBJT_SOCKS4         (0x14|ROAR_VIO_DSTR_OBJGT_PROXY)
#define ROAR_VIO_DSTR_OBJT_SOCKS4A        (0x1a|ROAR_VIO_DSTR_OBJGT_PROXY)
#define ROAR_VIO_DSTR_OBJT_SOCKS4D        (0x1d|ROAR_VIO_DSTR_OBJGT_PROXY)
#define ROAR_VIO_DSTR_OBJT_SOCKS5         (0x15|ROAR_VIO_DSTR_OBJGT_PROXY)
#define ROAR_VIO_DSTR_OBJT_SSH            (0x21|ROAR_VIO_DSTR_OBJGT_PROXY)
//#define ROAR_VIO_DSTR_OBJT_HTTP           (0x31|ROAR_VIO_DSTR_OBJGT_PROXY)

//#define ROAR_VIO_DSTR_OBJT_HTTP           (0x10|ROAR_VIO_DSTR_OBJGT_PROTO)
#define ROAR_VIO_DSTR_OBJT_HTTP09         (0x11|ROAR_VIO_DSTR_OBJGT_PROTO)
#define ROAR_VIO_DSTR_OBJT_HTTP10         (0x12|ROAR_VIO_DSTR_OBJGT_PROTO)
#define ROAR_VIO_DSTR_OBJT_HTTP11         (0x13|ROAR_VIO_DSTR_OBJGT_PROTO)
#define ROAR_VIO_DSTR_OBJT_HTTP           ROAR_VIO_DSTR_OBJT_HTTP11
#define ROAR_VIO_DSTR_OBJT_GOPHER         (0x21|ROAR_VIO_DSTR_OBJGT_PROTO)
#define ROAR_VIO_DSTR_OBJT_GOPHER_PLUS    (0x22|ROAR_VIO_DSTR_OBJGT_PROTO)
#define ROAR_VIO_DSTR_OBJT_ICY            (0x31|ROAR_VIO_DSTR_OBJGT_PROTO)

/*
#define ROAR_VIO_DSTR_OBJGT_CRYPT         0x0500
*/

#define ROAR_VIO_DSTR_OBJT_GZIP           (0x10|ROAR_VIO_DSTR_OBJGT_COMP)
#define ROAR_VIO_DSTR_OBJT_BZIP2          (0x22|ROAR_VIO_DSTR_OBJGT_COMP)

#define ROAR_VIO_DSTR_OBJT_PGP            (0x10|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_PGP_ENC        (0x11|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_PGP_STORE      (0x12|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_SSL1           (0x21|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_SSL2           (0x22|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_SSL3           (0x23|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_TLS            (0x2a|ROAR_VIO_DSTR_OBJGT_CRYPT)
#define ROAR_VIO_DSTR_OBJT_SSLTLS         ROAR_VIO_DSTR_OBJT_TLS

#define ROAR_VIO_DSTR_OBJT_MAGIC          (0x01|ROAR_VIO_DSTR_OBJGT_SPECAL)


struct roar_vio_defaults {
 int type;

 mode_t o_mode;
 int    o_flags;

 union {
  char *   file;
  int      fh;
  struct {
          int               domain;
          int               type;
          char            * host;
          union {
                 struct sockaddr     sa;
#ifdef ROAR_HAVE_IPV4
                 struct sockaddr_in  in;
#endif
#ifdef ROAR_HAVE_UNIX
                 struct sockaddr_un  un;
#endif
#ifdef ROAR_HAVE_LIBDNET
                 struct sockaddr_dn  dn;
#endif
#ifdef ROAR_HAVE_IPV6
                 struct sockaddr_in6 in6;
#endif
#ifdef ROAR_HAVE_IPX
                 struct sockaddr_ipx ipx;
#endif
                } sa;
          socklen_t         len;
         } socket;
 } d;
};

#define ROAR_VIO_DSTR_MAX_OBJ_PER_CHAIN 16

struct roar_vio_dstr_chain {
 int    type;
 char * opts;
 char * dst;
 int    need_vio;
 struct roar_vio_defaults * def;
 struct roar_vio_calls    * vio;
 struct roar_vio_defaults   store_def;
};

int     roar_vio_dstr_get_type(char * str);
char *  roar_vio_dstr_get_name(int type);

int     roar_vio_dstr_init_defaults (struct roar_vio_defaults * def, int type, int o_flags, mode_t o_mode);
int     roar_vio_dstr_init_defaults_c (struct roar_vio_defaults * def, int type, struct roar_vio_defaults * odef, int o_flags);

int     roar_vio_open_default (struct roar_vio_calls * calls, struct roar_vio_defaults * def);

int     roar_vio_open_dstr    (struct roar_vio_calls * calls, char * dstr, struct roar_vio_defaults * def, int dnum);
int     roar_vio_open_dstr_vio(struct roar_vio_calls * calls, char * dstr, struct roar_vio_defaults * def, int dnum, struct roar_vio_calls * vio);

int     roar_vio_dstr_parse_opts(struct roar_vio_dstr_chain * chain);
int     roar_vio_dstr_set_defaults(struct roar_vio_dstr_chain * chain, int len, struct roar_vio_defaults * def, int dnum);
int     roar_vio_dstr_build_chain(struct roar_vio_dstr_chain * chain, struct roar_vio_calls * calls, struct roar_vio_calls * vio);

#endif

//ll
