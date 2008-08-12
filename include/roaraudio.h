//roaraudio.h:

#ifndef _ROARAUDIO_H_
#define _ROARAUDIO_H_

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>
#include <sys/mman.h>

// TODO: can we move the next block into roard specific includes?
#include <grp.h>
#include <pwd.h>
#include <sys/stat.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/un.h>

#include <netdb.h>

#include <roaraudio/config.h>

// NOTE: we need this macro in some of our header files.
#if INT_MAX >= 32767
#define roar_intm16  int
#define roar_uintm16 unsigned int
#else
#define roar_intm16  int16_t
#define roar_uintm16 uint16_t
#endif

#include <roaraudio/proto.h>
#include <roaraudio/audio.h>
#include <roaraudio/stream.h>
#include <roaraudio/client.h>
#include <roaraudio/sample.h>
#include <roaraudio/meta.h>
#include <roaraudio/acl.h>

#include <libroar/libroar.h>

#define ROAR_DEFAULT_PORT        16002
#define ROAR_DEFAULT_HOST        "localhost"

#define ROAR_DEFAULT_SOCK_GLOBAL "/tmp/roar"
#define ROAR_DEFAULT_SOCK_USER   ".roar"

#define ROAR_DEFAULT_SOCKGRP     "audio"

#define ROAR_LIBS                "-lroar"
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

#ifdef DEBUG
 #define ROAR_DBG(format, args...)  fprintf(ROAR_DEBUG_OUTFH, ROAR_DBG_FULLPREFIX "DEBUG: " format "\n", __LINE__, ## args)
#else
 #define ROAR_DBG(format, args...)
#endif

#define ROAR_ERR(format, args...)  fprintf(ROAR_DEBUG_OUTFH, ROAR_DBG_FULLPREFIX "Error: "   format "\n", __LINE__, ## args)
#define ROAR_WARN(format, args...) fprintf(ROAR_DEBUG_OUTFH, ROAR_DBG_FULLPREFIX "Warning: " format "\n", __LINE__, ## args)


#ifdef ROAR_HAVE_SAFE_OVERFLOW
#define ROAR_MATH_OVERFLOW_ADD(a, b) ((a)+(b))
#else
#define ROAR_MATH_OVERFLOW_ADD(a, b) ((4294967295U - (a)) + 1 + (b))
#endif

#ifdef ROAR_HAVE_MLOCK
#ifdef __linux__
#define ROAR_MLOCK(p,s) mlock((p), (s))
#else
#undef ROAR_HAVE_MLOCK
#define ROAR_MLOCK(p,s)
#warning No working mlock() support for this platform
#endif
#endif


#if BYTE_ORDER == BIG_ENDIAN

#define ROAR_NET2HOST32(x) (x)
#define ROAR_HOST2NET32(x) (x)
#define ROAR_NET2HOST16(x) (x)
#define ROAR_HOST2NET16(x) (x)

//#elif BYTE_ORDER == LITTLE_ENDIAN
#else

#define ROAR_NET2HOST32(x) ntohl((x))
#define ROAR_HOST2NET32(x) htonl((x))
#define ROAR_NET2HOST16(x) ntohs((x))
#define ROAR_HOST2NET16(x) htons((x))

#endif

#endif

//ll
