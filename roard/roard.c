//roard.c:

/*
 *      Copyright (C) Philipp 'ph3-der-loewe' Schafft - 2008
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

#include "roard.h"

#ifdef ROAR_SUPPORT_LISTEN
char * server[ROAR_MAX_LISTEN_SOCKETS];
#endif

#if defined(ROAR_HAVE_IO_POSIX) && defined(ROAR_HAVE_FS_POSIX)
#define SUPPORT_PIDFILE
char * pidfile = NULL;
#endif

#if defined(ROAR_HAVE_SETGID) || defined(ROAR_HAVE_SETUID)
 int    setids    = 0;
#endif

#ifdef ROAR_HAVE_MAIN_ARGS
void usage (void) {
 printf("Usage: roard [OPTIONS]...\n\n");

 printf("Misc Options:\n\n");
 printf(
        " --daemon              - Bring the server into background after init\n"
        " --verbose             - Be more verbose, can be used multiple times\n"
        " --terminate           - Terminate after last client quited\n"
        " --start               - No op parameter (starting roard is default operation)\n"
        " --restart             - Trys to stop an old instance and start a new with new settings\n"
        " --stop                - Stops a running roard (provide --pidfile!)\n"
        " --shutdown            - Terminates a running roard (provide --pidfile!)\n"
        " --realtime            - Trys to get realtime priority,\n"
        "                         give multible times for being more realtime\n"
        " --chroot DIR          - chroots to the given dir\n"
        " --setgid              - GroupID to the audio group as specified via -G\n"
        " --setuid              - UserID to the audio user as specified via -U\n"
        " --sysclocksync        - calculate exact sample rate using the system clock\n"
        " --location  LOC       - Set lion readable location of server\n"
#ifdef SUPPORT_PIDFILE
        " --pidfile PIDFILE     - Write a pidfile at PIDFILE\n"
#endif
       );

 printf("\nAudio Options:\n\n");
 printf(
        " -R  --rate   RATE     - Set server rate\n"
        " -B  --bits   BITS     - Set server bits\n"
        " -C  --chans  CHANNELS - Set server channels\n"
       );

 printf("\nStream Options:\n\n");
 printf(
        " --stream-flags D=F    - Set default flags for stream directions\n"
        "                         D is the stream direction and F is a comma seperated\n"
        "                         list of flags in form +flag or -flag to set or unset\n"
        "                         a flag as default or remove it from the default\n"
       );

 printf("\nDriver Options: (obsolete, do not use, Use Ouput Options)\n\n");
 printf(" -d  --driver DRV      - Set the driver (default: %s)\n", ROAR_DRIVER_DEFAULT);
 printf(" -D  --device DEV      - Set the device\n");
 printf(" -dO OPTS              - Set output options\n");
 printf(" --list-driver         - List all drivers\n");

 printf("\nOutput Options:\n\n");
 printf(" -o  --odriver DRV     - Set the driver, use '--list-driver' to get a list\n");
 printf(" -O  --odevice DEV     - Set the device\n");
 printf(" -oO OPTS              - Set output options\n");
 printf(" -oN                   - Adds another output\n");
 printf(" -oP                   - Mark output as primary\n");

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 printf("\nSource Options:\n\n");
 printf(" -s  --source DRV      - Use DRV as input driver\n"
        " -S           DEV      - Use DEV as input device\n"
        " -sO          OPTS     - Use OPTS as input options\n"
        " -sN                   - Adds another source\n"
        " -sP                   - Make souce as primary\n"
       );
 printf(" --list-sources        - List all sources\n");
#endif

 printf("\nCodec Filter Options:\n\n");
 printf(" --list-cf             - List all codec filter\n"
       );

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 printf("\nMIDI Options:\n\n");
 printf(" --midi-no-console     - Disable console based MIDI synth\n"
        " --midi-console-enable - Enables the console based MIDI synth\n"
        " --midi-console DEV    - Set device for MIDI console\n"
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
        " --ssynth-enable       - Enable simple software synth\n"
        " --ssynth-disable      - Disable simple software synth\n"
#endif
       );
#endif

#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 printf("\nLight Control Options:\n\n");
 printf(" --light-channels NUM  - Sets the number of channels for Light control (default: %i)\n",
                                  LIGHT_CHANNELS_DEFAULT
       );
#endif

#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 printf("\nRadio Date and Transmitter Control System Options:\n\n");
 printf(" --rds-pi   PI         - Sets the RDS Programme Identification (PI)\n"
        " --rds-ps   PS         - Sets the RDS Programme Service Name (PS)\n"
        " --rds-pty  PTY        - Sets the RDS Programme Type (PTY)\n"
        " --rds-tp              - Sets the RDS Traffic Programme (TP) flag\n"
        " --rds-ct              - Enables sending of RDS Clock Time (CT)\n"
       );
#endif

 printf("\nServer Options:\n\n");
 printf(" -t  --tcp             - Use TCP listen socket\n"
        " -u  --unix            - Use UNIX Domain listen socket (default)\n"
#ifdef ROAR_HAVE_LIBDNET
        " -n  --decnet          - use DECnet listen socket\n"
#endif
        " -4                    - Use IPv4 connections (implies -t)\n"
#ifdef PF_INET6
        " -6                    - Use IPv6 connections (implies -t)\n"
#endif
#ifdef IPV6_ADDRFORM
        " -64                   - Try to downgrade sockets from IPv6 into IPv4,\n"
        "                         this is normaly not usefull.\n"
#endif
        " -p  --port            - TCP Port to bind to\n"
        " -b  --bind            - IP/Hostname to bind to\n"
        "     --sock            - Filename for UNIX Domain Socket\n"
        "     --proto PROTO     - Use PROTO as protocol on Socket\n"
        "     --new-sock        - Parameters for new socket follows\n"
#ifdef ROAR_HAVE_LIBSLP
        "     --slp             - Enable OpenSLP support\n"
#endif
        " --jumbo-mtu MTU       - Sets the MTU for Jumbo Packets\n"
        " -G  GROUP             - Sets the group for the UNIX Domain Socket, (default: %s)\n"
        "                         You need the permissions to change the GID\n"
        " -U  USER              - Sets the user for the UNIX Domain Socket, (default: do not set)\n"
        "                         You need the permissions to change the UID (normaly only root has)\n"
        " --no-listen           - Do not listen for new clients\n"
        "                         (only usefull for relaing, impleys --terminate)\n"
        " --client-fh           - Comunicate with a client over this handle\n"
        "                         (only usefull for relaing)\n"
        " --close-fh            - Closes the given fh\n"
        " --standby             - Start in standby state\n"
        " --auto-standby        - Automatical goes into standby if there are no streams\n",
#ifdef ROAR_DEFAULT_SOCKGRP
        ROAR_DEFAULT_SOCKGRP
#else
        "(none)"
#endif
       );
// printf("\n Options:\n\n");
 printf("\n");
}
#endif

int restart_server (char * server, int terminate) {
 struct roar_connection con;
#ifdef ROAR_HAVE_KILL
 char buf[80];
 ssize_t l;
 struct roar_vio_calls fh;
 pid_t pid;
 int ok;

 if ( pidfile != NULL ) {
  if ( roar_vio_open_file(&fh, pidfile, O_RDONLY, 0644) == -1 ) {
   ROAR_WARN("restart_server(*): Can not read pidfile: %s", pidfile);
  } else {
   l = roar_vio_read(&fh, buf, 80);
   roar_vio_close(&fh);
   if ( l > 0 ) {
    buf[l-1] = 0;
    buf[79]  = 0;
    pid = atoi(buf);
    if ( terminate ) {
     ok = kill(pid, SIGUSR1);
    } else {
     ok = kill(pid, SIGINT);
    }
    if ( ok == 0 ) {
     return 0;
    } else {
     ROAR_WARN("restart_server(*): Can not kill roard by pidfile");
    }
   } else {
    ROAR_WARN("restart_server(*): Can not find a PID in the pidfile");
   }
  }
 }
#endif

 if ( roar_connect(&con, server) == -1 ) {
  return -1;
 }

 if ( roar_terminate(&con, terminate) == -1 ) {
  return -1;
 }

 return roar_disconnect(&con);
}

#define R_SETUID 1
#define R_SETGID 2

int init_config (void) {
 int i;

 memset(g_config, 0, sizeof(struct roard_config));

 for (i = 0; i < ROAR_DIR_DIRIDS; i++) {
  g_config->streams[i].mixer_channels = 1;
  g_config->streams[i].mixer.rpg_mul  = 1;
  g_config->streams[i].mixer.rpg_div  = 1;
  g_config->streams[i].mixer.scale    = 65535;
  g_config->streams[i].mixer.mixer[0] = g_config->streams[i].mixer.scale;
 }

 g_config->streams[ROAR_DIR_PLAY    ].flags = ROAR_FLAG_META;
 g_config->streams[ROAR_DIR_OUTPUT  ].flags = ROAR_FLAG_PASSMIXER;
 g_config->streams[ROAR_DIR_FILTER  ].flags = ROAR_FLAG_SYNC;
 g_config->streams[ROAR_DIR_MIDI_OUT].flags = ROAR_FLAG_SYNC;
 g_config->streams[ROAR_DIR_BIDIR   ].flags = ROAR_FLAG_ANTIECHO;

 g_config->location = "***default***";

 return 0;
}

#ifdef ROAR_SUPPORT_LISTEN
int init_listening (void) {
 int i;

 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  g_listen_socket[i] = -1;
  g_listen_proto[i]  = ROAR_PROTO_ROARAUDIO;
  server[i]          = NULL;
 }

 return 0;
}

int add_listen (char * addr, int port, int sock_type, char * user, char * group, int proto) {
#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
 struct group   * grp  = NULL;
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 struct passwd  * pwd  = NULL;
#endif
#ifdef ROAR_HAVE_UNIX
 char * env_roar_proxy_backup;
#endif
 int    sockid = -1;
 int    sock;
 int    i;

 if ( *addr != 0 ) {
  for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
   if ( g_listen_socket[i] == -1 ) {
    sockid = i;
    break;
   }
  }

  if ( sockid == -1 )
   return -1;

  g_listen_proto[sockid] = proto;

  ROAR_DBG("add_listen(*): proto=0x%.4x", proto);

  if ( (g_listen_socket[sockid] = roar_socket_listen(sock_type, addr, port)) == -1 ) {
#ifdef ROAR_HAVE_UNIX
   if ( *addr == '/' ) {
    if ( (env_roar_proxy_backup = getenv("ROAR_PROXY")) != NULL ) {
     env_roar_proxy_backup = strdup(env_roar_proxy_backup);
     unsetenv("ROAR_PROXY");
    }
    if ( (sock = roar_socket_connect(addr, port)) != -1 ) {
     close(sock);
     ROAR_ERR("Can not open listen socket!");
     return 1;
    } else {
     unlink(addr);
     if ( (g_listen_socket[sockid] = roar_socket_listen(sock_type, addr, port)) == -1 ) {
      ROAR_ERR("Can not open listen socket!");
      return 1;
     }
    }
    if ( env_roar_proxy_backup != NULL ) {
     setenv("ROAR_PROXY", env_roar_proxy_backup, 0);
     free(env_roar_proxy_backup);
    }
#else
   if (0) { // noop
#endif
   } else {
    ROAR_ERR("Can not open listen socket!");
    return 1;
   }
  }

#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
  if ( group != NULL ) {
   if ( (grp = getgrnam(group)) == NULL ) {
    ROAR_ERR("Can not get GID for group %s: %s", group, strerror(errno));
   }
  }
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
  if ( user ) {
   if ( (pwd = getpwnam(user)) == NULL ) {
    ROAR_ERR("Can not get UID for user %s: %s", user, strerror(errno));
   }
  }
#endif

#if defined(ROAR_HAVE_IO_POSIX) && defined(ROAR_HAVE_UNIX)
  if ( *addr == '/' ) {
   if ( grp || pwd ) {
     if ( chown(addr, pwd ? pwd->pw_uid : -1, grp ? grp->gr_gid : -1) == -1 )
      return 1;
   }
#ifdef ROAR_HAVE_GETUID
   if ( grp ) {
    if ( getuid() == 0 )
     if ( chmod(addr, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1 )
      return 1;
   }
#endif
  }
#endif
 }

 // in case we opened the listening socket correctly.
 server[sockid] = addr;
 return 0;
}
#endif

int update_stream_flags (char * str) {
 int    dir;
 char * flags;
 char * k;
 int    op;
 int    flag;

 if ( (flags = strstr(str, "=")) == NULL )
  return -1;

 *flags = 0;
  flags++;

 if ( (dir = roar_str2dir(str)) == -1 )
  return -1;

 while (flags != NULL) {
  k = flags;
  flags = strstr(flags, ",");

  if ( flags != NULL )
   *(flags++) = 0;

  switch (*k) {
   case '+': k++; op = ROAR_SET_FLAG;   break;
   case '-': k++; op = ROAR_RESET_FLAG; break;
   default:
     op = ROAR_SET_FLAG;
  }

  flag = 0;

  if ( !strcmp(k, "sync") ) {
   flag = ROAR_FLAG_SYNC;
  } else if ( !strcmp(k, "meta") ) {
   flag = ROAR_FLAG_META;
  } else if ( !strcmp(k, "cleanmeta") ) {
   flag = ROAR_FLAG_CLEANMETA;
  } else if ( !strcmp(k, "pause") ) {
   flag = ROAR_FLAG_PAUSE;
  } else if ( !strcmp(k, "mute") ) {
   flag = ROAR_FLAG_MUTE;
  } else if ( !strcmp(k, "antiecho") ) {
   flag = ROAR_FLAG_ANTIECHO;
  } else if ( !strcmp(k, "passmixer") ) {
   flag = ROAR_FLAG_PASSMIXER;
  } else {
   return -1;
  }

  g_config->streams[dir].flags |= flag;

  if ( op == ROAR_RESET_FLAG )
   g_config->streams[dir].flags -= flag;
 }

 return 0;
}

int add_output (char * drv, char * dev, char * opts, int prim, int count) {
 int stream;
 struct roar_stream * s;
 struct roar_stream_server * ss;
 char * k, * v;
#ifdef ROAR_DRIVER_CODEC
 char * to_free = NULL;
#endif
 int sync = 0, f_mmap = 0;
 int32_t blocks = -1, blocksize = -1;
 int dir = ROAR_DIR_OUTPUT;
 int error = 0;
 // DMX:
 int32_t channel  = -1;
 int32_t universe = -1;
 uint16_t tu16;
 float q = -32e6;

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 if ( drv == NULL && count == 0 ) {
  drv  = ROAR_DRIVER_DEFAULT;
  prim = 1;
  sync = 1;

#ifdef ROAR_DRIVER_CODEC
  if ( opts == NULL ) {
   opts = to_free = strdup("codec=" ROAR_DRIVER_CODEC);
  }
#endif
 }

 if ( opts == NULL && count == 0 ) {
  sync = 1;
  prim = 1; // if ( prim == 0 ) prim = 1; -> prim allways = 1
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 if ( (stream = streams_new()) == -1 ) {
  ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = -1", drv, dev, opts);
  if ( prim ) alive = 0;
  return -1;
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 streams_get(stream, &ss);
 s = ROAR_STREAM(ss);

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 memset(&(s->info), 0xFF, sizeof(struct roar_audio_info)); // set everything to -1

 s->pos_rel_id = -1;
// s->info.codec = codec;

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 if ( opts == NULL ) {
  k = NULL;
 } else {
  k = strtok(opts, ",");
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s'): initial k='%s'(%p)", drv, dev, opts, k, k);

 while (k != NULL) {
//  ROAR_WARN("add_output(*): opts: %s", k);

  if ( (v = strstr(k, "=")) != NULL ) {
   *v++ = 0;
  }

  ROAR_DBG("add_output(*): opts: k='%s', v='%s'", k, v);
  if ( strcmp(k, "rate") == 0 ) {
   s->info.rate = atoi(v);
  } else if ( strcmp(k, "channels") == 0 ) {
   s->info.channels = atoi(v);
  } else if ( strcmp(k, "bits") == 0 ) {
   s->info.bits = atoi(v);
  } else if ( strcmp(k, "codec") == 0 ) {
   if ( (s->info.codec = roar_str2codec(v)) == -1 ) {
    ROAR_ERR("add_output(*): unknown codec '%s'", v);
    error++;
   }
  } else if ( strcmp(k, "q") == 0 ) {
   q = atof(v);
  } else if ( strcmp(k, "blocks") == 0 ) {
   blocks = atoi(v);
  } else if ( strcmp(k, "blocksize") == 0 ) {
   blocksize = atoi(v);
  } else if ( strcmp(k, "mmap") == 0 ) {
   f_mmap = 1;
  } else if ( strcmp(k, "subsystem") == 0 ) {
   if ( !strcasecmp(v, "wave") || !strcasecmp(v, "waveform") ) {
    dir = ROAR_DIR_OUTPUT;
#ifndef ROAR_WITHOUT_DCOMP_MIDI
   } else if ( !strcasecmp(v, "midi") ) {
    dir = ROAR_DIR_MIDI_OUT;
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
   } else if ( !strcasecmp(v, "light") ) {
    dir = ROAR_DIR_LIGHT_OUT;
#endif
#ifndef ROAR_WITHOUT_DCOMP_RAW
   } else if ( !strcasecmp(v, "raw") ) {
    dir = ROAR_DIR_RAW_OUT;
#endif
   } else if ( !strcasecmp(v, "complex") ) {
    dir = ROAR_DIR_COMPLEX_OUT;
   } else {
    ROAR_ERR("add_output(*): unknown/unsupported subsystem '%s'", k);
    error++;
   }
  // DMX:
  } else if ( strcmp(k, "channel") == 0 ) {
   channel  = atoi(v);
   if ( channel < 0 || channel > 65535 ) {
    ROAR_ERR("add_output(*): Invalide channel (not within 0..65535): %i", channel);
    channel = -1;
    error++;
   }
  } else if ( strcmp(k, "universe") == 0 ) {
   universe = atoi(v);
   if ( universe < 0 || universe > 255 ) {
    ROAR_ERR("add_output(*): Invalide universe (not within 0..255): %i", universe);
    universe = -1;
    error++;
   }

  } else if ( strcmp(k, "name") == 0 ) {
   if ( streams_set_name(stream, v) == -1 ) {
    ROAR_ERR("add_output(*): Can not set Stream name");
    error++;
   }

  } else if ( strcmp(k, "meta") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_META);
  } else if ( strcmp(k, "sync") == 0 ) {
   sync = 1;
  } else if ( strcmp(k, "primary") == 0 ) {
   prim = 1;

  } else if ( strcmp(k, "cleanmeta") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_CLEANMETA);
  } else if ( strcmp(k, "autoconf") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_AUTOCONF);
  } else if ( strcmp(k, "recsource") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_RECSOURCE);
  } else if ( strcmp(k, "passmixer") == 0 ) {
   streams_set_flag(stream, ROAR_FLAG_PASSMIXER);
  } else {
   ROAR_ERR("add_output(*): unknown option '%s'", k);
   error++;
  }

  if ( error ) {
   streams_delete(stream);
   if ( prim ) alive = 0;
#ifdef ROAR_DRIVER_CODEC
   if ( to_free != NULL )
    free(to_free);
#endif
   return -1;
  }

  k = strtok(NULL, ",");
 }

 ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = ?", drv, dev, opts);

 // set audio info...
 switch (dir) {
  case ROAR_DIR_LIGHT_OUT:
    switch (s->info.codec) {
     case ROAR_CODEC_DMX512:
     case -1:
       if ( s->info.rate == -1 )
        s->info.rate = ROAR_OUTPUT_CFREQ;

       s->info.channels =   0;
       s->info.bits     =   8;
       s->info.codec    = ROAR_CODEC_DMX512; // in case codec == -1
      break;
    }
   break;
  case ROAR_DIR_MIDI_OUT:
    switch (s->info.codec) {
     case ROAR_CODEC_MIDI:
     case -1:
       if ( s->info.rate == -1 )
        s->info.rate    = ROAR_MIDI_TICKS_PER_BEAT;

       s->info.channels = ROAR_MIDI_CHANNELS_DEFAULT;
       s->info.bits     = ROAR_MIDI_BITS;
       s->info.codec    = ROAR_CODEC_MIDI; // in case codec == -1
      break;
    }
   break;
  case ROAR_DIR_RAW_OUT:
    if ( s->info.rate == -1 )
     s->info.rate = 0;
    if ( s->info.bits == -1 )
     s->info.bits = 0;
    if ( s->info.channels == -1 )
     s->info.channels = 0;
    if ( s->info.codec == -1 )
     s->info.codec = 0;
   break;
 }

 if ( s->info.rate == -1 )
  s->info.rate = g_sa->rate;
 if ( s->info.bits == -1 )
  s->info.bits = g_sa->bits;
 if ( s->info.channels == -1 )
  s->info.channels = g_sa->channels;
 if ( s->info.codec == -1 )
  s->info.codec = g_sa->codec;

 ROAR_DBG("add_output(*): s->info = {.rate=%i, .bits=%i, .channels=%i, .codec=%i}", s->info.rate, s->info.bits, s->info.channels, s->info.codec);

 if ( streams_set_dir(stream, dir, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

#ifdef ROAR_DRIVER_CODEC
 if ( to_free != NULL )
  free(to_free);
#endif

 if ( s->info.codec == ROAR_CODEC_ALAW || s->info.codec == ROAR_CODEC_MULAW )
  s->info.bits = 8; // needed to open OSS driver, will be overriden by codecfilter

 ROAR_STREAM_SERVER(s)->codec_orgi = s->info.codec;

 if ( driver_openvio(&(ss->vio), &(ss->driver_id), drv, dev, &(s->info), -1, ss) == -1 ) {
  ss->driver_id = -1; // don't close a driver not opened...
  memset(&(ss->vio), 0, sizeof(struct roar_vio_calls));
  streams_delete(stream);
  ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = -1", drv, dev, opts);
  if ( prim ) alive = 0;
  return -1;
 }

 roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_SSTREAMID, &stream); // ignore errors here
 roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_SSTREAM,   s); // ignore errors here

 if ( blocks != -1 )
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DBLOCKS, &blocks);

 if ( blocksize != -1 )
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DBLKSIZE, &blocksize);

 // TODO: we shoudld *really* check for errors here...
 if ( channel != -1 ) {
  tu16 = channel;
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DMXSCHAN, &tu16);
 }
 if ( universe != -1 ) {
  tu16 = universe;
  roar_vio_ctl(&(ss->vio), ROAR_VIO_CTL_SET_DMXUNIV, &tu16);
 }

 ROAR_DBG("add_output(*): ss->driver_id=%i", ss->driver_id);

 streams_set_fh(stream, -1); // update some internal structures

 if ( q > -1e6 ) {
  ROAR_DBG("add_output(*): setting q=%f", q);
  streams_ctl(stream, ROAR_CODECFILTER_CTL_SET_Q|ROAR_STREAM_CTL_TYPE_FLOAT, &q);
 }

 client_stream_add(g_self_client, stream);

 if ( prim ) {
  streams_mark_primary(stream);
  s->pos_rel_id = stream;
 }

 if ( sync ) {
  streams_set_flag(stream, ROAR_FLAG_SYNC);
 } else {
  streams_reset_flag(stream, ROAR_FLAG_SYNC);
 }

 if ( f_mmap )
  streams_set_flag(stream, ROAR_FLAG_MMAP);

 ROAR_DBG("add_output(*): s->info = {.rate=%i, .bits=%i, .channels=%i, .codec=%i}", s->info.rate, s->info.bits, s->info.channels, s->info.codec);
 return 0;
}


// SLP:
void register_slp_callback(SLPHandle hslp, SLPError errcode, void * cookie) {
 /* return the error code in the cookie */
 *(SLPError*)cookie = errcode;
}

