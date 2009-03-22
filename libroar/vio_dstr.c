//vio_dstr.c:

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

#include "libroar.h"

struct {
 int    id;
 char * name;
 int    pdeftype[16];
} _roar_vio_dstr_objs[] = {
/*
grep '^#define ROAR_VIO_DSTR_OBJT_' vio_dstr.h | cut -d' ' -f2 | while read objt; do name=`cut -d_ -f5,6,7,8,9,10 <<<$objt | tr A-Z a-z`; echo -e " {$objt,\t \"$name\","; echo "      {ROAR_VIO_DEF_TYPE_EOL}},"; done;
*/
 {ROAR_VIO_DSTR_OBJT_FILE,       "file",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_FH,         "fh",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKETFH,   "socketfh",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_PASS,       "pass",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_RE,         "re",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_EXEC,       "exec",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_SOCKET,     "socket",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_UNIX,       "unix",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_DECNET,     "decnet",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TCP,        "tcp",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_UDP,        "udp",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TCP6,       "tcp6",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_UDP6,       "udp6",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_SOCKS,      "socks",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS4,     "socks4",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS4A,    "socks4a",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS4D,    "socks4d",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS5,     "socks5",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSH,        "ssh",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_HTTP09,     "http09",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_HTTP10,     "http10",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_HTTP11,     "http11",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_HTTP,       "http",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_GOPHER,     "gopher",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_GZIP,       "gzip",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_BZIP2,      "bzip2",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_PGP,        "pgp",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_PGP_ENC,    "pgp_enc",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_PGP_STORE,  "pgp_store",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSL1,       "ssl1",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSL2,       "ssl2",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSL3,       "ssl3",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TLS,        "tls",
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSLTLS,     "ssltls",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_MAGIC,      "magic",
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_INTERNAL, "INTERNAL",
      {ROAR_VIO_DEF_TYPE_FILE, ROAR_VIO_DEF_TYPE_SOCKET, ROAR_VIO_DEF_TYPE_FH, ROAR_VIO_DEF_TYPE_SOCKETFH,
       ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_EOL, NULL, {ROAR_VIO_DEF_TYPE_EOL}}
};

int     roar_vio_dstr_get_type(char * str) {
 int i;

 for (i = 0; _roar_vio_dstr_objs[i].id != ROAR_VIO_DSTR_OBJT_EOL; i++) {
  if ( strcasecmp(_roar_vio_dstr_objs[i].name, str) == 0 )
   return _roar_vio_dstr_objs[i].id;
 }

 return -1;
}

char *  roar_vio_dstr_get_name(int type) {
 int i;

 for (i = 0; _roar_vio_dstr_objs[i].id != ROAR_VIO_DSTR_OBJT_EOL; i++) {
  if ( _roar_vio_dstr_objs[i].id == type )
   return _roar_vio_dstr_objs[i].name;
 }

 return NULL;
}

int     roar_vio_dstr_init_defaults (struct roar_vio_defaults * def, int type, int o_flags, mode_t o_mode) {
 if ( def == NULL )
  return -1;

 memset(def, 0, sizeof(struct roar_vio_defaults));

 def->type    = type;
 def->o_flags = o_flags;
 def->o_mode  = o_mode;

 return 0;
}

int     roar_vio_open_dstr    (struct roar_vio_calls * calls, char * dstr, struct roar_vio_defaults * def, int dnum) {
 return roar_vio_open_dstr_vio(calls, dstr, def, dnum, NULL);
}

#define _ret(x) free(dstr); return (x)

int     roar_vio_open_dstr_vio(struct roar_vio_calls * calls,
                               char * dstr, struct roar_vio_defaults * def, int dnum,
                               struct roar_vio_calls * vio) {
 char * next;
 char * this;
 char * name;
 char * opts;
 char * dst;
 char * c;
 int    inopts;
 int    type;

 if ( calls == NULL || dstr == NULL )
  return -1;

 if ( dnum != 0 && def == NULL )
  return -1;

 if ( dnum > 1 )
  return -1;

 if ( (dstr = strdup(dstr)) == NULL )
  return -1;

 next = dstr;

 while (next != NULL) {
  this = next;
  next = strstr(next, "##");

  if (next != NULL) {
   *next = 0;
   next += 2;
  }

  // we have the current token in 'this'.

  opts   = NULL;
  dst    = NULL;

  if ( strstr(this, ":") != NULL ) {
   name   = this;
   inopts = 0;
   for (c = this; *c != 0; c++) {
    if ( *c == '[' ) {
     *c     = 0;
     opts   = c + 1;
     inopts = 1;
    } else if ( *c == ']' &&  inopts ) {
     *c     = 0;
     inopts = 0;
    } else if ( *c == ':' && !inopts ) {
     *c     = 0;
     dst    = *(c+1) == 0 ? NULL : c + 1;
     break;
    }
   }
  } else {
   // we need to guess that this is here...
   // currently we guess this is a file in all cases
   name = "file";
   dst  = this;
  }

  ROAR_WARN("roar_vio_open_dstr_vio(*): name='%s', opts='%s', dst='%s'", name, opts, dst);

  if ( (type = roar_vio_dstr_get_type(name)) == -1 ) {
   _ret(-1);
  }

  ROAR_WARN("roar_vio_open_dstr_vio(*): type=0x%.4x(%s)", type, roar_vio_dstr_get_name(type));

 }

 _ret(-1);
}

//ll
