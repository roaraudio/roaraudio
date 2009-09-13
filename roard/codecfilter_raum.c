//codecfilter_raum.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *
 *  This file is part of roard a part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_HAVE_LIBRAUM
#include <raum.h>

int cf_raum_pcb_open  (struct cont_fw_parent_inst * self,
                       int                          codec,
                       struct roar_stream_server  * stream,
                       struct roar_codecfilter    * filter) {
 struct roar_stream * s = ROAR_STREAM(stream);
 RAUMState * state;
 int flags = 0;

 if ( stream == NULL )
  return -1;

 switch (s->dir) {
  case ROAR_DIR_COMPLEX_IN:
  case ROAR_DIR_PLAY:
    flags = O_RDONLY;
   break;
  case ROAR_DIR_COMPLEX_OUT:
  case ROAR_DIR_MONITOR:
  case ROAR_DIR_OUTPUT:
    flags = O_WRONLY;
   break;
  default:
    return -1;
 }

 state = RAUMOpenVIO(&(stream->vio), flags);

 if ( state == NULL )
  return -1;

 cont_fw_set_uinst(self, state);

 return 0;
}

int cf_raum_pcb_close (struct cont_fw_parent_inst * self) {
 void * state;

 if ( cont_fw_get_uinst(self, &state) == -1 )
  return -1;

 return RAUMClose(state);
}

CONT_FW_SETUP_TYPE(cf_raum_setup) {
 self->pcb.open  = cf_raum_pcb_open;
 self->pcb.close = cf_raum_pcb_close;
 return 0;
}

#endif

//ll