int register_slp (int unreg, char * sockname) {
#ifdef ROAR_HAVE_LIBSLP
 static int regged = 0;
 static char * sn = NULL;
 SLPError err;
 SLPError callbackerr;
 SLPHandle hslp;
 char addr[1024];
 char attr[1024] = "";
 char * location;

 if ( sockname != NULL )
  sn = sockname;

 snprintf(addr, sizeof(addr), ROAR_SLP_URL_TYPE "://%s", sn);

 err = SLPOpen("en", SLP_FALSE, &hslp);

 if (err != SLP_OK) {
  ROAR_ERR("Error opening slp handle: Error #%i", err);
  return -1;
 }

 if (!unreg) {

  if ( SLPEscape(g_config->location, &location, SLP_FALSE) != SLP_OK ) {
   ROAR_ERR("Error using SLPEscape() on server location, really bad!");
   SLPClose(hslp);
   return -1;
  }

  snprintf(attr, sizeof(attr), "(wave-rate=%i),(wave-channels=%i),(wave-bits=%i),"
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
                               "(light-channels=%i),"
#endif
                               "(location=%s)",
           g_sa->rate, g_sa->channels, g_sa->bits,
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
           g_light_state.channels,
#endif
           location
          );

  /* Register a service with SLP */
  err = SLPReg(hslp,
               addr,
               SLP_LIFETIME_MAXIMUM,
               0,
               attr,
               SLP_TRUE,
               register_slp_callback,
               &callbackerr);
  regged = 1;
 } else if ( unreg && regged ) {
  err = SLPDereg(hslp, addr, register_slp_callback, &callbackerr);
  regged = 0;
 } else {
  SLPClose(hslp);
  return -1;
 }

 /* err may contain an error code that occurred as the slp library    */
 /* _prepared_ to make the call.                                     */
 if ( err != SLP_OK ) {
  ROAR_ERR("Error (de)registering service with slp: Error #%i", err);
  return -1;
 }

 /* callbackerr may contain an error code (that was assigned through */
 /* the callback cookie) that occurred as slp packets were sent on    */
 /* the wire */
 if (callbackerr != SLP_OK) {
  ROAR_ERR("Error (de)registering service with slp: Error #%i", callbackerr);
  return -1;
 }

 SLPClose(hslp);
 return 0;
#else
 return -1;
#endif
}


