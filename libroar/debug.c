//debug.c:

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

static int                     roar_debug_stderr_mode = ROAR_DEBUG_MODE_SYSIO;
static int                     roar_debug_stderr_fh   = ROAR_STDERR;
static struct roar_vio_calls * roar_debug_stderr_vio  = NULL;

void roar_debug_warn_sysio_real(char * func, char * newfunc, char * info) {
 struct roar_libroar_config * config = roar_libroar_get_config();

 if ( !roar_libroar_iswarn(config) )
  return;

 if ( config->warnings.sysio == ROAR_WARNING_ALWAYS ) {
  if ( newfunc == NULL ) {
   ROAR_WARN("%s(*): This function is obsolete. %s", func, info == NULL ? "" : info);
  } else {
   ROAR_WARN("%s(*): This function is obsolete. Please use %s(...). %s", func, newfunc, info == NULL ? "" : info);
  }
 }
}

void roar_debug_warn_obsolete_real(char * func, char * newfunc, char * info) {
 struct roar_libroar_config * config = roar_libroar_get_config();

 if ( !roar_libroar_iswarn(config) )
  return;

 if ( config->warnings.obsolete == ROAR_WARNING_ALWAYS ) {
  if ( newfunc == NULL ) {
   ROAR_WARN("%s(*): This function is obsolete. %s", func, info == NULL ? "" : info);
  } else {
   ROAR_WARN("%s(*): This function is obsolete. Please use %s(...). %s", func, newfunc, info == NULL ? "" : info);
  }
 }
}

void   roar_debug_set_stderr_fh(int fh) {
 roar_debug_stderr_fh = fh;
}

void   roar_debug_set_stderr_vio(struct roar_vio_calls * vio) {
 roar_debug_stderr_vio = vio;
}

void   roar_debug_set_stderr_mode(int mode) {
 roar_debug_stderr_mode = mode;
}

struct roar_vio_calls * roar_debug_get_stderr(void) {
 static struct roar_vio_calls STDERR;

 switch (roar_debug_stderr_mode) {
  case ROAR_DEBUG_MODE_SYSIO:
    if ( roar_debug_stderr_fh == -1 )
     return NULL;

    roar_vio_open_fh(&STDERR, roar_debug_stderr_fh);

    return &STDERR;
   break;
  case ROAR_DEBUG_MODE_VIO:
    return roar_debug_stderr_vio;
   break;
  default:
    return NULL;
   break;
 }
}

void roar_debug_msg_simple(const char *format, ...) {
 struct roar_vio_calls * vio;
 va_list ap;
 int ret;
 char buf[8192];
 size_t len;

 vio = roar_debug_get_stderr();

 va_start(ap, format);
 ret = vsnprintf(buf, 8192, format, ap);
 va_end(ap);

 if ( vio != NULL ) {
  roar_vio_write(vio, buf, ret);
 } else {
  switch (roar_debug_stderr_mode) {
#ifdef ROAR_HAVE_SYSLOG
   case ROAR_DEBUG_MODE_SYSLOG:
     // strip \n if needed for syslog:
     len = strlen(buf);
     if ( buf[len-1] == '\n' )
      buf[len-1] = 0;

     syslog(LOG_ERR, "%s", buf); // bad to use some defaults
    break;
#endif
   default:
     return;
    break;
  }
 }
}

void roar_debug_msg(int type, unsigned long int line, char * file, char * prefix, char * format, ...) {
 struct roar_vio_calls * vio;
 va_list ap;
 char    buf[8192];
 char  * bufp = buf;
 char  * typename;
 int     ret;
 int     priority;
 size_t  len;

 switch (type) {
  case ROAR_DEBUG_TYPE_ERROR:   typename = "Error";   break;
  case ROAR_DEBUG_TYPE_WARNING: typename = "Warning"; break;
  case ROAR_DEBUG_TYPE_INFO:    typename = "Info";    break;
  case ROAR_DEBUG_TYPE_DEBUG:   typename = "Debug";   break;
  default: typename = "Unknown Type"; break;
 }

 ret = snprintf(buf, sizeof(buf), "(%s: %s:%lu): %s: ", prefix, file, line, typename);

 if ( ret > 0 && ret < sizeof(buf) ) {
  bufp += ret;
 } else {
  ret = 0;
 }

 len = ret;

 va_start(ap, format);
 ret = vsnprintf(bufp, sizeof(buf)-ret, format, ap);
 va_end(ap);

 len += ret;

 switch (roar_debug_stderr_mode) {
  case ROAR_DEBUG_MODE_SYSIO:
  case ROAR_DEBUG_MODE_VIO:
    if ( (vio = roar_debug_get_stderr()) == NULL )
     return;
    roar_vio_write(vio, buf, len);
    roar_vio_write(vio, "\n", 1);
   break;
#ifdef ROAR_HAVE_SYSLOG
  case ROAR_DEBUG_MODE_SYSLOG:
    switch (type) {
     case ROAR_DEBUG_TYPE_ERROR:   priority = LOG_ERR;     break;
     case ROAR_DEBUG_TYPE_WARNING: priority = LOG_WARNING; break;
     case ROAR_DEBUG_TYPE_INFO:    priority = LOG_INFO;    break;
     case ROAR_DEBUG_TYPE_DEBUG:   priority = LOG_DEBUG;   break;
     default: priority = LOG_ERR; break;
    }
    syslog(priority, "%s", buf);
   break;
#endif
 }
}
//ll
