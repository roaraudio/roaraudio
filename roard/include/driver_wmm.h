//driver_wmm.h:

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

#ifndef _DRIVER_WMM_H_
#define _DRIVER_WMM_H_

#include <roaraudio.h>

struct driver_wmm {
  UINT  id;             /* device id                       */
  HWAVEOUT hwo;         /* waveout handler                 */
  WAVEOUTCAPS caps;     /* device caps                     */
  WAVEFORMATEX wavefmt; /* sample format                   */

  int opened;           /* device has been opened          */
  int prepared;         /* waveheaders have been prepared  */
  int blocks;           /* number of blocks (wave headers) */
  int splPerBlock;      /* sample per blocks.              */
  int msPerBlock;       /* millisecond per block (approx.) */

  void * bigbuffer;     /* Allocated buffer for waveheaders and sound data */
  struct {
   WAVEHDR wh;          /* waveheader                        */
   char *  data;        /* sample data ptr                   */
   int     idx;         /* index of this header              */
   int     count;       /* current byte count                */
   int     length;      /* size of data                      */
   int     sent;        /* set when header is sent to device */
  } * wh;          /* Pointer to waveheaders in bigbuffer             */
  BYTE * spl;           /* Pointer to sound data in bigbuffer              */

  int sent_blocks;      /* Number of waveheader sent (not ack).        */
  int full_blocks;      /* Number of waveheader full (ready to send).  */
  int widx;             /* Index to the block being currently filled.  */
  int ridx;             /* Index to the block being sent.              */
};

int     driver_wmm_open_vio(struct roar_vio_calls * inst, char * device, struct roar_audio_info * info, int fh, struct roar_stream_server * sstream);
int     driver_wmm_close_vio(struct roar_vio_calls * vio);
ssize_t driver_wmm_write(struct roar_vio_calls * vio, void *buf, size_t count);

#endif

//ll
