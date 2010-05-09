//codecfilter_raum.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#include "roard.h"

#ifdef ROAR_HAVE_LIBRAUM
#include <raum.h>

// PCBs:
int cf_raum_pcb_open  (struct cont_fw_parent_inst * self,
                       int                          codec,
                       struct roar_stream_server  * stream,
                       struct roar_codecfilter    * filter) {
 struct roar_stream * s = ROAR_STREAM(stream);
 RAUMState * state;
 int flags = 0;

 ROAR_DBG("cf_raum_pcb_open(*) = ?");

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

 state = RAUMOpenVIO(&(self->vio), flags);

 if ( state == NULL )
  return -1;

 cont_fw_set_uinst(self, state);

 ROAR_DBG("cf_raum_pcb_open(*) = 0");
 return 0;
}

int cf_raum_pcb_close (struct cont_fw_parent_inst * self) {
 void * state;

 ROAR_DBG("cf_raum_pcb_close(*) = ?");

 if ( cont_fw_get_uinst(self, &state) == -1 )
  return -1;

 ROAR_DBG("cf_raum_pcb_close(*) = ?");
 return RAUMClose(state);
}

int cf_raum_pcb_new_child(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child) {
 struct roar_stream_server * ss;
 struct roar_stream        *  s;
 void                      * ps;
 RAUMStream * stream;
 int dir = ROAR_DIR_PLAY;

 ROAR_DBG("cf_raum_pcb_new_child(self=%p, child=%p) = ?", self, child);

 if ( streams_get(child->child, &ss) == -1 )
  return -1;

 if ( (s = ROAR_STREAM(ss)) == NULL )
  return -1;

 if ( cont_fw_get_uinst(self, &ps) == -1 )
  return -1;

 if ( ps == NULL )
  return -1;

 if ( (stream = RAUMStreamNewSimple(s->info.rate, s->info.channels, s->info.bits, s->info.codec, dir)) == NULL )
  return -1;

 if ( RAUMAddStream(ps, stream) == -1 ) {
  RAUMStreamClose(stream);
  return -1;
 }

 child->u_inst = stream;

 ROAR_DBG("cf_raum_pcb_new_child(self=%p, child=%p) = 0", self, child);
 return 0;
}

// CCBs:
ssize_t cf_raum_ccb_read (struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child, void *buf, size_t len) {
 ROAR_DBG("cf_raum_ccb_read(self=%p, child=%p, buf=%p, len=%lu) = ?", self, child, buf, (unsigned long)len);
 return RAUMStreamRead(child->u_inst, buf, len);
}
ssize_t cf_raum_ccb_write(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child, void *buf, size_t len) {
 ROAR_DBG("cf_raum_ccb_write(self=%p, child=%p, buf=%p, len=%lu) = ?", self, child, buf, (unsigned long)len);
 return RAUMStreamWrite(child->u_inst, buf, len);
}
int     cf_raum_ccb_close(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child) {
 ROAR_DBG("cf_raum_ccb_close(self=%p, child=%p) = ?", self, child);
 return RAUMStreamClose(child->u_inst);
}

// SETUP:
CONT_FW_SETUP_TYPE(cf_raum_setup) {
 ROAR_DBG("cf_raum_setup(*) = ?");

 // PCBs:
 self->pcb.open      = cf_raum_pcb_open;
 self->pcb.close     = cf_raum_pcb_close;
 self->pcb.new_child = cf_raum_pcb_new_child;

 // CCBs:
 self->ccb.read      = cf_raum_ccb_read;
 self->ccb.write     = cf_raum_ccb_write;
 self->ccb.close     = cf_raum_ccb_close;

 ROAR_DBG("cf_raum_setup(*) = 0");
 return 0;
}

#endif

//ll