// MAIN:

#ifdef ROAR_HAVE_MAIN_ARGS
int main (int argc, char * argv[]) {
#else
int main (void) {
#endif
#ifdef ROAR_HAVE_MAIN_ARGS
 int i;
 char * k;
#endif
#if defined(ROAR_SUPPORT_LISTEN) && defined(ROAR_HAVE_GETUID)
 char user_sock[80]  = {0};
#endif
 struct roar_audio_info sa, max_sa;
 struct roard_config config;
#ifdef ROAR_HAVE_FORK
 int    daemon       = 0;
#endif
 int    realtime     = 0;
 int    sysclocksync = 0;
 char * driver    = NULL;
 char * device    = NULL;
#ifdef ROAR_HAVE_MAIN_ARGS
 char * opts      = NULL;
#endif
// char * server = ROAR_DEFAULT_SOCK_GLOBAL;
#ifdef ROAR_SUPPORT_LISTEN
 int    port       = ROAR_DEFAULT_PORT;
 char * sock_addr  = NULL;
 int    sock_proto = ROAR_PROTO_ROARAUDIO;
#endif
 int               drvid;
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 char * s_drv     = "cf";
 char * s_dev     = NULL;
 char * s_con     = NULL;
 char * s_opt     = NULL;
 int    s_prim    = 0;
#endif
 char * o_drv     = getenv("ROAR_DRIVER");
 char * o_dev     = getenv("ROAR_DEVICE");
 char * o_opts    = NULL;
 int    o_prim    = 0;
 int    o_count   = 0;
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 int    light_channels = LIGHT_CHANNELS_DEFAULT;
#endif
#ifdef ROAR_DEFAULT_SOCKGRP
 char * sock_grp  = ROAR_DEFAULT_SOCKGRP;
#else
 char * sock_grp  = NULL;
#endif
 char * sock_user = NULL;
#ifdef ROAR_SUPPORT_LISTEN
 int    sock_type = ROAR_SOCKET_TYPE_UNKNOWN;
#endif
#ifdef ROAR_HAVE_LIBSLP
 int    reg_slp   = 0;
#endif
#ifdef ROAR_HAVE_CHROOT
 char * chrootdir = NULL;
#endif
#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
 struct group   * grp  = NULL;
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 struct passwd  * pwd  = NULL;
#endif
#ifdef ROAR_HAVE_GETSERVBYNAME
 struct servent * serv = NULL;
#endif
 DRIVER_USERDATA_T drvinst;
 struct roar_client * self = NULL;
#ifdef ROAR_HAVE_LIBDNET
 char decnethost[80];
#endif
#ifdef SUPPORT_PIDFILE
 struct roar_vio_calls pidfile_vio;
#endif

 ROAR_DBG("main(*): starting roard...");

 g_standby       =  0;
 g_autostandby   =  0;
 alive           =  1;
#ifdef ROAR_SUPPORT_LISTEN
 g_no_listen     =  0;
#else
 g_terminate     =  1;
#endif

 g_verbose       = ROAR_DBG_INFO_NONE;

 sa.bits     = ROAR_BITS_DEFAULT;
 sa.channels = ROAR_CHANNELS_DEFAULT;
 sa.rate     = ROAR_RATE_DEFAULT;
 sa.codec    = ROAR_CODEC_DEFAULT;

 g_sa        = &sa;
 g_max_sa    = &max_sa;

 memcpy(g_max_sa, g_sa, sizeof(max_sa));

 g_config = &config;

 if ( init_config() == -1 ) {
  ROAR_ERR("Can not init default config!");
  return 1;
 }

 // load config
 roar_libroar_get_config();

#ifdef ROAR_SUPPORT_LISTEN
 if ( init_listening() == -1 ) {
  ROAR_ERR("Can not init listening sockets!");
  return 1;
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 if ( midi_init_config() == -1 ) {
  ROAR_ERR("Can not init MIDI config!");
  return 1;
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 if ( ssynth_init_config() == -1 ) {
  ROAR_ERR("Can not init ssynth config!");
  return 1;
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 if ( rdtcs_init_config() == -1 ) {
  ROAR_ERR("Can not init RDTCS config!");
  return 1;
 }
#endif

#ifdef ROAR_SUPPORT_LISTEN
#ifndef ROAR_TARGET_WIN32
 sock_addr = ROAR_DEFAULT_SOCK_GLOBAL;
#else
 sock_addr = ROAR_DEFAULT_HOST;
#endif

#ifdef ROAR_HAVE_GETUID
 if ( getuid() != 0 && getenv("HOME") != NULL ) {
  snprintf(user_sock, 79, "%s/%s", (char*)getenv("HOME"), ROAR_DEFAULT_SOCK_USER);
  sock_addr = user_sock;
 }
#endif

 if ( getenv("ROAR_SERVER") != NULL )
  sock_addr = getenv("ROAR_SERVER");
#endif

 if ( clients_init() == -1 ) {
  ROAR_ERR("Can not init clients!");
  return 1;
 }

 if ( streams_init() == -1 ) {
  ROAR_ERR("Can not init streams!");
  return 1;
 }

 if ( (g_self_client = clients_new()) == -1 ) {
  ROAR_ERR("Can not create self client!");
  return 1;
 }

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 if ( sources_init() == -1 ) {
  ROAR_ERR("Can not init sources!");
  return 1;
 }

 if ( (sources_set_client(g_self_client)) == -1 ) {
  ROAR_ERR("Can not init set source client!");
  return 1;
 }
#endif

#ifdef ROAR_HAVE_MAIN_ARGS
 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "-h") == 0 || strcmp(k, "--help") == 0 ) {
   usage();
   return 0;

  } else if ( strcmp(k, "--start") == 0 ) {
   // this is a no op
  } else if ( strcmp(k, "--restart") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(sock_addr, 1) == -1 ) {
    ROAR_WARN("Can not terminate old server (not running at %s?), tring to continue anyway", sock_addr);
   }
#else
   ROAR_ERR("--restart not supported");
#endif
  } else if ( strcmp(k, "--shutdown") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(sock_addr, 1) == -1 ) {
    ROAR_WARN("Can not terminate old server (not running at %s?)", sock_addr);
    return 1;
   }
   return 0;
#else
   ROAR_ERR("--shutdown not supported");
   return 1;
#endif
  } else if ( strcmp(k, "--stop") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(sock_addr, 0) == -1 ) {
    ROAR_WARN("Can not stop old server (not running at %s?)", sock_addr);
    return 1;
   }
   return 0;
#else
   ROAR_ERR("--stop not supported");
   return 1;
#endif


  } else if ( strcmp(k, "--demon") == 0 || strcmp(k, "--daemon") == 0 ) {
#ifdef ROAR_HAVE_FORK
   daemon = 1;
#else
   ROAR_ERR("--daemon not supported");
#endif
  } else if ( strcmp(k, "--verbose") == 0 ) {
   g_verbose++;
  } else if ( strcmp(k, "--terminate") == 0 ) {
   g_terminate = 1;
  } else if ( strcmp(k, "--sysclocksync") == 0 ) {
   sysclocksync = 1000;
  } else if ( strcmp(k, "--realtime") == 0 ) {
   realtime++;
  } else if ( strcmp(k, "--chroot") == 0 ) {
#ifdef ROAR_HAVE_CHROOT
   chrootdir = argv[++i];
#else
   ROAR_ERR("--chroot not supported");
   i++;
#endif
  } else if ( strcmp(k, "--setgid") == 0 ) {
#ifdef ROAR_HAVE_SETGID
   setids |= R_SETGID;
#else
   ROAR_ERR("--setgid not supported");
#endif
  } else if ( strcmp(k, "--setuid") == 0 ) {
#ifdef ROAR_HAVE_SETUID
   setids |= R_SETUID;
#else
   ROAR_ERR("--setuid not supported");
#endif
  } else if ( strcmp(k, "--location") == 0 ) {
   g_config->location = argv[++i];
  } else if ( strcmp(k, "--pidfile") == 0 ) {
#ifdef SUPPORT_PIDFILE
   pidfile = argv[++i];
#else
   ROAR_ERR("--pidfile not supported");
   i++;
#endif

  } else if ( strcmp(k, "--list-cf") == 0 ) {
   print_codecfilterlist();
   return 0;

  } else if ( strcmp(k, "-R") == 0 || strcmp(k, "--rate") == 0 ) {
   sa.rate = atoi(argv[++i]);
  } else if ( strcmp(k, "-B") == 0 || strcmp(k, "--bits") == 0 ) {
   sa.bits = atoi(argv[++i]);
  } else if ( strcmp(k, "-C") == 0 || strcmp(k, "--chans") == 0 ) {
   sa.channels = atoi(argv[++i]);

  } else if ( strcmp(k, "--stream-flags") == 0 ) {
   if ( update_stream_flags(argv[++i]) == -1 ) {
    ROAR_ERR("Can not set stream flags");
    return 1;
   }

  } else if ( strcmp(k, "-d") == 0 || strcmp(k, "--driver") == 0 ) {
   driver = argv[++i];
   if ( strcmp(driver, "list") == 0 ) {
    ROAR_WARN("The option is obsolete, use --list-driver!");
    print_driverlist();
    return 0;
   }
  } else if ( strcmp(k, "-D") == 0 || strcmp(k, "--device") == 0 ) {
   device = argv[++i];
  } else if ( strcmp(k, "-dO") == 0 ) {
   opts = argv[++i];
  } else if ( strcmp(k, "--list-driver") == 0 ) {
   print_driverlist();
   return 0;

  } else if ( strcmp(k, "-o") == 0 || strcmp(k, "--odriver") == 0 ) {
   o_drv  = argv[++i];
  } else if ( strcmp(k, "-O") == 0 || strcmp(k, "--odevice") == 0 ) {
   o_dev  = argv[++i];
  } else if ( strcmp(k, "-oO") == 0 ) {
   o_opts = argv[++i];
  } else if ( strcmp(k, "-oP") == 0 ) {
   o_prim = 1;
  } else if ( strcmp(k, "-oN") == 0 ) {
   if ( add_output(o_drv, o_dev, o_opts, o_prim, o_count) != -1 )
    o_count++;

   o_drv  = o_dev = o_opts = NULL;
   o_prim = 0;

  } else if ( strcmp(k, "-s") == 0 || strcmp(k, "--source") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_drv = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "-S") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_dev = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "-sO") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_opt = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "-sC") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_con = argv[++i];
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "-sP") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   s_prim = 1;
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "-sN") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   if ( sources_add(s_drv, s_dev, s_con, s_opt, s_prim) == -1 ) {
    ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, s_drv);
   }
   s_opt = s_dev = s_con = NULL;
   s_drv = "cf";
   s_prim = 0;
