//porto.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
 *
 *  This file is part of RoarAudio,
 *  a cross-platform sound system for both, home and professional use.
 *  See README for details.
 *
 *  This file is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License version 3
 *  as published by the Free Software Foundation.
 *
 *  RoarAudio is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this software; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 51 Franklin Street, Fifth Floor,
 *  Boston, MA 02110-1301, USA.
 *
 *  NOTE: Even though this file is LGPLed it (may) include GPLed files
 *  so the license of this file is/may therefore downgraded to GPL.
 *  See HACKING for details.
 */

#ifndef _ROARAUDIO_PORTO_H_
#define _ROARAUDIO_PORTO_H_

#define ROAR_CMD_EOL           -1 /* end of list */
#define ROAR_CMD_NOOP           0 /* do nothing */
#define ROAR_CMD_IDENTIFY       1 /* after connect we have to identify ourself */
#define ROAR_CMD_AUTH           2 /* send an auth cookie */
#define ROAR_CMD_NEW_STREAM     3 /* request a new stream id */
#define ROAR_CMD_SET_META       4 /* update meta data of streamed data */
#define ROAR_CMD_EXEC_STREAM    5 /* close all data channels and make the controll socket a raw data socket */
#define ROAR_CMD_QUIT           6 /* quits the connection */

#define ROAR_CMD_GET_STANDBY    7
#define ROAR_CMD_SET_STANDBY    8

#define ROAR_CMD_SERVER_INFO    9 /* Server version ... */
#define ROAR_CMD_SERVER_STATS  10 /* Server stats: number of clients, streams,.. */
#define ROAR_CMD_SERVER_OINFO  11 /* Output info, roar_audio_info sa */

#define ROAR_CMD_ADD_DATA      12 /* add some data to the input buffer */

//#define ROAR_CMD_

#define ROAR_CMD_EXIT          13 /* make the server quit */
#define ROAR_CMD_LIST_STREAMS  14 /* list all streams (not only playback streams like esd does) */
#define ROAR_CMD_LIST_CLIENTS  15 /* list all clients */

#define ROAR_CMD_GET_CLIENT    16 /* get infos about a client */
#define ROAR_CMD_GET_STREAM    17 /* get infos about a stream */

#define ROAR_CMD_KICK          18 /* kick a client, stream, sample or source */

#define ROAR_CMD_SET_VOL       19 /* change volume */
#define ROAR_CMD_GET_VOL       20 /* get volume */

#define ROAR_CMD_CON_STREAM    21 /* let the server connect to something to get data for a new stream */

#define ROAR_CMD_GET_META      22 /* get meta data of streamed data */
#define ROAR_CMD_LIST_META     23 /* get list of meta data of streamed data */

#define ROAR_CMD_BEEP          24 /* send an beep */

#define ROAR_GET_ACL           25 /* get ACL, may have subtypes */
#define ROAR_SET_ACL           26 /* set ACL, may have subtypes */

#define ROAR_CMD_GET_STREAM_PARA   27 /* Get addition stream parameters like blocksizes */
#define ROAR_CMD_SET_STREAM_PARA   28 /* set specal stream parameter */

#define ROAR_CMD_ATTACH        29 /* attach a OUTPUT or source */
#define ROAR_CMD_DETACH        ROAR_CMD_KICK /* just to have an alias */

#define ROAR_CMD_PASSFH        30 /* send an fh via UNIX Domain Socket to the roard */

#define ROAR_CMD_GETTIMEOFDAY  31

#define ROAR_CMD_WHOAMI        32


// Reserved for RAUM project:
#define ROAR_CMD_RAUM_SEEKTABLE 201
// End of reserved block

#define ROAR_CMD_EPERM        252 /* error: permition denided */
#define ROAR_CMD_OK_STOP      253 /* used internaly in the serveer: */
                                  // ROAR_CMD_OK but do not queue the client again within the block
#define ROAR_CMD_OK           254 /* return value OK */
#define ROAR_CMD_ERROR        255 /* return value ERROR */


#define ROAR_STANDBY_ACTIVE     1
#define ROAR_STANDBY_INACTIVE   0

// object types...
#define ROAR_OT_CLIENT 1
#define ROAR_OT_STREAM 2
#define ROAR_OT_SOURCE 3
#define ROAR_OT_SAMPLE 4

// auth types...
#define ROAR_AUTH_NONE    0 /* do not auth */
#define ROAR_AUTH_TRUST   1 /* trust: used for UNIX Domain sockets, same uid = trusted, diffrent uid = public */
#define ROAR_AUTH_COOKIE  2 /* use cookie */
#define ROAR_AUTH_SYSUSER 3 /* use local/NIS/PAM userdatabase */
#define ROAR_AUTH_PASSWD  4 /* use password */

// attach types...
#define ROAR_ATTACH_SIMPLE 1 /* move a stream from one to another client (attach the stream to it) */
#define ROAR_ATTACH_SOURCE 2 /* attach a new source to the server */
#define ROAR_ATTACH_OUTPUT 3 /* attach a new output stream to the server */

// modes for ROAR_CMD_?ET_STREAM_PARA...
#define ROAR_STREAM_PARA_INFO       1
#define ROAR_STREAM_PARA_FLAGS      2
#define ROAR_STREAM_PARA_NAME       3
#define ROAR_STREAM_PARA_CHANMAP    4
#define ROAR_STREAM_PARA_ROLE       5

struct roar_timeofday {
 int64_t  t_sec;   // secund part of system time
 uint64_t t_nsec;  // nano-secund part of system time // we use a int64 here so all types are int64...
 uint64_t c_nhz;   // system hi res timer freq in nHz
 uint64_t d_nsps;  // clock drift in nano secs per sec
};

#endif

//ll
