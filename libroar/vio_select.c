//vio_select.c:

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

ssize_t roar_vio_select(struct roar_vio_select * vios, size_t len, struct roar_vio_selecttv * rtv, struct roar_vio_selectctl * ctl) {
 struct timeval tv;
 size_t i;
 int max_fh = -1;
 int ret;
 fd_set rfds, wfds, efds;

 ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p) = ?", vios, (long long unsigned int)len, rtv, ctl);

 if ( len == 0 ) {
  ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p) = 0", vios, (long long unsigned int)len, rtv, ctl);
  return 0;
 }

 if ( vios == NULL ) {
  ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p) = -1", vios, (long long unsigned int)len, rtv, ctl);
  return -1;
 }

 // pepaer interl structs:
 for (i = 0; i < len; i++) {
  if ( vios[i].fh == -1 ) {
   vios[i].internal.action = ROAR_VIO_SELECT_ACTION_NONE;

   if ( vios[i].eventsq & ROAR_VIO_SELECT_READ ) {
    if ( roar_vio_ctl(vios[i].vio, ROAR_VIO_CTL_GET_SELECT_READ_FH, &(vios[i].internal.fh[0])) == -1 ) {
     vios[i].internal.action |= ROAR_VIO_SELECT_ACTION_VIOS;
     vios[i].internal.fh[0]   = -1;
    } else {
     vios[i].internal.action |= ROAR_VIO_SELECT_ACTION_SELECT;
    }
   }

   if ( vios[i].eventsq & ROAR_VIO_SELECT_WRITE ) {
    if ( roar_vio_ctl(vios[i].vio, ROAR_VIO_CTL_GET_SELECT_WRITE_FH, &(vios[i].internal.fh[1])) == -1 ) {
     vios[i].internal.action |= ROAR_VIO_SELECT_ACTION_VIOS;
     vios[i].internal.fh[1]   = -1;
    } else {
     vios[i].internal.action |= ROAR_VIO_SELECT_ACTION_SELECT;
    }
   }

   if ( vios[i].eventsq & ROAR_VIO_SELECT_EXCEPT ) {
    if ( roar_vio_ctl(vios[i].vio, ROAR_VIO_CTL_GET_SELECT_FH, &(vios[i].internal.fh[2])) == -1 ) {
     vios[i].internal.action |= ROAR_VIO_SELECT_ACTION_VIOS;
     vios[i].internal.fh[2]   = -1;
    } else {
     vios[i].internal.action |= ROAR_VIO_SELECT_ACTION_SELECT;
    }
   }
  } else {
   vios[i].internal.action = ROAR_VIO_SELECT_ACTION_SELECT;
   vios[i].internal.fh[0]  = vios[i].fh;
   vios[i].internal.fh[1]  = vios[i].fh;
   vios[i].internal.fh[2]  = vios[i].fh;
  }
 }

 // check:
 for (i = 0; i < len; i++) {
  if ( !( vios[i].internal.action == 0 || vios[i].internal.action == ROAR_VIO_SELECT_ACTION_SELECT ) ) {
   // we currently do not support non-select selects.
   // TODO: Fix this.
   ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p) = -1", vios, (long long unsigned int)len, rtv, ctl);
   return -1;
  }
 }

 // prepaer fdsets:
 FD_ZERO(&rfds);
 FD_ZERO(&wfds);
 FD_ZERO(&efds);

 for (i = 0; i < len; i++) {
  if ( vios[i].eventsq & ROAR_VIO_SELECT_READ ) {
   ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p): vios[i=%i] is READ, fh=%i", vios, (long long unsigned int)len, rtv, ctl, i, vios[i].internal.fh[0]);
   FD_SET(vios[i].internal.fh[0], &rfds);
   if ( vios[i].internal.fh[0] > max_fh )
    max_fh = vios[i].internal.fh[0];
  }

  if ( vios[i].eventsq & ROAR_VIO_SELECT_WRITE ) {
   ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p): vios[i=%i] is WRITE, fh=%i", vios, (long long unsigned int)len, rtv, ctl, i, vios[i].internal.fh[1]);
   FD_SET(vios[i].internal.fh[1], &wfds);
   if ( vios[i].internal.fh[1] > max_fh )
    max_fh = vios[i].internal.fh[1];
  }

  if ( vios[i].eventsq & ROAR_VIO_SELECT_EXCEPT ) {
   ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p): vios[i=%i] is EXCEPT, fh=%i", vios, (long long unsigned int)len, rtv, ctl, i, vios[i].internal.fh[2]);
   FD_SET(vios[i].internal.fh[2], &efds);
   if ( vios[i].internal.fh[2] > max_fh )
    max_fh = vios[i].internal.fh[2];
  }
 }

 // the the select:
  if ( rtv == NULL ) {
   tv.tv_sec  = 1024;
   tv.tv_usec = 0;
  } else {
   tv.tv_sec  = rtv->sec;
   tv.tv_usec = rtv->nsec / 1000;
  }


 ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p): Doing select() with max_fh=%i", vios, (long long unsigned int)len, rtv, ctl, max_fh);
 ret = select(max_fh + 1, &rfds, &wfds, &efds, &tv);
 ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p): select() returned %i", vios, (long long unsigned int)len, rtv, ctl, ret);

 // ret == -1 -> Error
 // ret ==  0 -> No data
 if ( ret < 1 ) {
  ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p) = %lli", vios, (long long unsigned int)len, rtv, ctl, (long long int)ret);
  return ret;
 }

 // set eventsa:
 for (i = 0; i < len; i++) {
  vios[i].eventsa = ROAR_VIO_SELECT_NONE;

  if ( vios[i].eventsq & ROAR_VIO_SELECT_READ )
   if ( FD_ISSET(vios[i].internal.fh[0], &rfds) )
    vios[i].eventsa |= ROAR_VIO_SELECT_READ;

  if ( vios[i].eventsq & ROAR_VIO_SELECT_WRITE )
   if ( FD_ISSET(vios[i].internal.fh[1], &wfds) )
    vios[i].eventsa |= ROAR_VIO_SELECT_WRITE;

  if ( vios[i].eventsq & ROAR_VIO_SELECT_EXCEPT )
   if ( FD_ISSET(vios[i].internal.fh[2], &efds) )
    vios[i].eventsa |= ROAR_VIO_SELECT_EXCEPT;
 }

 ROAR_DBG("roar_vio_select(vios=%p, len=%llu, rtv=%p, ctl=%p) = %lli", vios, (long long unsigned int)len, rtv, ctl, (long long int)ret);
 return ret;
}

//ll
