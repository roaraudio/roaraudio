//vio_dstr.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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

#ifndef ROAR_WITHOUT_VIO_DSTR
struct _roar_vio_dstr_type {
 int    id;
 char * name;
 int (* setdef) (struct roar_vio_dstr_chain * cur,   struct roar_vio_dstr_chain * next);
 int (* openvio)(struct roar_vio_calls      * calls, struct roar_vio_calls      * dst, struct roar_vio_dstr_chain * cur);
 int    pdeftype[16];
} _roar_vio_dstr_objs[] = {
/*
grep '^#define ROAR_VIO_DSTR_OBJT_' vio_dstr.h | cut -d' ' -f2 | while read objt; do name=`cut -d_ -f5,6,7,8,9,10 <<<$objt | tr A-Z a-z`; echo -e " {$objt,\t \"$name\","; echo "      {ROAR_VIO_DEF_TYPE_EOL}},"; done;
*/
 {ROAR_VIO_DSTR_OBJT_FILE,       "file",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_FH,         "fh",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_FD,         "fd",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKETFH,   "socketfh",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_PASS,       "pass",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_RE,         "re",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_JUMBO,      "jumbo", /* TODO */
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_EXEC,       "exec",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_SOCKET,     "socket",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_UNIX,       "unix",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_DECNET,     "decnet",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TCP,        "tcp",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_UDP,        "udp",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TCP6,       "tcp6",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_UDP6,       "udp6",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_SOCKS,      "socks",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS4,     "socks4",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS4A,    "socks4a",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS4D,    "socks4d",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SOCKS5,     "socks5",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSH,        "ssh",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_HTTP09,     "http09",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_HTTP10,     "http10",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_HTTP11,     "http11",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_HTTP,       "http",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_GOPHER,     "gopher",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_GOPHER_PLUS,"gopher+",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_ICY,        "icy",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_RTP2,       "rtp2",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_RTP,        "rtp",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_GZIP,       "gzip",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_BZIP2,      "bzip2",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_PGP,        "pgp",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_PGP_ENC,    "pgp_enc",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_PGP_STORE,  "pgp_store",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSL1,       "ssl1",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSL2,       "ssl2",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSL3,       "ssl3",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TLS,        "tls",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_SSLTLS,     "ssltls",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_TRANSCODE,  "transcode", /* TODO  */
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_RAUM,       "raum",      /* TODO */
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_OGG,        "ogg",       /* TODO */
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TAR,        "tar",       /* TODO */
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},


 {ROAR_VIO_DSTR_OBJT_MAGIC,      "magic",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_TANTALOS,   "tantalos",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_EOL}},

 {ROAR_VIO_DSTR_OBJT_INTERNAL, "INTERNAL",
      NULL, NULL,
      {ROAR_VIO_DEF_TYPE_FILE, ROAR_VIO_DEF_TYPE_SOCKET, ROAR_VIO_DEF_TYPE_FH, ROAR_VIO_DEF_TYPE_SOCKETFH,
       ROAR_VIO_DEF_TYPE_EOL}},
 {ROAR_VIO_DSTR_OBJT_EOL, NULL, NULL, NULL, {ROAR_VIO_DEF_TYPE_EOL}}
};

int     roar_vio_dstr_get_type(char * str) {
 int i;

 for (i = 0; _roar_vio_dstr_objs[i].id != ROAR_VIO_DSTR_OBJT_EOL; i++) {
  if ( strcasecmp(_roar_vio_dstr_objs[i].name, str) == 0 )
   return _roar_vio_dstr_objs[i].id;
 }

 return -1;
}

struct _roar_vio_dstr_type * roar_vio_dstr_get_by_type (int type) {
 int i;

 for (i = 0; _roar_vio_dstr_objs[i].id != ROAR_VIO_DSTR_OBJT_EOL; i++) {
  if ( _roar_vio_dstr_objs[i].id == type )
   return &(_roar_vio_dstr_objs[i]);
 }

 return NULL;
}

char *  roar_vio_dstr_get_name(int type) {
 struct _roar_vio_dstr_type * ret;

 if ( (ret = roar_vio_dstr_get_by_type(type)) != NULL )
  return ret->name;

 if ( type == ROAR_VIO_DSTR_OBJT_EOL )
  return "<<EOL>>";

 return NULL;
}

