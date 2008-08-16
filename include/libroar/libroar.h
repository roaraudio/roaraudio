//libroar.h:

#ifndef _LIBROAR_H_
#define _LIBROAR_H_

#define ROAR_DBG_PREFIX  "libroar"

#include <roaraudio.h>

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#ifdef ROAR_HAVE_LIBDNET
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif
#ifdef ROAR_HAVE_IPXSPX
#include <netipx/ipx.h>
#endif

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
#include "midi.h"
#include "acl.h"

#endif

//ll