#else
   ROAR_ERR("main(*): No support for sources compiled in");
#endif
  } else if ( strcmp(k, "--list-sources") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SOURCES
   print_sourcelist();
   return 0;
#else
   ROAR_ERR("main(*): No support for sources compiled in");
   return 1;
#endif

  } else if ( strcmp(k, "--light-channels") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
   light_channels = atoi(argv[++i]);
#else
   ROAR_WARN("main(*): no light subsystem compiled in");
#endif

  } else if ( strcmp(k, "--rds-pi") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   g_rdtcs.rds.pi = atoi(argv[++i]);
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif
  } else if ( strcmp(k, "--rds-ps") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_ps(argv[++i]) == -1 ) {
    ROAR_ERR("Can not set RDS PS to '%s' (longer than 8 chars?)", argv[i]);
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif
  } else if ( strcmp(k, "--rds-pty") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_pty(argv[++i]) == -1 ) {
    ROAR_ERR("Can not set RDS PTY to '%s'", argv[i]);
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif
  } else if ( strcmp(k, "--rds-tp") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_flag(RDTCS_RDS_FLAG_TP, 0) == -1 ) {
    ROAR_ERR("Can not set RDS TP flag");
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif
  } else if ( strcmp(k, "--rds-ct") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
   if ( rdtcs_rds_set_flag(RDTCS_RDS_FLAG_CT, 0) == -1 ) {
    ROAR_ERR("Can not set RDS CT flag");
    return 1;
   }
#else
   ROAR_WARN("main(*): no RDTCS subsystem compiled in");
#endif


  } else if ( strcmp(k, "--midi-no-console") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_CB
   midi_config.init_cb = 0;
#else
   // no warning here as this is the disable option
#endif
  } else if ( strcmp(k, "--midi-console-enable") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_CB
   midi_config.init_cb = 1;
#else
   ROAR_ERR("main(*): No support for MIDI subsystem part CB compiled in");
#endif
  } else if ( strcmp(k, "--midi-console") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_CB
   midi_config.console_dev = argv[++i];
   midi_config.init_cb = 1;
#else
   ROAR_ERR("main(*): No support for MIDI subsystem part CB compiled in");
#endif

  } else if ( strcmp(k, "--ssynth-enable") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
   ssynth_conf.enable = 1;
#else
   ROAR_ERR("main(*): No support for ssynth compiled in");
#endif
  } else if ( strcmp(k, "--ssynth-disable") == 0 ) {
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
   ssynth_conf.enable = 0;
#else
   // we can safely ignore the disable
#endif

  } else if ( strcmp(k, "-p") == 0 || strcmp(k, "--port") == 0 ) {
   // This is only usefull in INET not UNIX mode.
#ifdef ROAR_SUPPORT_LISTEN
   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;

   errno = 0;
   if ( (port = atoi(argv[++i])) < 1 ) {
#ifdef ROAR_HAVE_GETSERVBYNAME
    if ( (serv = getservbyname(argv[i], "tcp")) == NULL ) {
     ROAR_ERR("Unknown service: %s: %s", argv[i], strerror(errno));
     return 1;
    }
    // NOTE: we need to use ROAR_NET2HOST16() here even if s_port is of type int!
    ROAR_DBG("main(*): serv = {s_name='%s', s_aliases={...}, s_port=%i, s_proto='%s'}",
            serv->s_name, ROAR_NET2HOST16(serv->s_port), serv->s_proto);
    port = ROAR_NET2HOST16(serv->s_port);
#else
    ROAR_ERR("invalite port number: %s", argv[i]);
    return 1;
#endif
   }
#endif
  } else if ( strcmp(k, "-b") == 0 || strcmp(k, "--bind") == 0 || strcmp(k, "--sock") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   sock_addr = argv[++i];
#endif

  } else if ( strcmp(k, "--proto") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( (sock_proto = roar_str2proto(argv[++i])) == -1 ) {
    ROAR_ERR("Unknown protocol: %s", argv[i]);
    return 1;
   }
#endif

  } else if ( strcmp(k, "-t") == 0 || strcmp(k, "--tcp") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( sock_type != ROAR_SOCKET_TYPE_TCP && sock_type != ROAR_SOCKET_TYPE_TCP6 )
    sock_type = ROAR_SOCKET_TYPE_TCP;

   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;
#endif

  } else if ( strcmp(k, "-4") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   sock_type = ROAR_SOCKET_TYPE_TCP;
   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;
#endif
  } else if ( strcmp(k, "-6") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
#ifdef PF_INET6
   sock_type = ROAR_SOCKET_TYPE_TCP6;
   if ( *sock_addr == '/' )
    sock_addr = ROAR_DEFAULT_HOST;
#else
    ROAR_ERR("No IPv6 support compiled in!");
    return 1;
#endif
#endif

  } else if ( strcmp(k, "-u") == 0 || strcmp(k, "--unix") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   // ignore this case as it is the default behavor.
   sock_type = ROAR_SOCKET_TYPE_UNIX;
#endif

  } else if ( strcmp(k, "-n") == 0 || strcmp(k, "--decnet") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
#ifdef ROAR_HAVE_LIBDNET
    port   = ROAR_DEFAULT_NUM;
    strcpy(decnethost, ROAR_DEFAULT_LISTEN_OBJECT);
    sock_addr = decnethost;
    sock_type = ROAR_SOCKET_TYPE_DECNET;
#else
    ROAR_ERR("No DECnet support compiled in!");
    return 1;
#endif
#endif
  } else if ( strcmp(k, "--new-sock") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( add_listen(sock_addr, port, sock_type, sock_user, sock_grp, sock_proto) != 0 ) {
    ROAR_ERR("Can not open listen socket!");
    return 1;
   }
#endif

  } else if ( strcmp(k, "--slp") == 0 ) {
#ifdef ROAR_HAVE_LIBSLP
   reg_slp = 1;
#else
    ROAR_ERR("No OpenSLP support compiled in!");
    return 1;
#endif

  } else if ( strcmp(k, "--jumbo-mtu") == 0 ) {
   g_config->jumbo_mtu = atoi(argv[++i]);

  } else if ( strcmp(k, "-G") == 0 ) {
   sock_grp  = argv[++i];
  } else if ( strcmp(k, "-U") == 0 ) {
   sock_user = argv[++i];

  } else if ( strcmp(k, "--no-listen") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   sock_addr   = "";
   g_terminate = 1;
   g_no_listen = 1;
#endif
  } else if ( strcmp(k, "--client-fh") == 0 ) {
   if ( clients_set_fh(clients_new(), atoi(argv[++i])) == -1 ) {
    ROAR_ERR("main(*): Can not set client's fh");
    return 1;
   }
  } else if ( strcmp(k, "--close-fh") == 0 ) {
#ifdef ROAR_HAVE_IO_POSIX
   close(atoi(argv[++i]));
#else
   i++;
   ROAR_WARN("can not close file handle %s (closing not supported)", argv[i]);
#endif

  } else if ( strcmp(k, "--standby") == 0 ) {
   g_standby = 1;
  } else if ( strcmp(k, "--auto-standby") == 0 ) {
   g_autostandby = 1;
  } else {
   usage();
   return 1;
  }

 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 if ( s_dev != NULL ) {
  if ( sources_add(s_drv, s_dev, s_con, s_opt, s_prim) == -1 ) {
   ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, s_drv);
  }
 }
