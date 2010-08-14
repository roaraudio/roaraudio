//error.c:

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

int    roar_err_int(struct roar_error_frame * frame) {
 if ( frame == NULL )
  return -1;

 memset(frame, 0, sizeof(struct roar_error_frame));

 frame->cmd         = -1;
 frame->ra_errno    = -1;
 frame->ra_suberrno = -1;
 frame->p_errno     = -1;

 return 0;
}

void * roar_err_buildmsg(struct roar_message * mes, struct roar_error_frame * frame) {
 int16_t * d;

 if ( mes == NULL || frame == NULL )
  return NULL;

 memset(mes,  0, sizeof(struct roar_message));

 d = (int16_t*)mes->data;

 mes->datalen = 8 + frame->datalen;
 frame->data  = &(mes->data[8]);

 mes->data[0]    = 0; // version.
 mes->data[1]    = frame->cmd;
 mes->data[2]    = frame->ra_errno;
 mes->data[3]    = frame->ra_suberrno;
 d[2]            = ROAR_HOST2NET16(frame->p_errno);
 d[3]            = ROAR_HOST2NET16(frame->flags);

 return frame->data;
}

int    roar_err_parsemsg(struct roar_message * mes, struct roar_error_frame * frame) {
 int16_t * d;

 if ( mes == NULL || frame == NULL )
  return -1;

 d = (int16_t*)mes->data;

 if ( mes->datalen < 8 )
  return -1;

 if ( mes->data[0] != 0 )
  return -1;

 frame->cmd         = mes->data[1];
 frame->ra_errno    = mes->data[2];
 frame->ra_suberrno = mes->data[3];
 frame->p_errno     = ROAR_NET2HOST16(d[2]);
 frame->flags       = ROAR_NET2HOST16(d[3]);

 frame->datalen     = mes->datalen - 8;
 frame->data        = &(mes->data[8]);

 return 0;
}

void   roar_err_clear(void) {
 roar_errno = ROAR_ERROR_NONE;
}

void   roar_err_from_errno(void) {
 switch (errno) {
#ifdef EPERM
  case EPERM:        roar_errno = ROAR_ERROR_PERM; break;
#endif
#ifdef ENOENT
  case ENOENT:       roar_errno = ROAR_ERROR_NOENT; break;
#endif
#ifdef EBADMSG
  case EBADMSG:      roar_errno = ROAR_ERROR_BADMSG; break;
#endif
#ifdef EBUSY
  case EBUSY:        roar_errno = ROAR_ERROR_BUSY; break;
#endif
#ifdef ECONNREFUSED
  case ECONNREFUSED: roar_errno = ROAR_ERROR_CONNREFUSED; break;
#endif
#ifdef ENOSYS
  case ENOSYS:       roar_errno = ROAR_ERROR_NOSYS; break;
#endif
#ifdef ENOTSUP
  case ENOTSUP:      roar_errno = ROAR_ERROR_NOTSUP; break;
#endif
#ifdef EPIPE
  case EPIPE:        roar_errno = ROAR_ERROR_PIPE; break;
#endif
#ifdef EPROTO
  case EPROTO:       roar_errno = ROAR_ERROR_PROTO; break;
#endif
#ifdef ERANGE
  case ERANGE:       roar_errno = ROAR_ERROR_RANGE; break;
#endif
#ifdef EMSGSIZE
  case EMSGSIZE:     roar_errno = ROAR_ERROR_MSGSIZE; break;
#endif
#ifdef ENOMEM
  case ENOMEM:       roar_errno = ROAR_ERROR_NOMEM; break;
#endif
#ifdef EINVAL
  case EINVAL:       roar_errno = ROAR_ERROR_INVAL; break;
#endif
  default:
    roar_errno = ROAR_ERROR_UNKNOWN;
   break;
 }
}

void   roar_err_to_errno(void) {
 switch (roar_errno) {
  case ROAR_ERROR_NONE:
    errno = 0; // just gussing
   break;
#ifdef EPERM
  case ROAR_ERROR_PERM:
    errno = EPERM;
   break;
#endif
#ifdef ENOENT
  case ROAR_ERROR_NOENT:
    errno = ENOENT;
   break;
#endif
#ifdef EBADMSG
  case ROAR_ERROR_BADMSG:
    errno = EBADMSG;
   break;
#endif
#ifdef EBUSY
  case ROAR_ERROR_BUSY:
    errno = EBUSY;
   break;
#endif
#ifdef ECONNREFUSED
  case ROAR_ERROR_CONNREFUSED:
    errno = ECONNREFUSED;
   break;
#endif
#ifdef ENOSYS
  case ROAR_ERROR_NOSYS:
    errno = ENOSYS;
   break;
#endif
#ifdef ENOTSUP
  case ROAR_ERROR_NOTSUP:
    errno = ENOTSUP;
   break;
#endif
#ifdef EPIPE
  case ROAR_ERROR_PIPE:
    errno = EPIPE;
   break;
#endif
#ifdef EPROTO
  case ROAR_ERROR_PROTO:
    errno = EPROTO;
   break;
#endif
#ifdef ERANGE
  case ROAR_ERROR_RANGE:
    errno = ERANGE;
   break;
#endif
#ifdef EMSGSIZE
  case ROAR_ERROR_MSGSIZE:
    errno = EMSGSIZE;
   break;
#endif
#ifdef ENOMEM
  case ROAR_ERROR_NOMEM:
    errno = ENOMEM;
   break;
#endif
#ifdef EINVAL
  case ROAR_ERROR_INVAL:
    errno = EINVAL;
   break;
#endif
  default:
#ifdef EINVAL
    errno = EINVAL;
#else
    errno = -1; // just guess
#endif
   break;
 }
}

//ll
