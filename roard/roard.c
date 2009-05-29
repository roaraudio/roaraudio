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
char * server = ROAR_DEFAULT_SOCK_GLOBAL; // global server address
#endif

#ifdef ROAR_HAVE_MAIN_ARGS
void usage (void) {
 printf("Usage: roard [OPTIONS]...\n\n");

 printf("Misc Options:\n\n");
 printf(
        " --daemon              - Bring the server into background after init\n"
        " --terminate           - Terminate after last client quited\n"
        " --restart             - Trys to stop an old instance and start a new with new settings\n"
        " --realtime            - Trys to get realtime priority,\n"
        "                         give multible times for being more realtime\n"
        " --chroot DIR          - chroots to the given dir\n"
        " --setgid              - GroupID to the audio group as specified via -G\n"
        " --setuid              - UserID to the audio user as specified via -U\n"
        " --sysclocksync        - calculate exact sample rate using the system clock\n"
       );

 printf("\nAudio Options:\n\n");
 printf(
        " -R  --rate   RATE     - Set server rate\n"
        " -B  --bits   BITS     - Set server bits\n"
        " -C  --chans  CHANNELS - Set server channels\n"
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

 printf("\nSource Options:\n\n");
 printf(" -s  --source DRV      - Use DRV as input driver\n"
        " -S           DEV      - Use DEV as input device\n"
        " -sO          OPTS     - Use OPTS as input options\n"
        " -sN                   - Adds another source\n"
        " -sP                   - Make souce as primary\n"
       );

 printf("\nCodec Filter Options:\n\n");
 printf(" --list-cf             - List all codec filter\n"
       );

 printf("\nMIDI Options:\n\n");
 printf(" --midi-no-console     - Disable console based MIDI synth\n"
        " --midi-console DEV    - Set device for MIDI console\n"
       );

 printf("\nLight Control Options:\n\n");
 printf(" --light-channels NUM  - Sets the number of channels for Light control (default: %i)\n",
                                  LIGHT_CHANNELS_DEFAULT
       );

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
        " -G  GROUP             - Sets the group for the UNIX Domain Socket, (default: %s)\n"
        "                         You need the permittions to change the GID\n"
        " -U  USER              - Sets the user for the UNIX Domain Socket, (default: do not set)\n"
        "                         You need the permittions to change the UID (normaly only root has)\n"
        " --no-listen           - Do not listen for new clients\n"
        "                         (only usefull for relaing, impleys --terminate)\n"
        " --client-fh           - Comunicate with a client over this handle\n"
        "                         (only usefull for relaing)\n"
        " --close-fh            - Closes the given fh\n"
        " --standby             - Start in standby state\n"
        " --auto-standby        - Automatical goes into standby if there are no streams\n",
        ROAR_DEFAULT_SOCKGRP
       );
// printf("\n Options:\n\n");
 printf("\n");
}
#endif

