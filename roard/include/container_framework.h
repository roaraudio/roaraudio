//container_framework.h:

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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 */

#ifndef _CONTAINER_FRAMEWORK_H_
#define _CONTAINER_FRAMEWORK_H_

#include <roaraudio.h>

#define CONT_FW_MAX_CHILDS 8

#define CONT_FW_SETUP_TYPE(f) int (f)(struct cont_fw_parent_inst * self, int codec, struct roar_codecfilter    * filter)

struct cont_fw_parent_inst;

struct cont_fw_child_vio_inst {
 int child;
 void * u_inst;
 struct cont_fw_parent_inst * parent;
};

struct cont_fw_parent_inst {
 struct cont_fw_child_vio_inst * child[CONT_FW_MAX_CHILDS];
 void * u_inst;
 int state;
 struct roar_vio_calls vio;
 struct {
  int id;
  int codec;
  struct roar_stream_server  * stream;
  struct roar_codecfilter    * filter;
 } stream;
 struct {
  ssize_t (*read) (struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child, void *buf, size_t len);
  ssize_t (*write)(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child, void *buf, size_t len);
  int     (*flush)(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child);
  int     (*close)(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child);
 } ccb;
 struct {
  int     (*open     )(struct cont_fw_parent_inst * self,
                       int                          codec,
                       struct roar_stream_server  * stream,
                       struct roar_codecfilter    * filter);
  int     (*close    )(struct cont_fw_parent_inst * self);
  int     (*flush    )(struct cont_fw_parent_inst * self);
  int     (*new_child)(struct cont_fw_parent_inst * self, struct cont_fw_child_vio_inst * child);
 } pcb;
};

// Parent:
int     cont_fw_new      (struct cont_fw_parent_inst ** inst);
int     cont_fw_delete   (struct cont_fw_parent_inst  * inst);
int     cont_fw_set_uinst(struct cont_fw_parent_inst  * inst, void  * u_inst);
int     cont_fw_get_uinst(struct cont_fw_parent_inst  * inst, void ** u_inst);

// Child:
int     cont_fw_new_child(struct cont_fw_parent_inst  * inst, int id);
int     cont_fw_init_vio(struct roar_vio_calls * vio, void * inst);

// VIO Childs:
ssize_t cont_fw_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t cont_fw_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   cont_fw_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     cont_fw_nonblock(struct roar_vio_calls * vio, int state);
int     cont_fw_sync    (struct roar_vio_calls * vio);
int     cont_fw_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     cont_fw_close   (struct roar_vio_calls * vio);

// VIO Parent:
int     cont_pvio_open    (struct roar_vio_calls * vio, void * inst);
ssize_t cont_pvio_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t cont_pvio_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   cont_pvio_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     cont_pvio_nonblock(struct roar_vio_calls * vio, int state);
int     cont_pvio_sync    (struct roar_vio_calls * vio);
int     cont_pvio_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     cont_pvio_close   (struct roar_vio_calls * vio);

// CF:
int cont_fw_cf_open(CODECFILTER_USERDATA_T * inst, int codec,
                                             struct roar_stream_server * info,
                                             struct roar_codecfilter   * filter);
int cont_fw_cf_close(CODECFILTER_USERDATA_T   inst);
int cont_fw_cf_pause(CODECFILTER_USERDATA_T   inst, int newstate);
int cont_fw_cf_write(CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cont_fw_cf_read (CODECFILTER_USERDATA_T   inst, char * buf, int len);
int cont_fw_cf_flush(CODECFILTER_USERDATA_T   inst);
int cont_fw_cf_delay(CODECFILTER_USERDATA_T   inst, uint_least32_t * delay);
int cont_fw_cf_ctl  (CODECFILTER_USERDATA_T   inst, int cmd, void * data);

#endif

//ll
