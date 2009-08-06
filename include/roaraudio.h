//roaraudio.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *  NOTE: Even though this file is LGPLed it (may) include GPLed files
 *  so the license of this file is/may therefore downgraded to GPL.
 *  See HACKING for details.
 */

#ifndef _ROARAUDIO_H_
#define _ROARAUDIO_H_

#include <roaraudio/config.h>
#include <roaraudio/muconthacks.h>
#include <roaraudio/win32hacks.h> // we include this at the beginning of the file
                                  // so we can define well known standard types known to everyone
                                  // and everywhere but win32 here

// consts we need to set depending on the OS and features:
#if defined(ROAR_NEED_GNU_SOURCE) && !defined(_GNU_SOURCE)
#define _GNU_SOURCE
#endif

#if defined(ROAR_NEED_BSD_VISIBLE) && !defined(__BSD_VISIBLE)
#define __BSD_VISIBLE 1
#endif

#include <stdint.h>
#include <stdlib.h>

#ifdef ROAR_HAVE_H_UNISTD
#include <unistd.h>
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef ROAR_HAVE_H_SYS_TYPES
#include <sys/types.h>
#endif

#include <limits.h>
#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
#include <sys/mman.h>
#endif

// TODO: can we move the next block into roard specific includes?
#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>
#endif

#ifdef ROAR_TARGET_WIN32
#include <winsock2.h>
#else /* ROAR_TARGET_WIN32 */
#ifdef ROAR_HAVE_BSDSOCKETS

#if defined(ROAR_HAVE_IPV4) || defined(ROAR_HAVE_IPV6)
#include <arpa/inet.h>
#endif

#ifdef ROAR_HAVE_SELECT
#include <sys/socket.h>
#endif

#ifdef ROAR_HAVE_IPV4
#include <netinet/in.h>
#include <netinet/tcp.h>
#endif
#ifdef ROAR_HAVE_UNIX
#include <sys/un.h>
#endif

#endif /* ROAR_HAVE_BSDSOCKETS */
#endif /* ROAR_TARGET_WIN32 */

#ifdef __NetBSD__
#include <netinet/in_systm.h>
#endif

#if !defined(ROAR_TARGET_WIN32) && !defined(ROAR_TARGET_MICROCONTROLLER)
#include <netdb.h>
#endif

// NOTE: we need this macro in some of our header files.
#if INT_MAX >= 32767
#define roar_intm16  int
#define roar_uintm16 unsigned int
#else
#define roar_intm16  int16_t
#define roar_uintm16 uint16_t
#endif

// this is to avoid warning messages on platforms
// where sizeof(void*) == 8 and szeof(int) == 4
#ifdef __LP64__
#define ROAR_INSTINT long int
#else
#define ROAR_INSTINT int
#endif

#ifndef __BEGIN_DECLS
#ifdef __cplusplus
# define __BEGIN_DECLS extern "C" {
# define __END_DECLS }
#else
# define __BEGIN_DECLS
# define __END_DECLS
#endif
#endif

__BEGIN_DECLS

#include <roaraudio/proto.h>
#include <roaraudio/error.h>
#include <roaraudio/audio.h>
#include <roaraudio/stream.h>
#include <roaraudio/client.h>
#include <roaraudio/sample.h>
#include <roaraudio/meta.h>
#include <roaraudio/acl.h>

#include <libroar/libroar.h>

// IP
#define ROAR_DEFAULT_PORT        16002
#define ROAR_DEFAULT_HOST        "localhost"

// UNIX Domain Sockets
#define ROAR_DEFAULT_SOCK_GLOBAL "/tmp/roar"
#define ROAR_DEFAULT_SOCK_USER   ".roar"

// DECnet
#define ROAR_DEFAULT_OBJECT      "roar"
#define ROAR_DEFAULT_NUM         0
#define ROAR_DEFAULT_LISTEN_OBJECT "::" ROAR_DEFAULT_OBJECT

// now handled by condiguere
//#define ROAR_DEFAULT_SOCKGRP     "audio"

#if defined(ROAR_HAVE_LIBWSOCK32) && defined(ROAR_HAVE_LIBWS2_32)
#define ROAR_LIBS_WIN32          " -lwsock32 -lws2_32"
#else
#define ROAR_LIBS_WIN32          ""
#endif

#ifdef ROAR_HAVE_LIBSOCKET
#define ROAR_LIBS_LIBSOCKET      " -lsocket"
#else
#define ROAR_LIBS_LIBSOCKET      ""
#endif

#ifdef ROAR_HAVE_LIBSENDFILE
#define ROAR_LIBS_LIBSENDFILE    " -lsendfile"
#else
#define ROAR_LIBS_LIBSENDFILE    ""
#endif

#define ROAR_LIBS_NET_LIBS       ROAR_LIBS_LIBSOCKET ROAR_LIBS_WIN32

