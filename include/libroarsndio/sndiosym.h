//sndiosym.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2009
 *  The code (may) include prototypes and comments (and maybe
 *  other code fragements) from EsounD.
 *  They are mostly copyrighted by Eric B. Mitchell (aka 'Ricdude)
 *  <ericmit@ix.netcom.com>. For more information see AUTHORS.esd.
 *
 *  This file is part of libroaresd a part of RoarAudio,
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
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 */

#ifndef _LIBROARSNDIO_SNDIOSYM_H_
#define _LIBROARSNDIO_SNDIOSYM_H_

#include <roaraudio.h>

#ifdef ROAR_HAVE_H_POLL
#include <poll.h>
#else
struct pollfd;
#endif

#define SIO_PLAY        1
#define SIO_REC         2

#define SIO_IGNORE      0       /* pause during xrun */
#define SIO_SYNC        1       /* resync after xrun */
#define SIO_ERROR       2       /* terminate on xrun */

#if BYTE_ORDER == BIG_ENDIAN && !defined(ROAR_TARGET_WIN32)
#define SIO_LE_NATIVE   0
#else
#if BYTE_ORDER == LITTLE_ENDIAN
#define SIO_LE_NATIVE   1
#else
#error Byte Order of this system is not supported within the sndio interface.
#endif
#endif

#define SIO_BPS(bits) (((bits)/8) + ((bits) % 8 ? 1 : 0))

struct sio_par {
 unsigned bits;          /* bits per sample */
 unsigned bps;           /* bytes per sample */
 unsigned sig;           /* 1 = signed, 0 = unsigned */
 unsigned le;            /* 1 = LE, 0 = BE byte order */
 unsigned msb;           /* 1 = MSB, 0 = LSB aligned */
 unsigned rchan;         /* number channels for recording */
 unsigned pchan;         /* number channels for playback */
 unsigned rate;          /* frames per second */
 unsigned appbufsz;      /* minimum buffer size without xruns */
 unsigned bufsz;         /* end-to-end buffer size (read-only) */
 unsigned round;         /* optimal buffer size divisor */
 unsigned xrun;          /* what to do on overrun/underrun */
};

struct sio_cap;

struct sio_hdl {
 char           * device;
 int              fh;
 struct sio_par   para;
 void           (*on_move)(void * arg, int delta);
 void           * on_move_arg;
 void           (*on_vol )(void * arg, unsigned vol);
 void           * on_vol_arg;
};

struct sio_hdl * sio_open(char * name, unsigned mode, int nbio_flag);
void   sio_close  (struct sio_hdl * hdl);

void   sio_initpar(struct sio_par * par);
int    sio_setpar (struct sio_hdl * hdl, struct sio_par * par);
int    sio_getpar (struct sio_hdl * hdl, struct sio_par * par);

int    sio_getcap (struct sio_hdl * hdl, struct sio_cap * cap);

int    sio_start  (struct sio_hdl * hdl);
int    sio_stop   (struct sio_hdl * hdl);

size_t sio_read   (struct sio_hdl * hdl, void * addr, size_t nbytes);
size_t sio_write  (struct sio_hdl * hdl, void * addr, size_t nbytes);

void   sio_onmove (struct sio_hdl * hdl, void (*cb)(void * arg, int delta), void * arg);

int    sio_nfds   (struct sio_hdl * hdl);

int    sio_pollfd (struct sio_hdl * hdl, struct pollfd * pfd, int events);

int    sio_revents(struct sio_hdl * hdl, struct pollfd * pfd);

int    sio_eof    (struct sio_hdl * hdl);

int    sio_setvol (struct sio_hdl * hdl, unsigned vol);
void   sio_onvol  (struct sio_hdl * hdl, void (*cb)(void * arg, unsigned vol), void * arg);

#endif

//ll
