//vio_ctl.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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

#ifndef _LIBROARVIO_CTL_H_
#define _LIBROARVIO_CTL_H_

#include "libroar.h"

// CTLs:
/*
 * 0xAAAABBBB
 * AAAA:
 *   0x0001 -> Basic stream calls
 *   0x0002 -> Driver calls
 *
 * BBBB:
 *   0x0XXX -> Client
 *   0x1XXX -> Server
 */

#define ROAR_VIO_CTL_GET                  0x1
#define ROAR_VIO_CTL_SET                  0x2

#define ROAR_VIO_CTL_CLIENT            0x0000
#define ROAR_VIO_CTL_SERVER            0x1000

#define ROAR_VIO_CTL_GENERIC     (0x0000<<16)
#define ROAR_VIO_CTL_STREAM      (0x0001<<16)
#define ROAR_VIO_CTL_DRIVER      (0x0002<<16)

// basic calls:
#define ROAR_VIO_CTL_GET_NEXT          (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_GET|0x0100)
#define ROAR_VIO_CTL_SET_NEXT          (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_SET|0x0100)
#define ROAR_VIO_CTL_GET_FH            (ROAR_VIO_CTL_GENERIC|0x0110)
#define ROAR_VIO_CTL_GET_READ_FH       (ROAR_VIO_CTL_GENERIC|0x0111)
#define ROAR_VIO_CTL_GET_WRITE_FH      (ROAR_VIO_CTL_GENERIC|0x0112)
#define ROAR_VIO_CTL_SELECT            (ROAR_VIO_CTL_GENERIC|0x0120)
#define ROAR_VIO_CTL_GET_UMMAP         (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_GET|0x0130) /* Use mmap(), int as bool */
#define ROAR_VIO_CTL_SET_UMMAP         (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_SET|0x0130) /* ** */
#define ROAR_VIO_CTL_GET_SHUTDOWN      (ROAR_VIO_CTL_GENERIC|0x0140) /* shutdown(), need specs */
#define ROAR_VIO_CTL_SET_NOSYNC        (ROAR_VIO_CTL_GENERIC|0x0150) /* delete call of vio sync() from object */
#define ROAR_VIO_CTL_GET_NAME          (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_GET|0x0160) /* get name of VIO */

// get or set data format used for read and write calls, see below
#define ROAR_VIO_CTL_GET_DATA_FORMAT   (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_GET|0x0170)
#define ROAR_VIO_CTL_SET_DATA_FORMAT   (ROAR_VIO_CTL_GENERIC|ROAR_VIO_CTL_SET|0x0170)

// stream:
#define ROAR_VIO_CTL_SET_STREAM    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_SET) /* normal streams */
#define ROAR_VIO_CTL_GET_STREAM    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_GET)
#define ROAR_VIO_CTL_SET_DMXSCHAN  (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_SET|0x10) /* simple DMX Channel */
#define ROAR_VIO_CTL_GET_DMXSCHAN  (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_GET|0x10)
#define ROAR_VIO_CTL_SET_DMXUNIV   (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_SET|0x20) /* DMX Universe */
#define ROAR_VIO_CTL_GET_DMXUNIV   (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_CLIENT|ROAR_VIO_CTL_GET|0x20)

#define ROAR_VIO_CTL_SET_SSTREAM   (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_SET) /* server streams */
#define ROAR_VIO_CTL_GET_SSTREAM   (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_GET)
#define ROAR_VIO_CTL_SET_SSTREAMID (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_SET|0x10) /* server streams */
#define ROAR_VIO_CTL_GET_SSTREAMID (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SERVER|ROAR_VIO_CTL_GET|0x10)

#define ROAR_VIO_CTL_SET_AUINFO    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_SET|0x2000) /* set a struct roar_audio_info */
#define ROAR_VIO_CTL_GET_AUINFO    (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_GET|0x2000) /* get a struct roar_audio_info */

#define ROAR_VIO_CTL_GET_DELAY     (ROAR_VIO_CTL_STREAM|ROAR_VIO_CTL_GET|0x010) /* return in bytes as the vio interface */
                                              /* does not know anything about streams */

#define ROAR_VIO_CTL_GET_DBLOCKS   (ROAR_VIO_CTL_DRIVER|0x0001) /* get Driver Blocks */
#define ROAR_VIO_CTL_SET_DBLOCKS   (ROAR_VIO_CTL_DRIVER|0x0002) /* set Driver Blocks */
#define ROAR_VIO_CTL_GET_DBLKSIZE  (ROAR_VIO_CTL_DRIVER|0x0003) /* get Driver Blocks size (in byte) */
#define ROAR_VIO_CTL_SET_DBLKSIZE  (ROAR_VIO_CTL_DRIVER|0x0004) /* set Driver Blocks size (in byte) */
#define ROAR_VIO_CTL_GET_VOLUME    (ROAR_VIO_CTL_DRIVER|ROAR_VIO_CTL_GET|0x10)
#define ROAR_VIO_CTL_SET_VOLUME    (ROAR_VIO_CTL_DRIVER|ROAR_VIO_CTL_SET|0x10)


// Data format used for read/write():

// _D_ata _F_ormat _T_ypes:
// generic types:
#define ROAR_VIO_DFT_UNKNOWN           -1
#define ROAR_VIO_DFT_NULL          0x0000
#define ROAR_VIO_DFT_RAW           0x0001 /* raw bytes, default */

// RoarAudio types:
#define ROAR_VIO_DFT_RA_MESSAGE    0x0101
#define ROAR_VIO_DFT_RA_BUFFER     0x0102

// extern types:
#define ROAR_VIO_DFT_OGG_PAGE      0x0201
#define ROAR_VIO_DFT_OGG_PACKET    0x0202

struct roar_vio_dataformat {
 unsigned int type;
};

#endif

//ll