int     roar_vio_dstr_register_type(int   type,
                                    char *name,
                                    int (*setdef) (struct roar_vio_dstr_chain * cur,
                                                   struct roar_vio_dstr_chain * next),
                                    int (*openvio)(struct roar_vio_calls      * calls,
                                                   struct roar_vio_calls      * dst,
                                                   struct roar_vio_dstr_chain * cur)) {
 struct _roar_vio_dstr_type * ret;

 if ( (ret = roar_vio_dstr_get_by_type(type)) == NULL ) /* we can currently not register new types */
  return -1;


 // check if things are allready set, we do not want to allow overwrite here.
 if ( setdef != NULL && ret->setdef != NULL )
  return -1;

 if ( openvio != NULL && ret->openvio != NULL )
  return -1;

 if ( setdef != NULL )
  ret->setdef = setdef;

 if ( openvio != NULL )
  ret->openvio = openvio;

 return 0;
}

static void _roar_vio_dstr_init_otherlibs (void) {
 roar_dl_ra_init(ROAR_DL_HANDLE_DEFAULT, "libroardsp");
 roar_dl_ra_init(ROAR_DL_HANDLE_DEFAULT, "libroareio");
 roar_dl_ra_init(ROAR_DL_HANDLE_DEFAULT, "libroarlight");
 roar_dl_ra_init(ROAR_DL_HANDLE_DEFAULT, "libroarmidi");
}

#endif

int     roar_vio_dstr_init_defaults (struct roar_vio_defaults * def, int type, int o_flags, mode_t o_mode) {
 if ( def == NULL )
  return -1;

 memset(def, 0, sizeof(struct roar_vio_defaults));

 def->type    = type;
 def->o_flags = o_flags;
 def->o_mode  = o_mode;

 return 0;
}

int     roar_vio_dstr_init_defaults_c (struct roar_vio_defaults * def, int type, struct roar_vio_defaults * odef, int o_flags) {
 if ( o_flags < 1 )
  o_flags = O_RDONLY;

 if ( odef == NULL ) {
  return roar_vio_dstr_init_defaults(def, type, o_flags, 0644);
 } else {
  return roar_vio_dstr_init_defaults(def, type, odef->o_flags, odef->o_mode);
 }
}

#ifndef ROAR_WITHOUT_VIO_DSTR
int     roar_vio_open_default (struct roar_vio_calls * calls, struct roar_vio_defaults * def, char * opts) {
 ROAR_DBG("roar_vio_open_default(calls=%p, def=%p, opts='%s') = ?", calls, def, opts);

 if ( calls == NULL || def == NULL )
  return -1;

 switch (def->type) {
  case ROAR_VIO_DEF_TYPE_NONE:
   break;
  case ROAR_VIO_DEF_TYPE_FILE:
    if ( roar_vio_open_file(calls, def->d.file, def->o_flags, def->o_mode) == -1 )
     return -1;
   break;
  case ROAR_VIO_DEF_TYPE_SOCKET:
     if ( roar_vio_open_def_socket(calls, def, opts) == -1 )
      return -1;
   break;
  case ROAR_VIO_DEF_TYPE_FH:
    if ( roar_vio_open_fh(calls, def->d.fh) == -1 )
     return -1;
   break;
  case ROAR_VIO_DEF_TYPE_SOCKETFH:
    if ( roar_vio_open_fh_socket(calls, def->d.fh) == -1 )
     return -1;
   break;
  default:
    return -1;
 }

 return 0;
}
#endif

int     roar_vio_open_dstr    (struct roar_vio_calls * calls, char * dstr, struct roar_vio_defaults * def, int dnum) {
 return roar_vio_open_dstr_vio(calls, dstr, def, dnum, NULL);
}

#define _ret(x) roar_mm_free(dstr); return (x)

int     roar_vio_open_dstr_vio(struct roar_vio_calls * calls,
                               char * dstr, struct roar_vio_defaults * def, int dnum,
                               struct roar_vio_calls * vio) {
#ifndef ROAR_WITHOUT_VIO_DSTR
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

 if ( (dstr = roar_mm_strdup(dstr)) == NULL )
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

 ROAR_DBG("roar_vio_open_dstr_vio(*): chain=%p", chain);

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
#else
 return -1;
#endif
}

#undef _ret

#ifndef ROAR_WITHOUT_VIO_DSTR
int     roar_vio_dstr_parse_opts(struct roar_vio_dstr_chain * chain) {
 if ( chain == NULL )
  return -1;

 // TODO: we should add some code here later...

 return 0;
}

