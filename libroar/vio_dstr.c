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
 {ROAR_VIO_DSTR_OBJT_FD,         "fd",
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

 if ( type == ROAR_VIO_DSTR_OBJT_EOL )
  return "<<EOL>>";

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
 struct roar_vio_dstr_chain chain[ROAR_VIO_DSTR_MAX_OBJ_PER_CHAIN];
 char * next;
 char * this;
 char * name;
 char * opts;
 char * dst;
 char * c;
 int    inopts;
 int    type;
 int    cc = 1; // current chain element

 if ( calls == NULL || dstr == NULL )
  return -1;

 if ( dnum != 0 && def == NULL )
  return -1;

 if ( (dstr = strdup(dstr)) == NULL )
  return -1;

 memset(chain, 0, sizeof(chain));

 chain[0].type = ROAR_VIO_DSTR_OBJT_INTERNAL;

 next = dstr;

 while (next != NULL) {
  if ( (cc+1) == ROAR_VIO_DSTR_MAX_OBJ_PER_CHAIN ) {
   _ret(-1);
  }

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

  ROAR_DBG("roar_vio_open_dstr_vio(*): name='%s', opts='%s', dst='%s'", name, opts, dst);

  if ( (type = roar_vio_dstr_get_type(name)) == -1 ) {
   _ret(-1);
  }

  ROAR_DBG("roar_vio_open_dstr_vio(*): type=0x%.4x(%s)", type, roar_vio_dstr_get_name(type));

  chain[cc].type     = type;
  chain[cc].opts     = opts;
  chain[cc].dst      = dst;
  chain[cc].def      = NULL;
  chain[cc].vio      = NULL;
  chain[cc].need_vio = -1;
  cc++;

 }

 chain[cc].type = ROAR_VIO_DSTR_OBJT_EOL;

 ROAR_WARN("roar_vio_open_dstr_vio(*): chain=%p", chain);

 if ( roar_vio_dstr_parse_opts(chain) == -1 ) {
  _ret(-1);
 }

 if ( roar_vio_dstr_set_defaults(chain, cc, def, dnum) == -1 ) {
  _ret(-1);
 }

 if ( roar_vio_dstr_build_chain(chain, calls, vio) == -1 ) {
  _ret(-1);
 }

 _ret(0);
}

#undef _ret

int     roar_vio_dstr_parse_opts(struct roar_vio_dstr_chain * chain) {
 if ( chain == NULL )
  return -1;

 // TODO: we should add some code here later...

 return 0;
}

int     roar_vio_dstr_set_defaults(struct roar_vio_dstr_chain * chain, int len, struct roar_vio_defaults * def, int dnum) {
 struct roar_vio_dstr_chain * c, * next;
 int i;
 int tmp[8];

 if ( chain == NULL )
  return -1;

 if ( def == NULL && dnum != 0 )
  return -1;

 if ( dnum > 1 ) /* currently not supported */
  return -1;

 if ( dnum == 0 )
  def = NULL;

 chain[len].def = def;

 for (i = len; i >= 0; i--) {
  c    = &chain[i];

  if ( i > 0 ) {
   next = &chain[i-1];
  } else {
   next = NULL;

   if ( c->type != ROAR_VIO_DSTR_OBJT_INTERNAL )
    return -1;
  }

  memset(tmp, 0, sizeof(tmp));

  ROAR_WARN("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s)", i, c->type & 0xFFFF, roar_vio_dstr_get_name(c->type));
  ROAR_WARN("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): c->def=%p, c->def->type=%i", i, c->type & 0xFFFF,
                   roar_vio_dstr_get_name(c->type), c->def, c->def == NULL ? -1 : c->def->type);

  c->need_vio = 1;

  switch (c->type) {
   case ROAR_VIO_DSTR_OBJT_INTERNAL:
     c->need_vio = 0;
    break;
   case ROAR_VIO_DSTR_OBJT_EOL:
     tmp[0] = 1;
   case ROAR_VIO_DSTR_OBJT_PASS:
   case ROAR_VIO_DSTR_OBJT_RE:
   case ROAR_VIO_DSTR_OBJT_GZIP:
   case ROAR_VIO_DSTR_OBJT_BZIP2:
   case ROAR_VIO_DSTR_OBJT_PGP:
   case ROAR_VIO_DSTR_OBJT_PGP_ENC:
   case ROAR_VIO_DSTR_OBJT_PGP_STORE:
   case ROAR_VIO_DSTR_OBJT_SSL1:
   case ROAR_VIO_DSTR_OBJT_SSL2:
   case ROAR_VIO_DSTR_OBJT_SSL3:
   case ROAR_VIO_DSTR_OBJT_TLS:
   case ROAR_VIO_DSTR_OBJT_MAGIC:
     if ( tmp[0] )
      c->need_vio = 0;

     next->def = c->def;
    break;
   case ROAR_VIO_DSTR_OBJT_FILE:
     if ( c->dst == NULL ) /* should we allow multible cascaed file: objects? */
      return -1;

     c->need_vio = 0;
     next->def = &(next->store_def);
     if ( c->def != NULL ) {
      roar_vio_dstr_init_defaults(next->def, ROAR_VIO_DEF_TYPE_FILE, c->def->o_flags, c->def->o_mode);
     } else {
      roar_vio_dstr_init_defaults(next->def, ROAR_VIO_DEF_TYPE_FILE, O_RDONLY, 0644);
     }

     if ( c->dst[0] == '/' && c->dst[1] == '/' ) {
      next->def->d.file = c->dst + 1;
     } else {
      next->def->d.file = c->dst;
     }
    break;
   case ROAR_VIO_DSTR_OBJT_FH:
     tmp[0] = 1;
   case ROAR_VIO_DSTR_OBJT_SOCKETFH:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( c->def != NULL ) {
      tmp[2] = c->def->o_flags;
      tmp[3] = c->def->o_mode;
     } else {
      tmp[2] = O_RDONLY;
      tmp[3] = 0644;
     }

     if ( !strcasecmp(c->dst, "stdin") ) {
      tmp[1] = ROAR_STDIN;
      tmp[2] = O_RDONLY;
     } else if ( !strcasecmp(c->dst, "stdout") ) {
      tmp[1] = ROAR_STDOUT;
      tmp[2] = O_WRONLY;
     } else if ( !strcasecmp(c->dst, "stderr") ) {
      tmp[1] = ROAR_STDERR;
      tmp[2] = O_WRONLY;
     } else {
      if ( sscanf(c->dst, "%i", &tmp[1]) != 1 )
       return -1;
     }

     roar_vio_dstr_init_defaults(next->def, tmp[0] ? ROAR_VIO_DEF_TYPE_FH : ROAR_VIO_DEF_TYPE_SOCKETFH, tmp[2], tmp[3]);
     next->def->d.fh = tmp[1];
    break;
   default:
    return -1;
  }

  if ( next != NULL ) {
   ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): next->def=%p, next->def->type=%i", i,
                    c->type & 0xFFFF, roar_vio_dstr_get_name(c->type),
                    next->def, next->def == NULL ? -1 : next->def->type);
  } else {
   ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): next=NULL", i,
                    c->type & 0xFFFF, roar_vio_dstr_get_name(c->type));
  }
 }

 ROAR_WARN("roar_vio_dstr_set_defaults(*) = 0");

 return 0;
}

