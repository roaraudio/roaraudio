//ao_roar.c:
/*
 * ao_roar - RoarAudio audio output driver for MPlayer
 *
 * copyright (c) 2002 Juergen Keil <jk@tools.de>
 * copyright (c) 2008-2010 Philipp 'ph3-der-loewe' Schafft
 *
 * This file is part of RoarAudio, based on a file from MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <time.h>
#ifdef	__svr4__
#include <stropts.h>
#endif
#include <roaraudio.h>

#include "config.h"
#include "audio_out.h"
#include "audio_out_internal.h"
#include "libaf/af_format.h"
#include "mp_msg.h"
#include "help_mp.h"


#undef	ESD_DEBUG

#if	ESD_DEBUG
#define	dprintf(...)	printf(__VA_ARGS__)
#else
#define	dprintf(...)	/**/
#endif


#define	ROAR_CLIENT_NAME	"MPlayer"

static struct roar_connection  roar_con;
static struct roar_stream      roar_stream;
static struct roar_stream_info roar_info;
static int                     roar_fh;

static ao_info_t info =
{
    "RoarAudio audio output",
    "roar",
    "Philipp 'ph3-der-loewe' Schafft <lion@lion.leolix.org>",
    ""
};

LIBAO_EXTERN(roar)

/*
 * to set/get/query special features/parameters
 */
static int control(int cmd, void *arg)
{
    switch (cmd) {
    case AOCONTROL_GET_VOLUME:
    case AOCONTROL_SET_VOLUME:
      return CONTROL_OK;
    default:
      return CONTROL_UNKNOWN;
    }
}


/*
 * open & setup audio device
 * return: 1=success 0=fail
 */
static int init(int rate_hz, int channels, int format, int flags) {
 int codec = ROAR_CODEC_DEFAULT;
 int bits  = 16;

 if ( roar_simple_connect(&roar_con, NULL, ROAR_CLIENT_NAME) == -1 )
  return 0;

 switch (format) {
  case AF_FORMAT_S8:
   bits = 8;
   break;
  case AF_FORMAT_U8:
   bits  = 8;
   codec = ROAR_CODEC_PCM_U_LE;
   break;
 }

 if ( (roar_fh = roar_simple_new_stream_obj(&roar_con, &roar_stream, rate_hz, channels, bits, codec, ROAR_DIR_PLAY)) == -1 )
  return 0;

 if ( roar_stream_get_info(&roar_con, &roar_stream, &roar_info) == -1 ) {
  close(roar_fh);
  return 0;
 }

 roar_socket_nonblock(roar_fh, ROAR_SOCKET_NONBLOCK);

 return 1;
}


/*
 * close audio device
 */
static void uninit(int immed) {
 if ( roar_fh != -1 )
  close(roar_fh);

 roar_fh = -1;

 roar_disconnect(&roar_con);
}


/*
 * plays 'len' bytes of 'data'
 * it should round it down to outburst*n
 * return: number of bytes played
 */
static int play(void* data, int len, int flags) {
 int pkg  = 441*2*2;
 int ret  = 0;
 int r;

 while (len > 0) {
  if ( (r = write(roar_fh, data, pkg < len ? pkg : len)) > 0 ) {
   len  -= r;
   data += r;
   ret  += r;
  } else {
   break;
  }
 }

// ROAR_WARN("play(*) = %i", ret);

 return ret;
}


/*
 * stop playing, keep buffers (for pause)
 */
static void audio_pause(void)
{
    /*
     * not possible with esd.  the esd daemom will continue playing
     * buffered data (not more than ESD_MAX_DELAY seconds of samples)
     */
}


/*
 * resume playing, after audio_pause()
 */
static void audio_resume(void)
{
    /*
     * not possible with esd.
     *
     * Let's hope the pause was long enough that the esd ran out of
     * buffered data;  we restart our time based delay computation
     * for an audio resume.
     */
}


/*
 * stop playing and empty buffers (for seeking/pause)
 */
static void reset(void) {
}


/*
 * return: how many bytes can be played without blocking
 */
static int get_space(void) {
 fd_set sl;
 struct timeval tv;
 int r;

 FD_ZERO(&sl);
 FD_SET(roar_fh, &sl);

 tv.tv_sec  = 0;
 tv.tv_usec = 1;

 if ((r = select(roar_fh + 1, NULL, &sl, NULL, &tv)) > 0) {
  return roar_info.block_size;
 }
 return 0;
}


/*
 * return: delay in seconds between first and last sample in buffer
 */
static float get_delay(void) {
 return 0.02;
}
