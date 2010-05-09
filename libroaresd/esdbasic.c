//esdbasic.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008-2010
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
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE for everyone want's to change something and send patches:
 *  read README and HACKING! There a addition information on
 *  the license of this document you need to read before you send
 *  any patches.
 *
 *  NOTE for uses of non-GPL (LGPL,...) software using libesd, libartsc
 *  or libpulse*:
 *  The libs libroaresd, libroararts and libroarpulse link this libroar
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#include "libroaresd.h"

/* opens channel, authenticates connection, and prefares for protos */
/* returns EsounD socket for communication, result < 0 = error */
/* server = listen socket (localhost:5001, 192.168.168.0:9999 */
/* rate, format = (bits | channels | stream | func) */
int esd_open_sound( const char *host ) {
 struct roar_connection con;
 int fh;

 if ( host == NULL )
  host = getenv("ESPEAKER");

 if ( roar_simple_connect(&con, (char*) host, NULL) == -1 ) {
  ROAR_DBG("esd_open_sound(*): roar_simple_connect() faild!");
  return -1;
 }

 if ( (fh = roar_get_connection_fh(&con)) == -1 ) {
  roar_disconnect(&con);
  return -1;
 }

 return fh;
}

/* send the authorization cookie, create one if needed */
int esd_send_auth( int sock ) {
 return 0;
}

/* closes fd, previously obtained by esd_open */
int esd_close( int esd ) {
 return roar_simple_close(esd);
}

/* get the stream latency to esound (latency is number of samples  */
/* at 44.1khz stereo 16 bit - you'll have to adjust if oyur input  */
/* sampling rate is less (in bytes per second)                     */
/* so if you're at 44.1khz stereo 16bit in your stream - your lag  */
/* in bytes woudl be lag * 2 * 2 bytes (2 for stereo, 2 for 16bit) */
/* if your stream is at 22.05 Khz it'll be double this - in mono   */
/* double again ... etc.                                           */
int esd_get_latency(int esd) {
#ifdef ROAR_HAVE_GETTIMEOFDAY
 struct timeval         try, ans;
 struct roar_message    m;
 struct roar_connection con;

 m.cmd = ROAR_CMD_NOOP;
 m.datalen = 0;

 roar_connect_fh(&con, esd);

 gettimeofday(&try, NULL);
 roar_req(&con, &m, NULL);
 gettimeofday(&ans, NULL);

 while (ans.tv_sec > try.tv_sec) {
  ans.tv_sec--;
  ans.tv_usec += 1000000;
 }
 ans.tv_usec -= try.tv_usec;

/*
 1   -> 44100
 0.5 -> 22050
*/

 return 441*ans.tv_usec/10000;
#else
 // don't know...
 return 0;
#endif
}


//ll
