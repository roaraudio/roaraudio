//cdrom.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

int roar_cdrom_open (struct roar_connection * con, struct roar_cdrom * cdrom, char * device) {
 if ( cdrom == NULL )
  return -1;

 memset((void*)cdrom, 0, sizeof(struct roar_cdrom));

 if ( device == NULL )
  device = roar_cdromdevice();

 if ( device == NULL )
  return -1;

 strncpy(cdrom->device, device, ROAR_CDROM_MAX_DEVLEN);

 cdrom->con    = con; // we do not care here if it is set or not as we can operate in local only mode

 cdrom->stream = -1;
 cdrom->fh     = -1;

 return 0;
}

int roar_cdrom_close(struct roar_cdrom * cdrom) {
 if ( cdrom == NULL )
  return -1;

 if ( cdrom->fh != -1 )
  close(cdrom->fh);

 memset((void*)cdrom, 0, sizeof(struct roar_cdrom));

 return 0;
}

int roar_cdrom_stop (struct roar_cdrom * cdrom);
int roar_cdrom_play (struct roar_cdrom * cdrom, int track);

//ll
