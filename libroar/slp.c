//slp.c:

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

SLPBoolean roar_slp_url_callback(SLPHandle        hslp,
                                 const char     * srvurl,
                                 unsigned short   lifetime,
                                 SLPError         errcode,
                                 void           * cookie) {
#ifdef ROAR_HAVE_LIBSLP
 struct roar_slp_cookie * self = cookie;

 ROAR_DBG("roar_slp_url_callback(*) = ?");

 if (errcode == SLP_OK || errcode == SLP_LAST_CALL) {
  self->callbackerr = SLP_OK;

  if ( srvurl == NULL ) /* hu? */
   return SLP_TRUE;

  if ( self->matchcount == ROAR_SLP_MAX_MATCHES )
   return SLP_FALSE;

  strncpy(self->match[self->matchcount].url, srvurl, ROAR_SLP_MAX_URL_LEN);

  self->match[self->matchcount].url[ROAR_SLP_MAX_URL_LEN-1] = 0;

  self->match[self->matchcount].tod  = time(NULL);
  self->match[self->matchcount].tod += lifetime;

  self->matchcount++;
 } else {
  self->callbackerr = errcode;
 }


 /* return SLP_TRUE because we want to be called again */
 /* if more services were found                        */

 return SLP_TRUE;
#else
 return SLP_FALSE;
#endif
}

int roar_slp_search          (struct roar_slp_cookie * cookie, char * type) {
#ifdef ROAR_HAVE_LIBSLP
 SLPError err;
 SLPHandle hslp;

 ROAR_DBG("roar_slp_search(cookie=%p, type='%s') = ?", cookie, type);

 if ( cookie->search != NULL ) /* currently only non-search filter mode supported */
  return -1;

 err = SLPOpen("en", SLP_FALSE, &hslp);
 if (err != SLP_OK) {
  return -1;
 }

 ROAR_DBG("roar_slp_search(*) = ?");

 err = SLPFindSrvs(hslp,
                   type,
                   0,                    /* use configured scopes */
                   0,                    /* no attr filter        */
                   roar_slp_url_callback,
                   cookie);

 ROAR_DBG("roar_slp_search(*) = ?");

  /* err may contain an error code that occurred as the slp library    */
  /* _prepared_ to make the call.                                     */
  if (err != SLP_OK) {
   return -1;
  }

 /* callbackerr may contain an error code (that was assigned through */
 /* the callback cookie) that occurred as slp packets were sent on    */
 /* the wire */
 if (cookie->callbackerr != SLP_OK) {
  return -1;
 }

 ROAR_DBG("roar_slp_search(*) = ?");

 /* Now that we're done using slp, close the slp handle */
 //SLPClose(hslp);

 ROAR_DBG("roar_slp_search(*) = ?");

 return 0;
#else
 return -1;
#endif
}

int roar_slp_cookie_init     (struct roar_slp_cookie * cookie, struct roar_slp_search * search) {
 if ( cookie == NULL )
  return -1;

 memset(cookie, 0, sizeof(struct roar_slp_cookie));

 cookie->search = search;

 return 0;
}


char * roar_slp_find_roard   (int nocache) {
 static char addr[80];

 if ( roar_slp_find_roard_r(addr, 80, nocache) == -1 )
  return NULL;

 return addr;
}

int    roar_slp_find_roard_r (char * addr, size_t len, int nocache) {
 static struct roar_slp_match    cache  = {"", 0};
        struct roar_slp_cookie   cookie;
 int                             offset = 0;
 char                          * url;

 ROAR_DBG("roar_slp_find_roard_r(addr=%p, len=%i) = ?", addr, len);

 if ( addr == NULL || len == 0 )
  return -1;

 *addr = 0; // just in case...

 if ( nocache || cache.tod < time(NULL) ) {
#ifdef DEBUG
  if ( nocache ) {
   ROAR_DBG("roar_slp_find_roard_r(*): forced ignoring of cache, doing a new lookup.");
  }
#endif
  ROAR_DBG("roar_slp_find_roard_r(*): cache too old, searching for a new server...");
  ROAR_DBG("roar_slp_find_roard_r(*) = ?");

  if ( roar_slp_cookie_init(&cookie, NULL) == -1 )
   return -1;

  ROAR_DBG("roar_slp_find_roard_r(*) = ?");

  if ( roar_slp_search(&cookie, ROAR_SLP_URL_TYPE) == -1 )
   return -1;

  ROAR_DBG("roar_slp_find_roard_r(*) = ?");

  if ( cookie.matchcount == 0 )
   return -1;

  ROAR_DBG("roar_slp_find_roard_r(*) = ?");

  url = cookie.match[0].url;

  ROAR_DBG("roar_slp_find_roard_r(*): found new server, caching it");
  memcpy(&cache, &(cookie.match[0]), sizeof(cache));
 } else {
  ROAR_DBG("roar_slp_find_roard_r(*): cache within TTL, no need to search for server, using cache.");
  url = cache.url;
 }

 if ( !strncmp(url, ROAR_SLP_URL_TYPE "://", ROAR_SLP_URL_TYPE_LEN + 3) )
  offset = 28;

 ROAR_DBG("roar_slp_find_roard_r(*): url='%s'", cookie.match[0].url);

 strncpy(addr, &(url[offset]), len);
 addr[len-1] = 0; // also just in case.

 ROAR_DBG("roar_slp_find_roard_r(*): addr='%s'", addr);

 return 0;
}

//ll
