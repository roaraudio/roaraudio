//authfile.c:

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

struct roar_authfile {
 size_t refc;
 size_t lockc;
 int type;
 int rw;
 int version;
 size_t magiclen;
 struct roar_vio_calls vio;
};

struct roar_authfile * roar_authfile_open(int type, const char * filename, int rw, int version) {
 struct roar_vio_defaults def;
 struct roar_authfile * ret;
 size_t magiclen = 0;

 switch (type) {
  case ROAR_AUTHFILE_TYPE_ESD:
  case ROAR_AUTHFILE_TYPE_PULSE:
   break;
  default:
    return NULL;
   break;
 }

 if ( roar_vio_dstr_init_defaults(&def, ROAR_VIO_DEF_TYPE_NONE, rw ? O_RDWR|O_CREAT : O_RDONLY, 0600) == -1 )
  return NULL;

 if ( (ret = roar_mm_malloc(sizeof(struct roar_authfile))) == NULL )
  return NULL;

 memset(ret, 0, sizeof(struct roar_authfile));

 ret->refc     = 1;
 ret->lockc    = 0;
 ret->type     = type;
 ret->rw       = rw;
 ret->version  = version;
 ret->magiclen = magiclen;

 filename = roar_mm_strdup(filename);

 if ( roar_vio_open_dstr(&(ret->vio), (char*)filename, &def, 1) == -1 ) {
  roar_mm_free((void*)filename);
  roar_mm_free(ret);
  return NULL;
 }

 roar_mm_free((void*)filename);

 return ret;
}

int roar_authfile_close(struct roar_authfile * authfile) {
 if ( authfile == NULL )
  return -1;

 roar_vio_close(&(authfile->vio));

 roar_mm_free(authfile);

 return 0;
}

int roar_authfile_lock(struct roar_authfile * authfile) {
 if ( authfile == NULL )
  return -1;

 // TODO: implement real locking here.
 return 0;
}

int roar_authfile_unlock(struct roar_authfile * authfile) {
 if ( authfile == NULL )
  return -1;

 // TODO: implement real unlocking here.
 return 0;
}

int roar_authfile_sync(struct roar_authfile * authfile) {
 if ( authfile == NULL )
  return -1;

 return roar_vio_sync(&(authfile->vio));
}


struct roar_authfile_key * roar_authfile_key_new(int type, size_t len, const char * addr) {
 struct roar_authfile_key * ret;
 size_t addrlen;
 size_t retlen;

 if ( addr == NULL ) {
  addrlen = 0;
 } else {
  addrlen = strlen(addr) + 1;
 }

 retlen = sizeof(struct roar_authfile_key) + len + addrlen;

 if ( (ret = roar_mm_malloc(retlen)) == NULL )
  return NULL;

 memset(ret, 0, retlen);

 ret->refc  = 1;
 ret->type  = type;
 ret->index = -1;

 if ( addrlen == 0 ) {
  ret->address = NULL;
 } else {
  ret->address = (const void *)ret+sizeof(struct roar_authfile_key);
  memcpy((void*)ret->address, addr, addrlen);
 }

 ret->data = ret + sizeof(struct roar_authfile_key) + addrlen;

 ret->len  = len;

 return ret;
}

int roar_authfile_key_ref(struct roar_authfile_key * key) {
 if ( key == NULL )
  return -1;

 key->refc++;

 return 0;
}

int roar_authfile_key_unref(struct roar_authfile_key * key) {
 if ( key == NULL )
  return -1;

 if ( key->refc == 0 ) {
  ROAR_ERR("roar_authfile_key_unref(key=%p): Key has reference count of zero. This is bad. assuming refc=1", key);
  key->refc = 1;
 }

 key->refc--;

 if ( key->refc > 0 )
  return 0;

 roar_mm_free(key);

 return 0;
}

int roar_authfile_add_key(struct roar_authfile * authfile, struct roar_authfile_key * key);

struct roar_authfile_key * roar_authfile_lookup_key(struct roar_authfile * authfile,
                                                    int type, int minindex, const char * address) {
 struct roar_authfile_key * ret = NULL;
 ssize_t len;

 if ( authfile == NULL )
  return NULL;

 switch (authfile->type) {
  case ROAR_AUTHFILE_TYPE_ESD:
  case ROAR_AUTHFILE_TYPE_PULSE:
    if ( type != ROAR_AUTH_T_COOKIE || minindex > 0 )
     return NULL;

    if ( (ret = roar_authfile_key_new(type, 256, NULL)) == NULL )
     return NULL;

    len = roar_vio_read(&(authfile->vio), ret->data, ret->len);

    if ( len == -1 ) {
     roar_authfile_key_unref(ret);
     return NULL;
    }

    ret->len   = len;
    ret->index = 0;
   break;
  default:
    return NULL;
   break;
 }

 return ret;
}

//ll