#endif

 add_output(o_drv, o_dev, o_opts, o_prim, o_count);

 ROAR_DBG("Server config: rate=%i, bits=%i, chans=%i", sa.rate, sa.bits, sa.channels);

 if ( waveform_init() == -1 ) {
  ROAR_ERR("Can not initialize Waveform subsystem");
  return 1;
 }

#ifndef ROAR_WITHOUT_DCOMP_MIDI
 if ( midi_init() == -1 ) {
  ROAR_ERR("Can not initialize MIDI subsystem");
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 if ( ssynth_init() == -1 ) {
  ROAR_ERR("Can not initialize ssynth subsystem");
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 if ( light_init(light_channels) == -1 ) {
  ROAR_ERR("Can not initialize light control subsystem");
 }
#endif

#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 if ( rdtcs_init() == -1 ) {
  ROAR_ERR("Can not initialize RDTCS subsystem");
 }
#endif

#ifdef ROAR_SUPPORT_LISTEN
 if ( add_listen(sock_addr, port, sock_type, sock_user, sock_grp, sock_proto) != 0 ) {
  ROAR_ERR("Can not open listen socket!");
  return 1;
 }
#endif

 if ( output_buffer_init(&sa) == -1 ) {
  ROAR_ERR("Can not init output buffer!");
  return 1;
 }

 if ( driver == NULL ) {
  driver = "null";
 } else {
  ROAR_WARN("Usage of old driver interface. use -o not -d!");
 }

 if ( driver_open(&drvinst, &drvid, driver, device, &sa) == -1 ) {
  ROAR_ERR("Can not open output driver!");
  return 1;
 }

 if ( samples_init() == -1 ) {
  ROAR_ERR("Can not init samples!");
  return 1;
 }


 // we should handle this on microcontrollers, too.
#if !defined(ROAR_TARGET_MICROCONTROLLER) && !defined(ROAR_TARGET_WIN32)
 signal(SIGINT,  on_sig_int);
 signal(SIGTERM, on_sig_term);
 signal(SIGCHLD, on_sig_chld);
 signal(SIGUSR1, on_sig_usr1);
 signal(SIGPIPE, SIG_IGN);  // ignore broken pipes
#endif

 if ( realtime ) {
#ifdef DEBUG
  ROAR_WARN("compiled with -DDEBUG but realtime is enabled: for real realtime support compiel without -DDEBUG");
#endif

#ifdef ROAR_HAVE_NICE
  errno = 0;
  nice(-5*realtime); // -5 for each --realtime
  if ( errno ) {
   ROAR_WARN("Can not decrease nice value by %i: %s", 5*realtime, strerror(errno));
  }
#else
  ROAR_WARN("Can not decrease nice value by %i: %s", 5*realtime, strerror(errno));
#endif
/*
#ifdef __linux__
  if ( ioprio_set(IOPRIO_WHO_PROCESS, getpid(), IOPRIO_PRIO_VALUE(IOPRIO_CLASS_BE, 0)) == -1 )
   ROAR_WARN("Can not set io priority: %s", strerror(errno));
#endif
*/
 }

#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_IO_POSIX)
 if ( setids & R_SETGID ) {
  if ( sock_grp == NULL ) {
   ROAR_ERR("Can not set GID if no groupname is supplied");
   return 1;
  }
  if ( (grp = getgrnam(sock_grp)) == NULL ) {
   ROAR_ERR("Can not get GID for group %s: %s", sock_grp, strerror(errno));
   return 1;
  }
  if ( setgroups(0, (const gid_t *) NULL) == -1 ) {
   ROAR_ERR("Can not clear supplementary group IDs: %s", strerror(errno));
  }
  if ( !grp || setgid(grp->gr_gid) == -1 ) {
   ROAR_ERR("Can not set GroupID: %s", strerror(errno));
  }
 }
#endif


 clients_set_pid(g_self_client, getpid());
#ifdef ROAR_HAVE_GETUID
 clients_set_uid(g_self_client, getuid());
#endif
#ifdef ROAR_HAVE_GETGID
 clients_set_gid(g_self_client, getgid());
#endif
 clients_get(g_self_client, &self);

 if ( self == NULL ) {
  ROAR_ERR("Can not get self client!");
  return 1;
 }

 strcpy(self->name, "RoarAudio daemon internal");

 if ( roar_nnode_free(&(self->nnode)) == -1 )
  return 1;

 // not fully correct but ok as workaorund
 // so tools assume that roard runs on the same machine as
 // they in case they use AF_UNIX:
 if ( roar_nnode_new(&(self->nnode), ROAR_SOCKET_TYPE_UNIX) == -1 )
  return 1;

#ifdef ROAR_HAVE_FORK
 if ( daemon ) {
  close(ROAR_STDIN );
  close(ROAR_STDOUT);
  close(ROAR_STDERR);

  if ( fork() )
   ROAR_U_EXIT(0);

#ifdef ROAR_HAVE_SETSID
  setsid();
#endif
  clients_set_pid(g_self_client, getpid()); // reset pid as it changed
 }
#endif

#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 // early test for UID as we need this for the pidfile and the setuid()
 if ( sock_user != NULL ) {
  if ( (pwd = getpwnam(sock_user)) == NULL ) {
   ROAR_ERR("Can not get UID for user %s: %s", sock_user, strerror(errno));
   return 1;
  }
 }
#endif

#ifdef SUPPORT_PIDFILE
 if ( pidfile != NULL ) {
  if ( roar_vio_open_file(&pidfile_vio, pidfile, O_WRONLY|O_CREAT, 0644) == -1 ) {
   ROAR_ERR("Can not write pidfile: %s", pidfile);
  } else {
   roar_vio_printf(&pidfile_vio, "%i\n", getpid());
   roar_vio_close(&pidfile_vio);
  }
#if defined(ROAR_HAVE_SETGID) && defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
  if ( pwd || grp ) {
   if ( chown(pidfile, pwd ? pwd->pw_uid : -1, grp ? grp->gr_gid : -1) == -1 ) {
    ROAR_WARN("Can not change ownership of pidfile: %s: %s", pidfile, strerror(errno));
   }
  }
  if ( chmod(pidfile, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH) == -1 ) {
   ROAR_WARN("Can not change permissions of pidfile: %s: %s", pidfile, strerror(errno));
  }
#endif
 }
#endif

#ifdef ROAR_HAVE_CHROOT
 if (chrootdir) {
  if ( chroot(chrootdir) == -1 ) {
   ROAR_ERR("Can not chroot to %s: %s", chrootdir, strerror(errno));
   return 2;
  }
  if ( chdir("/") == -1 ) {
   ROAR_ERR("Can not chdir to /: %s", strerror(errno));
   return 2;
  }
 }
#endif

#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
 if ( setids & R_SETUID ) {
  if ( sock_user == NULL ) {
   ROAR_ERR("Can not set UID if no username is supplied");
   return 1;
  }
  if ( !pwd || setuid(pwd->pw_uid) == -1 ) {
   ROAR_ERR("Can not set UserID: %s", strerror(errno));
   return 3;
  }
#ifdef ROAR_HAVE_GETUID
  clients_set_uid(g_self_client, getuid());
#endif
 }
#endif

 // Register with OpenSLP:
#ifdef ROAR_HAVE_LIBSLP
 if ( reg_slp ) {
  register_slp(0, sock_addr);
 }
#endif

 // start main loop...
 main_loop(drvid, drvinst, &sa, sysclocksync);

 // clean up.
 clean_quit_prep();
 driver_close(drvinst, drvid);
 output_buffer_free();

 return 0;
}

