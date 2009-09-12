//container_framework.c:

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

#define _DECL()  struct cont_fw_child_vio_inst * inst
#define _CHECK() if ( vio == NULL ) \
                 return -1
#define _CINST() if ( (inst = vio->inst) == NULL ) \
                  return -1
#define _PREP()  _CHECK(); \
                 _CINST()
#define _BASIC() _DECL();  \
                 _PREP()

int     cont_fw_new     (struct cont_fw_parent_inst ** inst) {
 return -1;
}

int     cont_fw_delete  (struct cont_fw_parent_inst  * inst) {
 return -1;
}

int     cont_fw_init_vio(struct roar_vio_calls * vio, void * inst) {
 if ( vio == NULL )
  return -1;

 memset(vio, 0, sizeof(struct roar_vio_calls));
 vio->inst = inst;

 vio->read  = cont_fw_read;
 vio->write = cont_fw_write;
 vio->sync  = cont_fw_sync;
 vio->close = cont_fw_close;

 return 0;
}

ssize_t cont_fw_read    (struct roar_vio_calls * vio, void *buf, size_t count) {
 _BASIC();

 if ( inst->parent->cb.read != NULL )
  return inst->parent->cb.read(inst->parent, inst->inst, buf, count);

 return -1;
}

ssize_t cont_fw_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 _BASIC();

 if ( inst->parent->cb.write != NULL )
  return inst->parent->cb.write(inst->parent, inst->inst, buf, count);

 return -1;
}

off_t   cont_fw_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     cont_fw_nonblock(struct roar_vio_calls * vio, int state);

int     cont_fw_sync    (struct roar_vio_calls * vio) {
 _BASIC();

 if ( inst->parent->cb.flush != NULL )
  return inst->parent->cb.flush(inst->parent, inst->inst);

 return 0;
}

int     cont_fw_close   (struct roar_vio_calls * vio) {
 _DECL();
 int r = 0;

 _PREP();

 if ( cont_fw_sync(vio) == -1 )
  r = -1;

 return -1;
 return  r;
}

int     cont_fw_ctl     (struct roar_vio_calls * vio, int cmd, void * data);

//ll
