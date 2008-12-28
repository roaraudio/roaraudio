//cdrom.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

#include "libroar.h"
#include <sys/types.h>
#include <signal.h>

#define ROAR_CDROM_ERROR_NORETURN(format, args...) ROAR_ERR(format, ## args); _exit(3)

#if BYTE_ORDER == BIG_ENDIAN
#define ROAR_CDROM_CDPARANOIA_OUTPUTFORMAT "--output-raw-big-endian"
#elif BYTE_ORDER == LITTLE_ENDIAN
#define ROAR_CDROM_CDPARANOIA_OUTPUTFORMAT "--output-raw-little-endian"
#endif

#ifdef ROAR_TARGET_WIN32
#undef ROAR_HAVE_BIN_CDPARANOIA
#endif

pid_t roar_cdrom_run_cdparanoia (int cdrom, int data, int track, char * pos) {
#if defined(ROAR_HAVE_BIN_CDPARANOIA) && defined(ROAR_CDROM_CDPARANOIA_OUTPUTFORMAT)
 char my_pos[32] = {0};
 pid_t pid;
 int fh[2];

 ROAR_DBG("roar_cdrom_run_cdparanoia(cdrom=%i, data=%i, track=%i, pos='%s') = ?", cdrom, data, track, pos);

 if ( cdrom == -1 || data == -1 || (track == -1 && pos == NULL) || (track != -1 && pos != NULL) )
  return -1;

 if ( track != -1 ) {
  pos = my_pos;
  snprintf(pos, 32, "%i", track);
 }

 if ( (pid = fork()) == -1 ) {
  return -1;
 }

 if ( pid )
  return pid;

 fh[0] = dup(cdrom);
 fh[1] = dup(data);

 if ( fh[0] == -1 || fh[1] == -1 ) {
  ROAR_CDROM_ERROR_NORETURN("Can not dup(): %s", strerror(errno));
 }

 close(ROAR_STDIN);
 close(ROAR_STDOUT);

 // TODO: should I close some other handles?

 if ( dup2(fh[0], ROAR_STDIN) == -1 || dup2(fh[1], ROAR_STDOUT) == -1 ) {
  ROAR_CDROM_ERROR_NORETURN("Can not dup2(): %s", strerror(errno));
 }

 // new close our backups:
 close(fh[0]);
 close(fh[1]);

 execl(ROAR_HAVE_BIN_CDPARANOIA, "cdparanoia", "--force-cdrom-device", "/dev/stdin", "-q",
                ROAR_CDROM_CDPARANOIA_OUTPUTFORMAT, pos, "-", NULL);

 ROAR_CDROM_ERROR_NORETURN("We are still alive after exec()!, very bad!, error was: %s", strerror(errno));
 return -1;
#else
#ifndef ROAR_HAVE_BIN_CDPARANOIA
 ROAR_ERR("roar_cdrom_run_cdparanoia(*): ROAR_HAVE_BIN_CDPARANOIA not defined!");
#endif
#ifndef ROAR_CDROM_CDPARANOIA_OUTPUTFORMAT
 ROAR_ERR("roar_cdrom_run_cdparanoia(*): ROAR_CDROM_CDPARANOIA_OUTPUTFORMAT not defined!");
#endif
 ROAR_ERR("roar_cdrom_run_cdparanoia(cdrom=%i, data=%i, track=%i, pos='%s') = -1 // no cdparanoia support compiled in",
             cdrom, data, track, pos);
 return -1;
#endif
}

int roar_cdrom_open (struct roar_connection * con, struct roar_cdrom * cdrom, char * device) {
 int flags;

 if ( cdrom == NULL )
  return -1;

 memset((void*)cdrom, 0, sizeof(struct roar_cdrom));

 if ( device == NULL )
  device = roar_cdromdevice();

 if ( device == NULL )
  return -1;

 strncpy(cdrom->device, device, ROAR_CDROM_MAX_DEVLEN);

 cdrom->con        = con; // we do not care here if it is set or not as we can operate in local only mode

 cdrom->stream     = -1;
 cdrom->play_local =  1;
 cdrom->player     = -1;

 if ( (cdrom->fh = open(cdrom->device, O_RDONLY, 0644)) == -1 )
  return -1;

#ifndef ROAR_TARGET_WIN32
 if ( (flags = fcntl(cdrom->fh, F_GETFL, 0)) == -1 ) {
  close(cdrom->fh);
  cdrom->fh  = -1;
  return -1;
 }

 flags |= FD_CLOEXEC;

 if ( fcntl(cdrom->fh, F_SETFL, flags) == -1 ) {
  close(cdrom->fh);
  cdrom->fh = -1;
  return -1;
 }
#endif

 return 0;
}

int roar_cdrom_close(struct roar_cdrom * cdrom) {
 if ( cdrom == NULL )
  return -1;

 roar_cdrom_stop(cdrom); // stop on close

 if ( cdrom->fh != -1 )
  close(cdrom->fh);

 memset((void*)cdrom, 0, sizeof(struct roar_cdrom));

 return 0;
}

int roar_cdrom_stop (struct roar_cdrom * cdrom) {
 int ret;

 if ( cdrom == NULL )
  return -1;

 if ( cdrom->con == NULL )
  return -1;

 if ( cdrom->stream == -1 )
  return -1;

 if ( (ret = roar_kick(cdrom->con, ROAR_OT_STREAM, cdrom->stream)) == -1 ) {
  return -1;
 }

#ifndef ROAR_TARGET_WIN32
 if ( cdrom->player != -1 )
  kill(cdrom->player, SIGINT);
#else
 if ( cdrom->player != -1 ) {
  ROAR_ERR("roar_cdrom_stop(*): Can not kill player with pid %i, not supported on win32", cdrom->player);
 }
#endif

 cdrom->player = -1;
 cdrom->stream = -1;

 return ret;
}

int roar_cdrom_play (struct roar_cdrom * cdrom, int track) {
 int stream_fh;
 struct roar_stream stream[1];

 if ( cdrom == NULL )
  return -1;

 if ( cdrom->con == NULL )
  return -1;

 if ( cdrom->stream != -1 ) {
  if ( roar_cdrom_stop(cdrom) == -1 )
   return -1;
 }

 if ( cdrom->play_local ) {

  if ( (stream_fh = roar_simple_new_stream_obj(cdrom->con, stream, ROAR_CDROM_STREAMINFO, ROAR_DIR_PLAY)) == -1 ) {
   return -1;
  }

  if ( (cdrom->player = roar_cdrom_run_cdparanoia(cdrom->fh, stream_fh, track, NULL)) != -1 ) {
   cdrom->stream = stream->id;
   return 0;
  }

  close(stream_fh);

  return -1;
 } else {
  // no support for remote playback yet
  return -1;
 }
}

//ll