#define ROAR_LIBS                "-lroar"       ROAR_LIBS_LIBSENDFILE ROAR_LIBS_NET_LIBS
#define ROAR_LIBS_DSP            "-lroardsp "   ROAR_LIBS
#define ROAR_LIBS_MIDI           "-lroarmidi "  ROAR_LIBS_DSP
#define ROAR_LIBS_LIGHT          "-lroarlight " ROAR_LIBS
#define ROAR_LIBS_EIO            "-lroareio "   ROAR_LIBS
#define ROAR_CFLAGS              ""

//some basic macros:
#define ROAR_STDIN  0
#define ROAR_STDOUT 1
#define ROAR_STDERR 2

#define ROAR_DEBUG_OUTFH stderr

#ifdef ROAR_DBG_PREFIX
#undef ROAR_DBG_PREFIX
#endif
#define ROAR_DBG_PREFIX "roaraudio"

#define ROAR_DBG_FULLPREFIX "(" ROAR_DBG_PREFIX ": " __FILE__ ":%i): "

#if __GNUC__ < 3
 #define ROAR_DBG(format, args...)
 #define ROAR_ERR(format, args...)
 #define ROAR_WARN(format, args...)
#else

#ifdef DEBUG
 #define ROAR_DBG(format, args...)  fprintf(ROAR_DEBUG_OUTFH, ROAR_DBG_FULLPREFIX "DEBUG: " format "\n", __LINE__, ## args)
#else
 #define ROAR_DBG(format, args...)
#endif

#define ROAR_ERR(format, args...)  fprintf(ROAR_DEBUG_OUTFH, ROAR_DBG_FULLPREFIX "Error: "   format "\n", __LINE__, ## args)
#define ROAR_WARN(format, args...) fprintf(ROAR_DEBUG_OUTFH, ROAR_DBG_FULLPREFIX "Warning: " format "\n", __LINE__, ## args)

#endif

#ifdef ROAR_HAVE_SAFE_OVERFLOW
#define ROAR_MATH_OVERFLOW_ADD(a, b) ((a)+(b))
#else
#define ROAR_MATH_OVERFLOW_ADD(a, b) ((4294967295U - (a)) + 1 + (b))
#endif

#ifdef ROAR_HAVE_MLOCK
#ifdef __linux__
#define ROAR_MLOCK(p,s) mlock((p), (s))
#else
int _ROAR_MLOCK(const void *addr, size_t len);
#define ROAR_MLOCK _ROAR_MLOCK
#endif
#endif

#if BYTE_ORDER == BIG_ENDIAN && !defined(ROAR_TARGET_WIN32)

#ifdef ROAR_TARGET_WIN32
#error This is nonsens. No win32 runs on a BE machine
#endif

#define ROAR_NET2HOST64(x) (x)
#define ROAR_HOST2NET64(x) (x)
#define ROAR_NET2HOST32(x) (x)
#define ROAR_HOST2NET32(x) (x)
#define ROAR_NET2HOST16(x) (x)
#define ROAR_HOST2NET16(x) (x)

#ifdef ROAR_HAVE_LIBDNET
#define ROAR_dn_ntohs(x) ((((x)&0x0ff)<<8) | (((x)&0xff00)>>8))
#define ROAR_dn_ntohl(x) ( ((dn_ntohs((x)&0xffff))<<16) |\
                           ((dn_ntohs(((x)>>16)))) )
#define ROAR_dn_htonl(x) ROAR_dn_ntohl(x)
#define ROAR_dn_htons(x) ROAR_dn_ntohs(x)
#endif

//#elif BYTE_ORDER == LITTLE_ENDIAN
#else

#ifdef BYTE_ORDER == LITTLE_ENDIAN
#define _ROAR_MOVE_BYTE(x,p) (((x) & (0xFFUL << (8*(p)))) >> (8*(p)) << (64-8*((p)+1)))
#define ROAR_NET2HOST64(x) ROAR_HOST2NET64(x)
#define ROAR_HOST2NET64(x) (_ROAR_MOVE_BYTE((x), 0) | _ROAR_MOVE_BYTE((x), 1) | \
                            _ROAR_MOVE_BYTE((x), 2) | _ROAR_MOVE_BYTE((x), 3) | \
                            _ROAR_MOVE_BYTE((x), 4) | _ROAR_MOVE_BYTE((x), 5) | \
                            _ROAR_MOVE_BYTE((x), 6) | _ROAR_MOVE_BYTE((x), 7) | )
#else /* PDP byte order */
#endif

#define ROAR_NET2HOST32(x) ntohl((x))
#define ROAR_HOST2NET32(x) htonl((x))
#define ROAR_NET2HOST16(x) ntohs((x))
#define ROAR_HOST2NET16(x) htons((x))

#ifdef ROAR_HAVE_LIBDNET
#if BYTE_ORDER == LITTLE_ENDIAN
#define ROAR_dn_ntohs(x) (x)
#define ROAR_dn_htons(x) (x)

#define ROAR_dn_ntohl(x) (x)
#define ROAR_dn_htonl(x) (x)
#else
#error can not build on this architecture with DECnet support enabled
#endif
#endif

#endif

__END_DECLS

#endif

//ll
