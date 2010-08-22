//vs.c:

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

#include "libroar.h"

#define FLAG_NONE     0x0000
#define FLAG_STREAM   0x0001
#define FLAG_NONBLOCK 0x0002
#define FLAG_BUFFERED 0x0004

#define _initerr()  do { errno = 0; roar_err_clear(); } while(0)
#define _seterr(x)  do { if ( error != NULL ) *error = (x); } while(0)
#define _seterrre() do { _seterr(roar_errno); } while(0)
#define _seterrse() do { roar_err_from_errno(); _seterr(roar_errno); } while(0)

struct roar_vs {
 int flags;
 struct roar_connection con_store;
 struct roar_connection * con;
 struct roar_stream       stream;
 struct roar_vio_calls    vio;
 struct roar_audio_info   info;
 size_t                   readc, writec;
 int                      mixerid;
 int                      first_primid;
};

static int _roar_vs_find_first_prim(roar_vs_t * vss);

const char * roar_vs_strerr(int error) {
 const struct {
  int err;
  const char * msg;
 } msgs[] = {
  {ROAR_ERROR_NONE,        "No error"},
  {ROAR_ERROR_PERM,        "Operation not permitted"},
  {ROAR_ERROR_NOENT,       "No such object, file or directory"},
  {ROAR_ERROR_BADMSG,      "Bad message"},
  {ROAR_ERROR_BUSY,        "Device or resource busy"},
  {ROAR_ERROR_CONNREFUSED, "Connection refused"},
  {ROAR_ERROR_NOSYS,       "Function not implemented"},
  {ROAR_ERROR_NOTSUP,      "Operation not supported"},
  {ROAR_ERROR_PIPE,        "Broken pipe"},
  {ROAR_ERROR_PROTO,       "Protocol error"},
  {ROAR_ERROR_RANGE,       "Result too large or parameter out of range"},
  {ROAR_ERROR_MSGSIZE,     "Message too long"},
  {ROAR_ERROR_NOMEM,       "Not enough space"},
  {ROAR_ERROR_INVAL,       "Invalid argument"},
  {-1, NULL}
 };
 int i;

 for (i = 0; msgs[i].msg != NULL; i++)
  if ( msgs[i].err == error )
   return msgs[i].msg;

 return "(unknown)";
}

static roar_vs_t * roar_vs_init(int * error) {
 roar_vs_t * vss = roar_mm_malloc(sizeof(roar_vs_t));

 if ( vss == NULL ) {
  _seterrse();
  return NULL;
 }

 memset(vss, 0, sizeof(roar_vs_t));

 vss->mixerid      = -1;
 vss->first_primid = -1;

 return vss;
}

roar_vs_t * roar_vs_new_from_con(struct roar_connection * con, int * error) {
 roar_vs_t * vss = roar_vs_init(error);

 if ( vss == NULL )
  return NULL;

 vss->con = con;

 return vss;
}

roar_vs_t * roar_vs_new(const char * server, const char * name, int * error) {
 roar_vs_t * vss = roar_vs_init(error);
 int ret;

 if ( vss == NULL )
  return NULL;

 vss->con = &(vss->con_store);

 _initerr();

 ret = roar_simple_connect(vss->con, (char*)server, (char*)name);

 if ( ret == -1 ) {
  roar_vs_close(vss, ROAR_VS_TRUE, NULL);
  _seterrre();
  return NULL;
 }

 return vss;
}