int     roar_vio_dstr_set_defaults(struct roar_vio_dstr_chain * chain, int len, struct roar_vio_defaults * def, int dnum) {
 struct _roar_vio_dstr_type * type;
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

  ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s)", i, c->type & 0xFFFF, roar_vio_dstr_get_name(c->type));
  ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): c->def=%p, c->def->type=%i", i, c->type & 0xFFFF,
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
   case ROAR_VIO_DSTR_OBJT_RTP2: // we currently only forward the defs
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
   case ROAR_VIO_DSTR_OBJT_TANTALOS:
     next->def = &(next->store_def);
     roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_NONE, NULL, -1);
    break;
   case ROAR_VIO_DSTR_OBJT_FILE:
     if ( c->dst == NULL ) /* should we allow multible cascaed file: objects? */
      return -1;

     c->need_vio = 0;
     next->def = &(next->store_def);
     roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_FILE, c->def, -1);

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
#ifdef ROAR_HAVE_UNIX
   case ROAR_VIO_DSTR_OBJT_UNIX:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( c->dst == NULL ) { // we don't have a destination? -> slow way
      if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
       return -1;

      if ( roar_vio_socket_init_dstr_def(next->def, c->dst, AF_UNIX, SOCK_STREAM, c->def) == -1 )
       return -1;
     } else {                // we have a destination? -> fast way
      if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
       return -1;

      if ( roar_vio_socket_init_unix_def(next->def, c->dst) == -1 )
       return -1;
     }
    break;
#endif
   case ROAR_VIO_DSTR_OBJT_SOCKET:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
      return -1;

     if ( roar_vio_socket_init_dstr_def(next->def, c->dst, -1, SOCK_STREAM, c->def) == -1 )
      return -1;
    break;
#ifdef ROAR_HAVE_LIBDNET
   case ROAR_VIO_DSTR_OBJT_DECNET:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
      return -1;

     if ( roar_vio_socket_init_dstr_def(next->def, c->dst, AF_DECnet, SOCK_STREAM, c->def) == -1 )
      return -1;
    break;
#endif
#ifdef ROAR_HAVE_IPV4
   case ROAR_VIO_DSTR_OBJT_TCP:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
      return -1;

     if ( roar_vio_socket_init_dstr_def(next->def, c->dst, AF_INET, SOCK_STREAM, c->def) == -1 )
      return -1;
    break;
   case ROAR_VIO_DSTR_OBJT_UDP:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
      return -1;

     if ( roar_vio_socket_init_dstr_def(next->def, c->dst, AF_INET, SOCK_DGRAM, c->def) == -1 )
      return -1;
    break;
#endif
#ifdef ROAR_HAVE_IPV6
   case ROAR_VIO_DSTR_OBJT_TCP6:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
      return -1;

     if ( roar_vio_socket_init_dstr_def(next->def, c->dst, AF_INET6, SOCK_STREAM, c->def) == -1 )
      return -1;
    break;
   case ROAR_VIO_DSTR_OBJT_UDP6:
     c->need_vio = 0;
     next->def = &(next->store_def);

     if ( roar_vio_dstr_init_defaults_c(next->def, ROAR_VIO_DEF_TYPE_SOCKET, c->def, O_WRONLY) == -1 )
      return -1;

     if ( roar_vio_socket_init_dstr_def(next->def, c->dst, AF_INET6, SOCK_DGRAM, c->def) == -1 )
      return -1;
    break;
#endif
   case ROAR_VIO_DSTR_OBJT_HTTP09:
   case ROAR_VIO_DSTR_OBJT_HTTP10:
   case ROAR_VIO_DSTR_OBJT_HTTP11:
     c->need_vio = 1;
     next->def = &(next->store_def);

     if ( roar_vio_proto_init_def(next->def, c->dst, ROAR_VIO_PROTO_P_HTTP, c->def) == -1 )
      return -1;
    break;
   case ROAR_VIO_DSTR_OBJT_GOPHER:
   case ROAR_VIO_DSTR_OBJT_GOPHER_PLUS:
     c->need_vio = 1;
     next->def = &(next->store_def);

     if ( roar_vio_proto_init_def(next->def, c->dst, ROAR_VIO_PROTO_P_GOPHER, c->def) == -1 )
      return -1;
    break;
   case ROAR_VIO_DSTR_OBJT_ICY:
     c->need_vio = 1;
     next->def = &(next->store_def);

     if ( roar_vio_proto_init_def(next->def, c->dst, ROAR_VIO_PROTO_P_ICY, c->def) == -1 )
      return -1;
    break;
   default:
     if ( (type = roar_vio_dstr_get_by_type(c->type)) == NULL ) {
      return -1;
     }

     if ( type->setdef == NULL )
      _roar_vio_dstr_init_otherlibs();

     if ( type->setdef == NULL ) {
      return -1;
     }

     if ( type->setdef(c, next) == -1 ) {
      return -1;
     }
  }

  if ( next != NULL ) {
   ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): next->def=%p, next->def->type=%i", i,
                    c->type & 0xFFFF, roar_vio_dstr_get_name(c->type),
                    next->def, next->def == NULL ? -1 : next->def->type);
   if ( next->def != NULL ) {
    ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): next->def->o_flags=%i", i,
                     c->type & 0xFFFF, roar_vio_dstr_get_name(c->type),
                     next->def->o_flags);
   }
  } else {
   ROAR_DBG("roar_vio_dstr_set_defaults(*): i=%i, c->type=0x%.4x(%s): next=NULL", i,
                    c->type & 0xFFFF, roar_vio_dstr_get_name(c->type));
  }
 }

 ROAR_DBG("roar_vio_dstr_set_defaults(*) = 0");

 return 0;
}