#define _ret(x) roar_vio_close(calls); ROAR_WARN("roar_vio_dstr_build_chain(*) = %i", (x)); return (x)

int     roar_vio_dstr_build_chain(struct roar_vio_dstr_chain * chain, struct roar_vio_calls * calls,
                                  struct roar_vio_calls * vio) {
 struct roar_vio_dstr_chain * c;
 struct roar_vio_defaults   * def;
 struct roar_vio_calls      * tc, * prev;
 int i;

 ROAR_WARN("roar_vio_dstr_build_chain(*) = ?");

 if ( chain == NULL || calls == NULL )
  return -1;

 if ( roar_vio_open_stack(calls) == -1 )
  return -1;

 ROAR_DBG("roar_vio_dstr_build_chain(*): chain=%p", chain);

 if ( (def = chain->def) != NULL ) {
  if ( (tc = malloc(sizeof(struct roar_vio_calls))) == NULL ) {
   _ret(-1);
  }

  if ( roar_vio_stack_add(calls, tc) == -1 ) {
   _ret(-1);
  }

  switch (def->type) {
   case ROAR_VIO_DEF_TYPE_FILE:
     if ( roar_vio_open_file(tc, def->d.file, def->o_flags, def->o_mode) == -1 ) {
      _ret(-1);
     }
    break;
   case ROAR_VIO_DEF_TYPE_SOCKET:
     _ret(-1);
    break;
   case ROAR_VIO_DEF_TYPE_FH:
     if ( roar_vio_open_fh(tc, def->d.fh) == -1 ) {
      _ret(-1);
     }
    break;
   case ROAR_VIO_DEF_TYPE_SOCKETFH:
     if ( roar_vio_open_fh_socket(tc, def->d.fh) == -1 ) {
      _ret(-1);
     }
    break;
   default:
     _ret(-1);
  }
  prev = tc;
 } else {
  prev = vio;
 }

 for (i = 0; (c = &chain[i])->type != ROAR_VIO_DSTR_OBJT_EOL; i++) {
  if ( c->need_vio ) {
   if ( (tc = malloc(sizeof(struct roar_vio_calls))) == NULL ) {
    _ret(-1);
   }

   if ( roar_vio_stack_add(calls, tc) == -1 ) {
    _ret(-1);
   }

   switch (c->type) {
    case ROAR_VIO_DSTR_OBJT_PASS:
      if ( roar_vio_open_pass(tc, prev) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_RE:
      if ( roar_vio_open_re(tc, prev) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_GZIP:
      if ( roar_vio_open_gzip(tc, prev, -1) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_BZIP2:
    case ROAR_VIO_DSTR_OBJT_PGP:
      if ( roar_vio_open_pgp_decrypt(tc, prev, NULL) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_PGP_ENC:
    case ROAR_VIO_DSTR_OBJT_PGP_STORE:
      if ( roar_vio_open_pgp_store(tc, prev, ROAR_VIO_PGP_OPTS_NONE) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_SSL1:
    case ROAR_VIO_DSTR_OBJT_SSL2:
    case ROAR_VIO_DSTR_OBJT_SSL3:
    case ROAR_VIO_DSTR_OBJT_TLS:
    case ROAR_VIO_DSTR_OBJT_MAGIC:
      _ret(-1);
     break;
    default:
      _ret(-1);
   }

   prev = tc;
  } // else we can skip to the next :)
 }

 ROAR_WARN("roar_vio_dstr_build_chain(*) = 0");
 return 0;
}

#undef _ret

//ll