int roar_vs_stream(roar_vs_t * vss, const struct roar_audio_info * info, int dir, int * error) {
 struct roar_stream_info sinfo;
 int ret;

 if ( vss->flags & FLAG_STREAM ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 if ( info != &(vss->info) )
  memcpy(&(vss->info), info, sizeof(struct roar_audio_info));

 ret = roar_vio_simple_new_stream_obj(&(vss->vio), vss->con, &(vss->stream),
                                      info->rate, info->channels, info->bits, info->codec,
                                      dir
                                     );

 if ( ret == -1 ) {
  _seterrre();
  return -1;
 }

 if ( roar_stream_get_info(vss->con, &(vss->stream), &sinfo) != -1 ) {
  vss->mixerid = sinfo.mixer;
  _roar_vs_find_first_prim(vss);
 }

 vss->flags |= FLAG_STREAM;

 return 0;
}

roar_vs_t * roar_vs_new_simple(const char * server, const char * name, int rate, int channels, int codec, int bits, int dir, int * error) {
 roar_vs_t * vss = roar_vs_new(server, name, error);
 int ret;

 if (vss == NULL)
  return NULL;

 memset(&(vss->info), 0, sizeof(vss->info));

 vss->info.rate     = rate;
 vss->info.channels = channels;
 vss->info.codec    = codec;
 vss->info.bits     = bits;

 ret = roar_vs_stream(vss, &(vss->info), dir, error);

 if (ret == -1) {
  roar_vs_close(vss, ROAR_VS_TRUE, NULL);
  return NULL;
 }

 return vss;
}

int roar_vs_close(roar_vs_t * vss, int killit, int * error) {
 if ( killit != ROAR_VS_TRUE && killit != ROAR_VS_FALSE ) {
  _seterr(ROAR_ERROR_UNKNOWN);
  return -1;
 }

 if ( vss->flags & FLAG_STREAM ) {
  if ( killit == ROAR_VS_TRUE ) {
   roar_kick(vss->con, ROAR_OT_STREAM, roar_stream_get_id(&(vss->stream)));
  }

  roar_vio_close(&(vss->vio));
 }

 if ( vss->con == &(vss->con_store) ) {
  roar_disconnect(vss->con);
 }

 roar_mm_free(vss);
 return 0;
}

ssize_t roar_vs_write(roar_vs_t * vss, const void * buf, size_t len, int * error) {
 ssize_t ret;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 ret = roar_vio_write(&(vss->vio), (void*)buf, len);

 if ( ret == -1 ) {
#ifdef EAGAIN
  if ( errno == EAGAIN )
   return 0;
#endif

#ifdef EWOULDBLOCK
  if ( errno == EWOULDBLOCK )
   return 0;
#endif

  _seterrre();
 } else {
  vss->writec += ret;
 }

 return ret;
}

ssize_t roar_vs_read (roar_vs_t * vss,       void * buf, size_t len, int * error) {
 ssize_t ret;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 ret = roar_vio_read(&(vss->vio), buf, len);

 if ( ret == -1 ) {
  _seterrre();
 } else {
  vss->readc += ret;
 }

 return ret;
}

int     roar_vs_sync (roar_vs_t * vss, int wait, int * error) {
 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 if ( wait != ROAR_VS_NOWAIT ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 if ( roar_vio_sync(&(vss->vio)) == -1 ) {
  _seterrre();
  return -1;
 }

 return 0;
}

int     roar_vs_blocking (roar_vs_t * vss, int val, int * error) {
 int old = -1;

  if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 old = vss->flags & FLAG_NONBLOCK ? ROAR_VS_FALSE : ROAR_VS_TRUE;

 _initerr();

 switch (val) {
  case ROAR_VS_TRUE:
    if ( roar_vio_nonblock(&(vss->vio), ROAR_SOCKET_BLOCK) == -1 ) {
     _seterrre();
     return -1;
    }
    vss->flags |= FLAG_NONBLOCK;
    vss->flags -= FLAG_NONBLOCK;
    return old;
   break;
  case ROAR_VS_FALSE:
    if ( roar_vio_nonblock(&(vss->vio), ROAR_SOCKET_NONBLOCK) == -1 ) {
     _seterrre();
     return -1;
    }
    vss->flags |= FLAG_NONBLOCK;
    return old;
   break;
  case ROAR_VS_TOGGLE:
    if ( old == ROAR_VS_TRUE ) {
     return roar_vs_blocking(vss, ROAR_VS_FALSE, error);
    } else {
     return roar_vs_blocking(vss, ROAR_VS_TRUE, error);
    }
   break;
  case ROAR_VS_ASK:
    return old;
   break;
 }

 _seterr(ROAR_ERROR_INVAL);
 return -1;
}

static int _roar_vs_find_first_prim(roar_vs_t * vss) {
 struct roar_stream stream;
 struct roar_stream_info info;
 int id[ROAR_STREAMS_MAX];
 int num;
 int i;

 if ( vss->first_primid != -1 )
  return vss->first_primid;

 if ( vss->mixerid == -1 )
  return -1;

 if ( (num = roar_list_streams(vss->con, id, ROAR_STREAMS_MAX)) == -1 ) {
  return -1;
 }

 for (i = 0; i < num; i++) {
  if ( roar_get_stream(vss->con, &stream, id[i]) == -1 )
   continue;

  if ( stream.dir != ROAR_DIR_OUTPUT )
   continue;

  if ( roar_stream_get_info(vss->con, &stream, &info) == -1 )
   continue;

  if ( info.mixer == vss->mixerid ) {
   vss->first_primid = id[i];
   return id[i];
  }
 }

 return -1;
}

ssize_t roar_vs_position(roar_vs_t * vss, int backend, int * error) {
 struct roar_stream stream;
 struct roar_stream      out_stream;
 struct roar_stream_info out_info;
 size_t offset;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 if ( roar_get_stream(vss->con, &stream, roar_stream_get_id(&(vss->stream))) == -1 ) {
  _seterrre();
  return -1;
 }

 switch (backend) {
  case ROAR_VS_BACKEND_NONE:
    return stream.pos;
   break;
  case ROAR_VS_BACKEND_FIRST:
   // _roar_vs_find_first_prim(vss);
    if ( vss->first_primid == -1 ) {
     _seterr(ROAR_ERROR_UNKNOWN);
     return -1;
    }

    roar_stream_new_by_id(&out_stream, vss->first_primid);

    if ( roar_stream_get_info(vss->con, &out_stream, &out_info) == -1 ) {
     _seterrre();
     return -1;
    }

    offset  = out_info.delay * vss->info.rate;
    offset /= 1000000;

    return stream.pos + offset;
   break;
  default:
    _seterr(ROAR_ERROR_NOTSUP);
    return -1;
   break;
 }

 _seterr(ROAR_ERROR_NOSYS);
 return -1;
}

roar_mus_t roar_vs_latency(roar_vs_t * vss, int backend, int * error) {
 ssize_t pos  = roar_vs_position(vss, backend, error);
 ssize_t bps;  // byte per sample
 size_t  lioc; // local IO (byte) counter
 size_t  lpos; // local possition
 roar_mus_t  lag;

 _initerr();

 if (pos == -1) {
  _seterrre();
  return 0;
 }

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return 0;
 }

 if ( vss->writec == 0 ) {
  lioc = vss->readc;
 } else {
  lioc = vss->writec;
 }

 bps = roar_info2samplesize(&(vss->info));

 if ( bps == -1 ) {
  _seterrre();
  return 0;
 }

 lpos = lioc / bps;

 lag = (roar_mus_t)lpos - (roar_mus_t)pos;

 // we now have the lag in frames
 // return value are ms
 // so we need to multiply with 1s/ms and
 // multiply by 1/rate

 lag *= 1000000; // 1s/ms
 lag /= vss->info.rate;

 if ( lag == 0 ) {
  _seterr(ROAR_ERROR_NONE);
 }

 return lag;
}

static int roar_vs_flag(roar_vs_t * vss, int flag, int val, int * error) {
 struct roar_stream_info info;
 int old = -1;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 if ( val != ROAR_VS_ASK )
  old = roar_vs_flag(vss, flag, ROAR_VS_ASK, error);

 _initerr();

 switch (val) {
  case ROAR_VS_TRUE:
  case ROAR_VS_FALSE:
    if ( roar_stream_set_flags(vss->con, &(vss->stream), flag,
                               val == ROAR_VS_TRUE ? ROAR_SET_FLAG : ROAR_RESET_FLAG) == -1 ) {
     _seterrre();
     return -1;
    }
    return old;
   break;
  case ROAR_VS_TOGGLE:
    return roar_vs_flag(vss, flag, old == ROAR_VS_TRUE ? ROAR_VS_FALSE : ROAR_VS_TRUE, error);
   break;
  case ROAR_VS_ASK:
    if ( roar_stream_get_info(vss->con, &(vss->stream), &info) == -1 ) {
     _seterrre();
     return -1;
    }
    return info.flags & flag ? ROAR_VS_TRUE : ROAR_VS_FALSE;
   break;
 }

 _seterr(ROAR_ERROR_INVAL);
 return -1;
}

int     roar_vs_pause(roar_vs_t * vss, int val, int * error) {
 return roar_vs_flag(vss, ROAR_FLAG_PAUSE, val, error);
}

int     roar_vs_mute (roar_vs_t * vss, int val, int * error) {
 return roar_vs_flag(vss, ROAR_FLAG_MUTE, val, error);
}

static int roar_vs_volume (roar_vs_t * vss, float * c, size_t channels, int * error) {
 struct roar_mixer_settings mixer;
 size_t i;
 register float s;
 int oldchannels;
 int handled;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 if ( channels > ROAR_MAX_CHANNELS ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 if ( roar_get_vol(vss->con, roar_stream_get_id(&(vss->stream)), &mixer, &oldchannels) == -1 ) {
  _seterrre();
  return -1;
 }

 for (i = 0; i < channels; i++) {
  s = c[i] * 65535.0;
  if ( s > 66190.0 || s < -655.0 ) {
   _seterr(ROAR_ERROR_RANGE);
   return -1;
  } else if ( s > 65535.0 ) {
   s = 65535.0;
  } else if ( s <     0.0 ) {
   s = 0.0;
  }
  mixer.mixer[i] = s;
 }

 mixer.scale = 65535;

 if ( channels != oldchannels ) {
  handled = 0;
  switch (oldchannels) {
   case 1:
     if ( channels == 2 ) {
      mixer.mixer[0] = (mixer.mixer[0] + mixer.mixer[1]) / 2;
      handled = 1;
     }
    break;
   case 2:
     if ( channels == 1 ) {
      mixer.mixer[1] = mixer.mixer[0];
      handled = 1;
     }
    break;
   case 4:
     if ( channels == 1 ) {
      mixer.mixer[1] = mixer.mixer[0];
      mixer.mixer[2] = mixer.mixer[0];
      mixer.mixer[3] = mixer.mixer[0];
      handled = 1;
     } else if ( channels == 2 ) {
      mixer.mixer[2] = mixer.mixer[0];
      mixer.mixer[3] = mixer.mixer[1];
      handled = 1;
     }
    break;
  }
  if ( handled ) {
   channels = oldchannels;
  } else {
   _seterr(ROAR_ERROR_INVAL);
   return -1;
  }
 }

 if ( roar_set_vol(vss->con, roar_stream_get_id(&(vss->stream)), &mixer, channels) == -1 ) {
  _seterrre();
  return -1;
 }

 return 0;
}

int     roar_vs_volume_mono   (roar_vs_t * vss, float c, int * error) {
 return roar_vs_volume(vss, &c, 1, error);
}

int     roar_vs_volume_stereo (roar_vs_t * vss, float l, float r, int * error) {
 float c[2] = {l, r};
 return roar_vs_volume(vss, c, 2, error);
}

int     roar_vs_volume_get    (roar_vs_t * vss, float * l, float * r, int * error) {
 struct roar_mixer_settings mixer;
 int channels;

 if ( vss == NULL || l == NULL || r == NULL ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 if ( roar_get_vol(vss->con, roar_stream_get_id(&(vss->stream)), &mixer, &channels) == -1 ) {
  _seterrre();
  return -1;
 }

 if ( channels == 1 )
  mixer.mixer[1] = mixer.mixer[0];

 *l = mixer.mixer[0] / (float)mixer.scale;
 *r = mixer.mixer[1] / (float)mixer.scale;

 return 0;
}

int     roar_vs_meta          (roar_vs_t * vss, struct roar_keyval * kv, size_t len, int * error) {
 struct roar_meta meta;
 size_t i;
 int type;
 int ret = 0;

 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 meta.type   = ROAR_META_TYPE_NONE;
 meta.key[0] = 0;
 meta.value  = NULL;

 _initerr();

 if ( roar_stream_meta_set(vss->con, &(vss->stream), ROAR_META_MODE_CLEAR, &meta) == -1 ) {
  _seterrre();
  ret = -1;
 }

 for (i = 0; i < len; i++) {
  type = roar_meta_inttype(kv[i].key);
  meta.type  = type;
  meta.value = kv[i].value;

  if ( roar_stream_meta_set(vss->con, &(vss->stream), ROAR_META_MODE_ADD, &meta) == -1 ) {
   _seterrre();
   ret = -1;
  }
 }

 meta.type   = ROAR_META_TYPE_NONE;
 meta.key[0] = 0;
 meta.value  = NULL;
 if ( roar_stream_meta_set(vss->con, &(vss->stream), ROAR_META_MODE_FINALIZE, &meta) == -1 ) {
  _seterrre();
  ret = -1;
 }

 return ret;
}

int     roar_vs_role          (roar_vs_t * vss, int role, int * error) {
 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return -1;
 }

 _initerr();

 if ( roar_stream_set_role(vss->con, &(vss->stream), role) == -1 ) {
  _seterrre();
  return -1;
 }

 return 0;
}

struct roar_connection * roar_vs_connection_obj(roar_vs_t * vss, int * error) {
 return vss->con;
}

struct roar_stream     * roar_vs_stream_obj    (roar_vs_t * vss, int * error) {
 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return NULL;
 }

 return &(vss->stream);
}

struct roar_vio_calls  * roar_vs_vio_obj       (roar_vs_t * vss, int * error) {
 if ( !(vss->flags & FLAG_STREAM) ) {
  _seterr(ROAR_ERROR_INVAL);
  return NULL;
 }

 return &(vss->vio);
}

//ll
