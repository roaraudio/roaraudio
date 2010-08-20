//vs.h:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2010
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
 *  The libs libroaresd, libroararts and libroarpulse link this lib
 *  and are therefore GPL. Because of this it may be illigal to use
 *  them with any software that uses libesd, libartsc or libpulse*.
 */

#ifndef _LIBROARVS_H_
#define _LIBROARVS_H_

#include "libroar.h"

struct roar_vs;

typedef struct roar_vs roar_vs_t;
typedef signed long int roar_mus_t;

/* return readable string descriping the problem */
const char * roar_vs_strerr(int error);

/* create a new VS object from normal RoarAudio connection object
 * The connection must not be closed caller before roar_vs_close() is called.
 * The connection is not closed by roar_vs_close().
 */
roar_vs_t * roar_vs_new_from_con(struct roar_connection * con, int * error);

/* create a new VS object with a new connection */
roar_vs_t * roar_vs_new(const char * server, const char * name, int * error);

/* start a the stream in the VS object */
int roar_vs_stream(roar_vs_t * vss, const struct roar_audio_info * info, int dir, int * error);

/* connect to server and start stream in once
 * this is basicly roar_vs_new() and roar_vs_stream() in one function.
 */
roar_vs_t * roar_vs_new_simple(const char * server, const char * name, int rate, int channels, int codec, int bits, int dir, int * error);

/* create a VS object for playback.
 * This is roar_vs_new_simple() with direction set to 'playback' (wave form data)
 */
#define roar_vs_new_playback(s,n,r,c,e,b,error) roar_vs_new_simple((s), (n), (r), (c), (e), (b), ROAR_DIR_PLAY, (error))

/* Boolean TRUE for VS functions */
#define ROAR_VS_TRUE     1
/* Boolean FALSE for VS functions */
#define ROAR_VS_FALSE    0
/* Boolean TOGGLE for VS functions */
#define ROAR_VS_TOGGLE  -1
/* Boolean value used to ask for a value, do not change the value only ask for current value */
#define ROAR_VS_ASK     -2

/* close and free the VS object
 * This does all needed cleanup.
 * If server connection was made by VS it is closed, too.
 * If server connectionw as provided by caller it is untouched.
 */
int roar_vs_close(roar_vs_t * vss, int killit, int * error);

/* write data to a stream
 * This function writes some data to the stream.
 * return is number of bytes written or -1 on error.
 * return value can be zero to intercate no data can be written but no error.
 * this may be the case with non-blocking streams.
 * retruned value can be less then requested value. indecates a short write.
 * you should wait some (short!) time (for example one main loop interation) and try again.
 */
ssize_t roar_vs_write(roar_vs_t * vss, const void * buf, size_t len, int * error);

/* read data from a stream
 * This function reads some data from the stream.
 * return is number of bytes read or -1 on error.
 * return value can be zero to intercate no data can be read but no error.
 * this may be the case with non-blocking streams.
 * retruned value can be less then requested value. indecates a short read.
 * you should wait some (short!) time (for example one main loop interation) and try again.
 */
ssize_t roar_vs_read (roar_vs_t * vss,       void * buf, size_t len, int * error);

/* wait value for waiting */
#define ROAR_VS_WAIT   1
/* wait value for no waiting */
#define ROAR_VS_NOWAIT 0

/* sync a stream with the server (flush buffers)
 * Returns 0 on no error and -1 on error.
 */
int     roar_vs_sync (roar_vs_t * vss, int wait, int * error);

/* set blocking mode of stream
 * returns old blocking state
 */
int     roar_vs_blocking (roar_vs_t * vss, int val, int * error);

/* TODO: document this */
#define ROAR_VS_BACKEND_DEFAULT ROAR_VS_BACKEND_FIRST
#define ROAR_VS_BACKEND_NONE    -1
#define ROAR_VS_BACKEND_FIRST   -2
#define ROAR_VS_BACKEND_MEAN    -3

ssize_t roar_vs_position(roar_vs_t * vss, int backend, int * error);

roar_mus_t roar_vs_latency(roar_vs_t * vss, int backend, int * error);

/* set pause flag
 * The pause flag should be set whenever the user presses the pause button or simular.
 * The stream may be come blocking after the pause flag has been set.
 * returns old pause setting (usefull with ROAR_VS_TOGGLE)
 */
int     roar_vs_pause(roar_vs_t * vss, int val, int * error);

/* set the mute flag of the stream
 * Thie pause flag should be set wehnever the user mutes the stream in some way.
 * This flag is used so the volume is not changed and can be restored by the server
 * while unmuting.
 * It is very recommended to use this flag and not just set the volume to zero
 * returns old mute setting (usefull with ROAR_VS_TOGGLE)
 */
int     roar_vs_mute (roar_vs_t * vss, int val, int * error);

/* set volume of stream (all channels to the same value)
 * volume c is float from 0 ('muted', see above) to 1 (full volume).
 * Returns 0 on no error and -1 on error.
 */
int     roar_vs_volume_mono   (roar_vs_t * vss, float c, int * error);
/* set volume of stream (like volume + ballance, stereo mode)
 * volume l and r are floats from 0 ('muted', see above) to 1 (full volume).
 * Returns 0 on no error and -1 on error.
 */
int     roar_vs_volume_stereo (roar_vs_t * vss, float l, float r, int * error);

/* get volume from stream (like volume + ballance, stereo mode)
 * volume pinters l and r are floats from 0 ('muted', see above) to 1 (full volume).
 * Returns 0 on no error and -1 on error.
 * NOTE: if you want a 'mono' volume (like roar_vs_volume_mono() takes)
 * you can just use: c = (*l + *r)/2
 */
int     roar_vs_volume_get    (roar_vs_t * vss, float * l, float * r, int * error);

/* set an array of meta data for the stream
 * This sets an array of meta data stored in kv of length len for
 * the stream.
 * This should be called before streaming is started using read or write functions
 * but may be called at any time (for example to updata meta data).
 * Returns 0 on no error and -1 on error.
 * Example:
 * struct roar_keyval kv = {.key = "TITLE", .value = "Some artist"};
 * ret = roar_vs_meta(vss, &kv, 1, &err);
 */
int     roar_vs_meta          (roar_vs_t * vss, struct roar_keyval * kv, size_t len, int * error);

/* sets the stream role
 * see ../roaraudio/stream.h for possible roles
 * Returns 0 on no error and -1 on error.
 */
int     roar_vs_role          (roar_vs_t * vss, int role, int * error);

/* Get used connection object
 * This may be usefull if you want to use functions from the main API.
 * Returns used connection object or NULL on error.
 */
struct roar_connection * roar_vs_connection_obj(roar_vs_t * vss, int * error);

/* Get used stream object
 * This may be usefull if you want to use functions from the main API.
 * Returns used stream object or NULL on error.
 */
struct roar_stream     * roar_vs_stream_obj    (roar_vs_t * vss, int * error);

/* Get used VIO object
 * This may be usefull if you want to use functions from the main API.
 * For example this can be used in non-blocking mode
 * to test if we can read or write. To test that use roar_vio_select().
 * Returns used VIO object or NULL on error.
 */
struct roar_vio_calls  * roar_vs_vio_obj       (roar_vs_t * vss, int * error);

/* send NOOP command to server
 * This can be used to ping the server.
 * This is of no use normaly.
 * Returns 0 on no error and -1 on error.
 */
#define roar_vs_noop(v, error) roar_noop(roar_vs_connection_obj((v), (error)))

#endif

//ll