void cleanup_listen_socket (int terminate) {
 int i;

 // Deregister from SLP:
#ifdef ROAR_HAVE_LIBSLP
 register_slp(1, NULL);
#endif

#ifdef ROAR_SUPPORT_LISTEN
 for (i = 0; i < ROAR_MAX_LISTEN_SOCKETS; i++) {
  if ( g_listen_socket[i] != -1 ) {
#ifdef ROAR_HAVE_IO_POSIX
   close(g_listen_socket[i]);
#endif // #else is useless because we are in void context.

   g_listen_socket[i] = -1;

#ifdef ROAR_HAVE_UNIX
   if ( server[i] != NULL )
    if ( *(server[i]) == '/' )
     unlink(server[i]);
#endif
  }
 }

#endif

 if ( terminate )
  g_terminate = 1;
}

void clean_quit_prep (void) {
 cleanup_listen_socket(0);

#ifndef ROAR_WITHOUT_DCOMP_SOURCES
 sources_free();
#endif
 streams_free();
 clients_free();
#ifndef ROAR_WITHOUT_DCOMP_SSYNTH
 ssynth_free();
#endif
#ifndef ROAR_WITHOUT_DCOMP_CB
 midi_cb_stop(); // stop console beep
#endif
#ifndef ROAR_WITHOUT_DCOMP_MIDI
 midi_free();
#endif
#ifndef ROAR_WITHOUT_DCOMP_LIGHT
 light_free();
#endif
#ifndef ROAR_WITHOUT_DCOMP_RDTCS
 rdtcs_free();
#endif

 waveform_free();

#ifdef SUPPORT_PIDFILE
 if ( pidfile != NULL )
  unlink(pidfile);
#endif
}

void clean_quit (void) {
 clean_quit_prep();
// driver_close(drvinst, drvid);
// output_buffer_free();
 exit(0);
}

//ll
