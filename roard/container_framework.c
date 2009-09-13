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

// Parent:
int     cont_fw_new     (struct cont_fw_parent_inst ** inst) {
 struct cont_fw_parent_inst * self;

 if ( inst == NULL )
  return -1;

 if ( (self = malloc(sizeof(struct cont_fw_parent_inst))) == NULL ) {
  *inst = NULL;
  return -1;
 }

 memset(self, 0, sizeof(struct cont_fw_parent_inst));

 *inst = self;
 return 0;
}

int     cont_fw_delete  (struct cont_fw_parent_inst  * inst) {
 int i;

 if ( inst == NULL )
  return -1;

 // check if there are streams to close...
 for (i = 0; i < CONT_FW_MAX_CHILDS; i++) {
  if ( inst->child[i] != NULL ) {
   return -1;
  }
 }

 if ( inst->pcb.close != NULL )
  inst->pcb.close(inst);

 free(inst);

 return 0;
}

int     cont_fw_set_uinst(struct cont_fw_parent_inst  * inst, void  * u_inst) {
 if ( inst == NULL )
  return -1;

 inst->u_inst = u_inst;

 return 0;
}

int     cont_fw_get_uinst(struct cont_fw_parent_inst  * inst, void ** u_inst) {
 if ( inst == NULL || u_inst == NULL )
  return -1;

 *u_inst = inst->u_inst;

 return 0;
}

// VIOs:
int     cont_fw_new_child(struct cont_fw_parent_inst  * inst, int id) {
 struct cont_fw_child_vio_inst * self;
 struct roar_stream_server     * ss;
 int i;
 int cid = -1;

 if ( inst == NULL || id == -1 )
  return -1;

 if ( streams_get(id, &ss) == -1 )
  return -1;

 for (i = 0; i < CONT_FW_MAX_CHILDS; i++) {
  if ( inst->child[i] == NULL ) {
   cid = i;
   break;
  }
 }

 if ( cid == -1 )
  return -1;

 if ( (self = malloc(sizeof(struct cont_fw_child_vio_inst))) == NULL )
  return -1;

 memset(self, 0, sizeof(struct cont_fw_child_vio_inst));

 self->parent = inst;
 self->child  = id;
 self->u_inst = NULL;

 inst->child[i] = self;

 if ( inst->pcb.new_child != NULL ) {
  if ( inst->pcb.new_child(inst, self) == -1 ) {
   inst->child[i] = NULL;
   free(self);
   return -1;
  }
 }

 // no error possible here.
 cont_fw_init_vio(&(ss->vio), self);

 streams_set_fh(id, -1); // update some internal structures

 return 0;
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

 if ( inst->parent->ccb.read != NULL )
  return inst->parent->ccb.read(inst->parent, inst, buf, count);

 return -1;
}

ssize_t cont_fw_write   (struct roar_vio_calls * vio, void *buf, size_t count) {
 _BASIC();

 if ( inst->parent->ccb.write != NULL )
  return inst->parent->ccb.write(inst->parent, inst, buf, count);

 return -1;
}

off_t   cont_fw_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     cont_fw_nonblock(struct roar_vio_calls * vio, int state);

int     cont_fw_sync    (struct roar_vio_calls * vio) {
 _BASIC();

 if ( inst->parent->ccb.flush != NULL )
  return inst->parent->ccb.flush(inst->parent, inst);

 return 0;
}

int     cont_fw_close   (struct roar_vio_calls * vio) {
 _DECL();
 int r = 0;
 int i;

 _PREP();

 if ( cont_fw_sync(vio) == -1 )
  r = -1;

 if ( inst->parent->ccb.close != NULL )
  return inst->parent->ccb.close(inst->parent, inst);

 for (i = 0; i < CONT_FW_MAX_CHILDS; i++) {
  if ( inst->parent->child[i] == inst ) {
   inst->parent->child[i] = NULL;
  }
 }

 free(inst);

 return  r;
}

int     cont_fw_ctl     (struct roar_vio_calls * vio, int cmd, void * data);

// CF:
int cont_fw_cf_open(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter) {
 struct cont_fw_parent_inst * self;
 CONT_FW_SETUP_TYPE((*setup));

 ROAR_DBG("cont_fw_cf_open(*) = ?");

 if ( cont_fw_new(&self) == -1 )
  return -1;

 ROAR_DBG("cont_fw_cf_open(*) = ?");

 if ( (setup = filter->setup) != NULL ) {
  if ( setup(self, codec, filter) == -1 ) {
   cont_fw_delete(self);
   return -1;
  }
 }

 ROAR_DBG("cont_fw_cf_open(*) = ?");

 if ( self->pcb.open != NULL ) {
  if ( self->pcb.open(self, codec, info, filter) == -1 ) {
   cont_fw_delete(self);
   return -1;
  }
 }

 ROAR_DBG("cont_fw_cf_open(*) = ?");

 *inst = self;

 ROAR_DBG("cont_fw_cf_open(*) = 0");
 return 0;
}

int cont_fw_cf_close(CODECFILTER_USERDATA_T   inst) {
 return cont_fw_delete(inst);
}

int cont_fw_cf_pause(CODECFILTER_USERDATA_T   inst, int newstate);


// no direct read or writing...
int cont_fw_cf_write(CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 return -1;
}

int cont_fw_cf_read (CODECFILTER_USERDATA_T   inst, char * buf, int len) {
 return -1;
}

int cont_fw_cf_flush(CODECFILTER_USERDATA_T   inst) {
 struct cont_fw_parent_inst * self = (void*)inst;

 if ( self->pcb.flush != NULL )
  return self->pcb.flush(self);

 return 0;
}

int cont_fw_cf_delay(CODECFILTER_USERDATA_T   inst, uint_least32_t * delay);

int cont_fw_cf_ctl  (CODECFILTER_USERDATA_T   inst, int cmd, void * data) {
 struct cont_fw_parent_inst * self = (void*)inst;
 int_least32_t type = cmd & ROAR_STREAM_CTL_TYPEMASK;

 cmd -= type;

 ROAR_DBG("cont_fw_cf_ctl(*): command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));

 if ( data == NULL && type != ROAR_STREAM_CTL_TYPE_VOID )
  return -1;

 switch (cmd) {
  case ROAR_CODECFILTER_CTL2CMD(ROAR_CODECFILTER_CTL_VIRTUAL_DELETE):
    return 0;
   break;
  case ROAR_CODECFILTER_CTL2CMD(ROAR_CODECFILTER_CTL_VIRTUAL_NEW):
    if ( type != ROAR_STREAM_CTL_TYPE_INT )
     return -1;

    return cont_fw_new_child(self, *(int*)data);
   break;
  default:
    ROAR_DBG("cont_fw_cf_ctl(*): Unknown command: cmd=0x%.8x, type=0x%.8x, pcmd=0x%.8x",
                    cmd, type, ROAR_CODECFILTER_CTL2CMD(cmd));
    return -1;
 }

 return -1;
}

//ll
