//sshaskpass.c:

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

int roar_sshaskpass_getpass   (char ** pw, char * desc) {
#if defined(ROAR_HAVE_BIN_SSH_ASKPASS) && defined(ROAR_SUPPORT_PASSWORD_API) && defined(ROAR_HAVE_POPEN) && defined(ROAR_HAVE_PCLOSE)
 FILE * cpipe;
 char   buf[1024];
 int    pos;
 int    i;

 if ( pw == NULL )
  return -1;

 if ( (cpipe = popen(ROAR_HAVE_BIN_SSH_ASKPASS, "r")) == NULL ) {
  return -1;
 }

 if ( fgets(buf, 1024, cpipe) == NULL ) {
  fclose(cpipe);
  return -1;
 }

 pclose(cpipe);

 pos = strlen(buf);

 ROAR_DBG("roar_sshaskpass_getpass(pw=%p, desc='%s'): pos=%i", pw, desc, pos);

 for (i = pos - 1; i >= 0 && (buf[i] == '\r' || buf[i] == '\n'); i--, pos--) {
  buf[i] = 0;
 }

 ROAR_DBG("roar_sshaskpass_getpass(pw=%p, desc='%s'): pos=%i", pw, desc, pos);

 if ( pos == 0 )
  return -1;

 *pw = roar_mm_strdup(buf);

 return 0;
#else
 return -1;
#endif
}

//ll