int restart_server (char * server) {
 struct roar_connection con;
 if ( roar_connect(&con, server) == -1 ) {
  return -1;
 }

 if ( roar_terminate(&con, 1) == -1 ) {
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

 g_config->streams[ROAR_DIR_MIDI_OUT].flags = ROAR_FLAG_SYNC;

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
 int codec;
 int sync = 0;
 int32_t blocks = -1, blocksize = -1;
 int dir = ROAR_DIR_OUTPUT;

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

 if ( (stream = streams_new()) == -1 ) {
  ROAR_DBG("add_output(drv='%s', dev='%s', opts='%s') = -1", drv, dev, opts);
  if ( prim ) alive = 0;
  return -1;
 }

 streams_get(stream, &ss);
 s = ROAR_STREAM(ss);

 memcpy(&(s->info), g_sa, sizeof(struct roar_audio_info));

 s->pos_rel_id = -1;
// s->info.codec = codec;

 codec = s->info.codec;

 k = strtok(opts, ",");
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
   if ( (codec = roar_str2codec(v)) == -1 ) {
    ROAR_ERR("add_output(*): unknown codec '%s'", v);
    streams_delete(stream);
    if ( prim ) alive = 0;
#ifdef ROAR_DRIVER_CODEC
    if ( to_free != NULL )
     free(to_free);
#endif
    return -1;
   }
  } else if ( strcmp(k, "blocks") == 0 ) {
   blocks = atoi(v);
  } else if ( strcmp(k, "blocksize") == 0 ) {
   blocksize = atoi(v);
  } else if ( strcmp(k, "subsystem") == 0 ) {
   if ( !strcasecmp(v, "wave") || !strcasecmp(v, "waveform") ) {
    dir = ROAR_DIR_OUTPUT;
   } else if ( !strcasecmp(v, "midi") ) {
    dir = ROAR_DIR_MIDI_OUT;
   } else if ( !strcasecmp(v, "light") ) {
    dir = ROAR_DIR_LIGHT_OUT;
   } else {
    ROAR_ERR("add_output(*): unknown subsystem '%s'", k);
    streams_delete(stream);
    if ( prim ) alive = 0;
#ifdef ROAR_DRIVER_CODEC
    if ( to_free != NULL )
     free(to_free);
#endif
    return -1; 
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
  } else {
   ROAR_ERR("add_output(*): unknown option '%s'", k);
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

 if ( streams_set_dir(stream, dir, 1) == -1 ) {
  streams_delete(stream);
  return -1;
 }

#ifdef ROAR_DRIVER_CODEC
 if ( to_free != NULL )
  free(to_free);
#endif

 if ( codec == ROAR_CODEC_ALAW || codec == ROAR_CODEC_MULAW )
  s->info.bits = 8; // needed to open OSS driver, will be overriden by codecfilter

 s->info.codec = codec;
 ROAR_STREAM_SERVER(s)->codec_orgi = codec;

 if ( driver_openvio(&(ss->vio), &(ss->driver_id), drv, dev, &(s->info), -1) == -1 ) {
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

 ROAR_DBG("add_output(*): ss->driver_id=%i", ss->driver_id);

 streams_set_fh(stream, -1); // update some internal structures

 client_stream_add(g_source_client, stream);

 if ( prim ) {
  streams_mark_primary(stream);
  s->pos_rel_id = stream;
 }

 if ( sync ) {
  streams_set_flag(stream, ROAR_FLAG_SYNC);
 } else {
  streams_reset_flag(stream, ROAR_FLAG_SYNC);
 }

 return 0;
}

#ifdef ROAR_HAVE_MAIN_ARGS
int main (int argc, char * argv[]) {
#else
int main (void) {
#endif
#ifdef ROAR_HAVE_MAIN_ARGS
 int i;
 char * k;
#endif
#ifdef ROAR_SUPPORT_LISTEN
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
 int      port    = ROAR_DEFAULT_PORT;
#endif
 int               drvid;
 char * s_drv     = "cf";
 char * s_dev     = NULL;
 char * s_con     = NULL;
 char * s_opt     = NULL;
 int    s_prim    = 0;
 char * o_drv     = getenv("ROAR_DRIVER");
 char * o_dev     = getenv("ROAR_DEVICE");
 char * o_opts    = NULL;
 int    o_prim    = 0;
 int    o_count   = 0;
 int    light_channels = LIGHT_CHANNELS_DEFAULT;
 char * sock_grp  = ROAR_DEFAULT_SOCKGRP;
 char * sock_user = NULL;
#ifdef ROAR_SUPPORT_LISTEN
 int    sock_type = ROAR_SOCKET_TYPE_UNKNOWN;
#endif
#ifdef ROAR_HAVE_CHROOT
 char * chrootdir = NULL;
#endif
#if defined(ROAR_HAVE_SETGID) || defined(ROAR_HAVE_SETUID)
 int    setids    = 0;
#endif
#ifdef ROAR_HAVE_UNIX
 char * env_roar_proxy_backup;
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

 g_standby       =  0;
 g_autostandby   =  0;
 alive           =  1;
#ifdef ROAR_SUPPORT_LISTEN
 g_no_listen     =  0;
 g_listen_socket = -1;
#else
 g_terminate     =  1;
#endif

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

 if ( midi_init_config() == -1 ) {
  ROAR_ERR("Can not init MIDI config!");
  return 1;
 }

#ifdef ROAR_SUPPORT_LISTEN
#ifdef ROAR_HAVE_GETUID
 if ( getuid() != 0 && getenv("HOME") != NULL ) {
  snprintf(user_sock, 79, "%s/%s", (char*)getenv("HOME"), ROAR_DEFAULT_SOCK_USER);
  server = user_sock;
 }
#endif

 if ( getenv("ROAR_SERVER") != NULL )
  server = getenv("ROAR_SERVER");
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

 if ( sources_init() == -1 ) {
  ROAR_ERR("Can not init sources!");
  return 1;
 }

 if ( (sources_set_client(g_self_client)) == -1 ) {
  ROAR_ERR("Can not init set source client!");
  return 1;
 }

#ifdef ROAR_HAVE_MAIN_ARGS
 for (i = 1; i < argc; i++) {
  k = argv[i];

  if ( strcmp(k, "-h") == 0 || strcmp(k, "--help") == 0 ) {
   usage();
   return 0;

  } else if ( strcmp(k, "--restart") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( restart_server(server) == -1 ) {
    ROAR_WARN("Can not terminate old server (not running at %s?), tring to continue anyway", server);
   }
#else
   ROAR_ERR("--restart not supported");
#endif

  } else if ( strcmp(k, "--demon") == 0 || strcmp(k, "--daemon") == 0 ) {
#ifdef ROAR_HAVE_FORK
   daemon = 1;
#else
   ROAR_ERR("--daemon not supported");
#endif
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

  } else if ( strcmp(k, "--list-cf") == 0 ) {
   print_codecfilterlist();
   return 0;

  } else if ( strcmp(k, "-R") == 0 || strcmp(k, "--rate") == 0 ) {
   sa.rate = atoi(argv[++i]);
  } else if ( strcmp(k, "-B") == 0 || strcmp(k, "--bits") == 0 ) {
   sa.bits = atoi(argv[++i]);
  } else if ( strcmp(k, "-C") == 0 || strcmp(k, "--chans") == 0 ) {
   sa.channels = atoi(argv[++i]);

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
   s_drv = argv[++i];
  } else if ( strcmp(k, "-S") == 0 ) {
   s_dev = argv[++i];
  } else if ( strcmp(k, "-sO") == 0 ) {
   s_opt = argv[++i];
  } else if ( strcmp(k, "-sC") == 0 ) {
   s_con = argv[++i];
  } else if ( strcmp(k, "-sP") == 0 ) {
   s_prim = 1;
  } else if ( strcmp(k, "-sN") == 0 ) {
   if ( sources_add(s_drv, s_dev, s_con, s_opt, s_prim) == -1 ) {
    ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, s_drv);
   }
   s_opt = s_dev = s_con = NULL;
   s_drv = "cf";
   s_prim = 0;

  } else if ( strcmp(k, "--light-channels") == 0 ) {
   light_channels = atoi(argv[++i]);

  } else if ( strcmp(k, "--midi-no-console") == 0 ) {
   midi_config.init_cb = 0;
  } else if ( strcmp(k, "--midi-console") == 0 ) {
   midi_config.console_dev = argv[++i];

  } else if ( strcmp(k, "-p") == 0 || strcmp(k, "--port") == 0 ) {
   // This is only usefull in INET not UNIX mode.
#ifdef ROAR_SUPPORT_LISTEN
   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;

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
   server = argv[++i];
#endif

  } else if ( strcmp(k, "-t") == 0 || strcmp(k, "--tcp") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   if ( sock_type != ROAR_SOCKET_TYPE_TCP && sock_type != ROAR_SOCKET_TYPE_TCP6 )
    sock_type = ROAR_SOCKET_TYPE_TCP;

   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;
#endif

  } else if ( strcmp(k, "-4") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   sock_type = ROAR_SOCKET_TYPE_TCP;
   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;
#endif
  } else if ( strcmp(k, "-6") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
#ifdef PF_INET6
   sock_type = ROAR_SOCKET_TYPE_TCP6;
   if ( *server == '/' )
    server = ROAR_DEFAULT_HOST;
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
    server = decnethost;
    sock_type = ROAR_SOCKET_TYPE_DECNET;
#else
    ROAR_ERR("No DECnet support compiled in!");
    return 1;
#endif
#endif

  } else if ( strcmp(k, "-G") == 0 ) {
   sock_grp  = argv[++i];
  } else if ( strcmp(k, "-U") == 0 ) {
   sock_user = argv[++i];

  } else if ( strcmp(k, "--no-listen") == 0 ) {
#ifdef ROAR_SUPPORT_LISTEN
   server      = "";
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

 if ( s_dev != NULL ) {
  if ( sources_add(s_drv, s_dev, s_con, s_opt, s_prim) == -1 ) {
   ROAR_ERR("main(*): adding source '%s' via '%s' failed!", s_dev, s_drv);
  }
 }

 add_output(o_drv, o_dev, o_opts, o_prim, o_count);

 ROAR_DBG("Server config: rate=%i, bits=%i, chans=%i", sa.rate, sa.bits, sa.channels);

 if ( midi_init() == -1 ) {
  ROAR_ERR("Can not initialize MIDI subsystem");
 }

 if ( light_init(light_channels) == -1 ) {
  ROAR_ERR("Can not initialize light control subsystem");
 }

#ifdef ROAR_SUPPORT_LISTEN
 if ( *server != 0 ) {
  if ( (g_listen_socket = roar_socket_listen(sock_type, server, port)) == -1 ) {
#ifdef ROAR_HAVE_UNIX
   if ( *server == '/' ) {
    if ( (env_roar_proxy_backup = getenv("ROAR_PROXY")) != NULL ) {
     env_roar_proxy_backup = strdup(env_roar_proxy_backup);
     unsetenv("ROAR_PROXY");
    }
    if ( (i = roar_socket_connect(server, port)) != -1 ) {
     close(i);
     ROAR_ERR("Can not open listen socket!");
     return 1;
    } else {
     unlink(server);
     if ( (g_listen_socket = roar_socket_listen(sock_type, server, port)) == -1 ) {
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
  if ( (grp = getgrnam(sock_grp)) == NULL ) {
   ROAR_ERR("Can not get GID for group %s: %s", sock_grp, strerror(errno));
  }
#endif
#if defined(ROAR_HAVE_SETUID) && defined(ROAR_HAVE_IO_POSIX)
  if ( sock_user || (setids & R_SETUID) ) {
   if ( (pwd = getpwnam(sock_user)) == NULL ) {
    ROAR_ERR("Can not get UID for user %s: %s", sock_user, strerror(errno));
   }
  }
#endif

#if defined(ROAR_HAVE_IO_POSIX) && defined(ROAR_HAVE_UNIX)
  if ( *server == '/' ) {
   if ( grp ) {
    if ( pwd ) {
     if ( chown(server, pwd->pw_uid, grp->gr_gid) == -1 )
      return 1;
    } else {
     if ( chown(server, -1, grp->gr_gid) == -1 )
      return 1;
    }
#ifdef ROAR_HAVE_GETUID
    if ( getuid() == 0 )
     if ( chmod(server, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP) == -1 )
      return 1;
#endif
   }
  }
#endif
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
 signal(SIGCHLD, on_sig_chld);
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

#ifdef ROAR_HAVE_SETGID
 if ( setids & R_SETGID ) {
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

#ifdef ROAR_HAVE_SETUID
 if ( setids & R_SETUID ) {
  if ( !pwd || setuid(pwd->pw_uid) == -1 ) {
   ROAR_ERR("Can not set UserID: %s", strerror(errno));
   return 3;
  }
#ifdef ROAR_HAVE_GETUID
  clients_set_uid(g_self_client, getuid());
#endif
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

#ifdef ROAR_SUPPORT_LISTEN
 if ( g_listen_socket != -1 ) {
#ifdef ROAR_HAVE_IO_POSIX
  close(g_listen_socket);
#endif // #else is useless because we are in void context.

  g_listen_socket = -1;

#ifdef ROAR_HAVE_UNIX
  if ( *server == '/' )
   unlink(server);
#endif
 }

#endif

 if ( terminate )
  g_terminate = 1;
}

void clean_quit_prep (void) {
 cleanup_listen_socket(0);

 sources_free();
 streams_free();
 clients_free();
 midi_cb_stop(); // stop console beep
 midi_free();
 light_free();
}

void clean_quit (void) {
 clean_quit_prep();
// driver_close(drvinst, drvid);
// output_buffer_free();
 exit(0);
}

//ll
