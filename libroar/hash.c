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

#ifdef ROAR_HAVE_LIBGCRYPT
#include <gcrypt.h>
#endif

static const struct hashes {
 const int     id;
 const char *  name;
 const ssize_t dlen;
} _libroar_hashes[] = {
/*
grep '^  +HT_' doc/new-cmds | sed 's/ *#(.*)$//; s/^  +HT_//; s/ *=.*$//' | while read n; do printf " {ROAR_HT_%-12s \"%-12s -1   },\n" $n, $n\",; done
*/
 {ROAR_HT_NONE,        "NONE",       -1   },
 {ROAR_HT_MD5,         "MD5",         16  },
 {ROAR_HT_SHA1,        "SHA1",        20  },
 {ROAR_HT_RIPEMD160,   "RIPEMD160",   20  },
 {ROAR_HT_MD2,         "MD2",        -1   },
 {ROAR_HT_TIGER,       "TIGER",       24  },
 {ROAR_HT_HAVAL,       "HAVAL",      -1   },
 {ROAR_HT_SHA256,      "SHA256",      32  },
 {ROAR_HT_SHA384,      "SHA384",      48  },
 {ROAR_HT_SHA512,      "SHA512",      64  },
 {ROAR_HT_SHA224,      "SHA224",      28  },
 {ROAR_HT_MD4,         "MD4",         16  },
 {ROAR_HT_CRC32,       "CRC32",       4   },
 {ROAR_HT_RFC1510,     "RFC1510",     4   },
 {ROAR_HT_RFC2440,     "RFC2440",     3   },
 {ROAR_HT_WHIRLPOOL,   "WHIRLPOOL",   64  },
 {ROAR_HT_UUID,        "UUID",        16  },
 {ROAR_HT_GTN8,        "GTN8",        1   },
 {ROAR_HT_GTN16,       "GTN16",       2   },
 {ROAR_HT_GTN32,       "GTN32",       4   },
 {ROAR_HT_GTN64,       "GTN64",       8   },
 {ROAR_HT_CLIENTID,    "CLIENTID",   -1   },
 {ROAR_HT_STREAMID,    "STREAMID",   -1   },
 {ROAR_HT_SOURCEID,    "SOURCEID",   -1   },
 {ROAR_HT_SAMPLEID,    "SAMPLEID",   -1   },
 {ROAR_HT_MIXERID,     "MIXERID",    -1   },
 {ROAR_HT_BRIDGEID,    "BRIDGEID",   -1   },
 {ROAR_HT_LISTENID,    "LISTENID",   -1   },
 {ROAR_HT_ACTIONID,    "ACTIONID",   -1   },
 {ROAR_HT_MSGQUEUEID,  "MSGQUEUEID", -1   },
 {ROAR_HT_MSGBUSID,    "MSGBUSID",   -1   },
 {ROAR_HT_GTIN8,       "GTIN8",       4   },
 {ROAR_HT_GTIN13,      "GTIN13",      8   },
 {ROAR_HT_ISBN10,      "ISBN10",      8   },
 {ROAR_HT_ISBN13,      "ISBN13",      8   },
 {-1, NULL}
};

static inline int roar_ht2gcrypt_tested (const int ht) {
 const char * name;

 if ( ht > 512 )
  return -1;

 // test the algo:
 name = gcry_md_algo_name(ht);

 if ( name == NULL )
  return -1;

 if ( *name == 0 )
  return -1;

 return ht;
}

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

ssize_t      roar_ht_digestlen (const int    ht) {
 int i;

 for (i = 0; _libroar_hashes[i].id != -1; i++)
  if ( _libroar_hashes[i].id == ht )
   return _libroar_hashes[i].dlen;

 return -1;
}

int          roar_ht_is_supported(const int    ht) {
 roar_crypto_init();

#ifdef ROAR_HAVE_LIBGCRYPT
 if ( roar_ht2gcrypt_tested(ht) == -1 )
  return 0;

 return 1;
#else
 return 0;
#endif
}

int roar_hash_buffer(void * digest, const void * data, size_t datalen, int algo) {
 roar_crypto_init();

 return roar_hash_salted_buffer(digest, data, datalen, algo, NULL, 0);
}

#ifdef ROAR_HAVE_LIBGCRYPT
static inline int roar_hash_salted_buffer_gcrypt(void * digest, const void * data, size_t datalen, int algo, const void * salt, size_t saltlen) {
 gcry_md_hd_t hdl;

 roar_crypto_init();

 algo = roar_ht2gcrypt_tested(algo);
 if ( algo == -1 )
  return -1;


 if ( salt == NULL ) {
  // optimized for unsalted:
  gcry_md_hash_buffer(algo, digest, data, datalen);
  return 0;
 } else {
  if ( gcry_md_open(&hdl, algo, 0) != 0 )
   return -1;

  gcry_md_write(hdl, data, datalen);
  gcry_md_write(hdl, salt, saltlen);

  memcpy(digest, gcry_md_read(hdl, algo), gcry_md_get_algo_dlen(algo));

  gcry_md_close(hdl);
 }

 return 0;
}
#endif

int roar_hash_salted_buffer(void * digest, const void * data, size_t datalen, int algo, const void * salt, size_t saltlen) {
 if ( digest == NULL || data == NULL )
  return -1;

#ifdef ROAR_HAVE_LIBGCRYPT
 return roar_hash_salted_buffer_gcrypt(digest, data, datalen, algo, salt, saltlen);
#else
 return -1;
#endif
}

//ll
