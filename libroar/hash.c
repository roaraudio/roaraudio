//hash.c:

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

#include "libroar.h"

static const struct hashes {
 const int    id;
 const char * name;
} _libroar_hashes[] = {
/*
grep '^  +HT_' doc/new-cmds | sed 's/ *#(.*)$//; s/^  +HT_//; s/ *=.*$//' | while read n; do printf " {ROAR_HT_%-12s \"%-12s},\n" $n, $n\"; done
*/
 {ROAR_HT_NONE,        "NONE"       },
 {ROAR_HT_MD5,         "MD5"        },
 {ROAR_HT_SHA1,        "SHA1"       },
 {ROAR_HT_RIPEMD160,   "RIPEMD160"  },
 {ROAR_HT_MD2,         "MD2"        },
 {ROAR_HT_TIGER,       "TIGER"      },
 {ROAR_HT_HAVAL,       "HAVAL"      },
 {ROAR_HT_SHA256,      "SHA256"     },
 {ROAR_HT_SHA384,      "SHA384"     },
 {ROAR_HT_SHA512,      "SHA512"     },
 {ROAR_HT_SHA224,      "SHA224"     },
 {ROAR_HT_MD4,         "MD4"        },
 {ROAR_HT_CRC32,       "CRC32"      },
 {ROAR_HT_RFC1510,     "RFC1510"    },
 {ROAR_HT_RFC2440,     "RFC2440"    },
 {ROAR_HT_WHIRLPOOL,   "WHIRLPOOL"  },
 {ROAR_HT_UUID,        "UUID"       },
 {ROAR_HT_GTN8,        "GTN8"       },
 {ROAR_HT_GTN16,       "GTN16"      },
 {ROAR_HT_GTN32,       "GTN32"      },
 {ROAR_HT_GTN64,       "GTN64"      },
 {ROAR_HT_CLIENTID,    "CLIENTID"   },
 {ROAR_HT_STREAMID,    "STREAMID"   },
 {ROAR_HT_SOURCEID,    "SOURCEID"   },
 {ROAR_HT_SAMPLEID,    "SAMPLEID"   },
 {ROAR_HT_MIXERID,     "MIXERID"    },
 {ROAR_HT_BRIDGEID,    "BRIDGEID"   },
 {ROAR_HT_LISTENID,    "LISTENID"   },
 {ROAR_HT_ACTIONID,    "ACTIONID"   },
 {ROAR_HT_MSGQUEUEID,  "MSGQUEUEID" },
 {ROAR_HT_MSGBUSID,    "MSGBUSID"   },
 {ROAR_HT_GTIN8,       "GTIN8"      },
 {ROAR_HT_GTIN13,      "GTIN13"     },
 {ROAR_HT_ISBN10,      "ISBN10"     },
 {ROAR_HT_ISBN13,      "ISBN13"     },
 {-1, NULL}
};

const char * roar_ht2str (const int    ht) {
 int i;

 for (i = 0; _libroar_hashes[i].id != -1; i++)
  if ( _libroar_hashes[i].id == ht )
   return _libroar_hashes[i].name;

 return NULL;
}

int          roar_str2ht (const char * ht) {
 int i;

 for (i = 0; _libroar_hashes[i].id != -1; i++)
  if ( !strcasecmp(_libroar_hashes[i].name, ht) )
   return _libroar_hashes[i].id;

 return -1;
}

int roar_hash_buffer(void * digest, const void * data, size_t datalen, int algo) {
 return roar_hash_salted_buffer(digest, data, datalen, algo, NULL, 0);
}

int roar_hash_salted_buffer(void * digest, const void * data, size_t datalen, int algo, const void * salt, size_t saltlen) {
 return -1;
}

//ll
