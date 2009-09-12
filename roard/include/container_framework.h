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
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#ifndef _CONTAINER_FRAMEWORK_H_
#define _CONTAINER_FRAMEWORK_H_

#include <roaraudio.h>

#define CONT_FW_MAX_CHILDS 8

struct cont_fw_parent_inst;

struct cont_fw_child {
 int child;
 void * u_inst;
};

struct cont_fw_child_vio_inst {
 int child;
 struct cont_fw_child       * inst;
 struct cont_fw_parent_inst * parent;
};

struct cont_fw_parent_inst {
 struct cont_fw_child * child[CONT_FW_MAX_CHILDS];
 void * u_inst;
 struct {
  ssize_t (*read) (struct cont_fw_parent_inst * self, struct cont_fw_child * child, void *buf, size_t len);
  ssize_t (*write)(struct cont_fw_parent_inst * self, struct cont_fw_child * child, void *buf, size_t len);
  int     (*flush)(struct cont_fw_parent_inst * self, struct cont_fw_child * child);
 } cb;
};

ssize_t cont_fw_read    (struct roar_vio_calls * vio, void *buf, size_t count);
ssize_t cont_fw_write   (struct roar_vio_calls * vio, void *buf, size_t count);
off_t   cont_fw_lseek   (struct roar_vio_calls * vio, off_t offset, int whence);
int     cont_fw_nonblock(struct roar_vio_calls * vio, int state);
int     cont_fw_sync    (struct roar_vio_calls * vio);
int     cont_fw_ctl     (struct roar_vio_calls * vio, int cmd, void * data);
int     cont_fw_close   (struct roar_vio_calls * vio);

#endif

//ll