#define _ret(x) roar_vio_close(calls); ROAR_DBG("roar_vio_dstr_build_chain(*) = %i", (x)); return (x)

int     roar_vio_dstr_build_chain(struct roar_vio_dstr_chain * chain, struct roar_vio_calls * calls,
                                  struct roar_vio_calls * vio) {
 struct _roar_vio_dstr_type * type;
 struct roar_vio_dstr_chain * c;
 struct roar_vio_defaults   * def;
 struct roar_vio_calls      * tc, * prev;
 int i;

 ROAR_DBG("roar_vio_dstr_build_chain(*) = ?");

 if ( chain == NULL || calls == NULL )
  return -1;

 if ( roar_vio_open_stack(calls) == -1 )
  return -1;

 ROAR_DBG("roar_vio_dstr_build_chain(*): chain=%p", chain);

 if ( (def = chain->def) != NULL ) {
  if ( (tc = malloc(sizeof(struct roar_vio_calls))) == NULL ) {
   _ret(-1);
  }

  if ( roar_vio_init_calls(tc) == -1 ) {
   free(tc);
   _ret(-1);
  }

  if ( roar_vio_stack_add(calls, tc) == -1 ) {
   _ret(-1);
  }

  if ( chain->opts == NULL ) {
   if ( chain[1].type != ROAR_VIO_DSTR_OBJT_EOL ) {
    chain->opts = chain[1].opts;
   }
  }

  if ( roar_vio_open_default(tc, def, chain->opts) == -1 ) {
   _ret(-1);
  }

  prev = tc;
 } else {
  prev = vio;
 }

 for (i = 0; (c = &chain[i])->type != ROAR_VIO_DSTR_OBJT_EOL; i++) {
  ROAR_DBG("roar_vio_dstr_build_chain(*): i=%i, c->type=0x%.4x(%s): need_vio=%i, def->o_flags=%i", i,
                   c->type & 0xFFFF, roar_vio_dstr_get_name(c->type), c->need_vio, c->def->o_flags);

  if ( c->need_vio ) {
   if ( (tc = malloc(sizeof(struct roar_vio_calls))) == NULL ) {
    _ret(-1);
   }

   if ( roar_vio_init_calls(tc) == -1 ) {
    free(tc);
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
    case ROAR_VIO_DSTR_OBJT_HTTP09:
    case ROAR_VIO_DSTR_OBJT_HTTP10:
    case ROAR_VIO_DSTR_OBJT_HTTP11:
      if ( roar_vio_open_proto(tc, prev, c->dst, ROAR_VIO_PROTO_P_HTTP, c->def) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_GOPHER:
    case ROAR_VIO_DSTR_OBJT_GOPHER_PLUS:
      if ( roar_vio_open_proto(tc, prev, c->dst, ROAR_VIO_PROTO_P_GOPHER, c->def) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_ICY:
      if ( roar_vio_open_proto(tc, prev, c->dst, ROAR_VIO_PROTO_P_ICY, c->def) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_RTP2:
      if ( roar_vio_open_rtp(tc, prev, c->dst, c->def) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_PGP_ENC:
    case ROAR_VIO_DSTR_OBJT_PGP_STORE:
      if ( roar_vio_open_pgp_store(tc, prev, ROAR_VIO_PGP_OPTS_NONE) == -1 ) {
       _ret(-1);
      }
     break;
    case ROAR_VIO_DSTR_OBJT_TANTALOS:
      if ( roar_vio_open_tantalos(tc, prev, c->dst, c->def) == -1 ) {
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
      if ( (type = roar_vio_dstr_get_by_type(c->type)) == NULL ) {
       _ret(-1);
      }

      if ( type->openvio == NULL )
       _roar_vio_dstr_init_otherlibs();

      if ( type->openvio == NULL ) {
       _ret(-1);
      }

      if ( type->openvio(tc, prev, c) == -1 ) {
       _ret(-1);
      }
   }

   prev = tc;
  } // else we can skip to the next :)
 }

 ROAR_DBG("roar_vio_dstr_build_chain(*) = 0");
 return 0;
}

#undef _ret
#endif

//ll
